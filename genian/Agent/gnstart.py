#!/usr/bin/env python
# -*- coding: utf-8 -*-


import os, sys, signal, threading
import socket, struct
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))

import gnagent
from gnagent import GnThread, Shell, Dataservice

from gnconfigure import (
    NotifyDataPos,
)

import time
import gntools as tool

from Lib.networkinfo import NetworkInfo

from Lib.debug import set_logging
from Lib.debug import set_logging_parameter
from include.common import AgentInstallPathInfo
import include.constants as CONST
import Lib.platform_profile as pp

# dataservice
dataservice = None

EVENT_DATA_TYPE                 = 1
EVENT_DATA_NOTIFYID             = 9
        
# event Header
EVENTH_VER10                    = 0x01
EVENT_TYPE_KEEPALIVE            = 0x01
EVENT_TYPE_LOG                  = 0x02
EVENT_TYPE_NOTIFY                = 0x03

# keepalive & event port
EVENT_LISTEN_PORT                = 3871

PEER_TYPE_GN_WIN_AGENT          = 2

PEER_STAT_DOWN                  = 0
PEER_STAT_UP                    = 2
PEER_STAT_UNKNOWN               = 3

AGENT_STAT_SLEEPING             = 0
AGENT_STAT_WORKING              = 1

KEYSTR_NODEID                   = 'node.id'

class ProcmonThread(GnThread):
    ''''''
    def __init__(self, stop, interval):
        user_info = tool.get_account_info()
        self.path = AgentInstallPathInfo()

        """ /usr/local/Geni/Genian/Agent """
        self.agent_path = ''.join([self.path.get_agent_path(), "/", self.path.p["AGENT_DIR_NAME"]])

        """ gnagent.py """
        self.agent_name = self.path.get_agent_process_name()

        """ /usr/local/Geni/Genian/log """
        self.agent_log_path = ''.join([self.path.get_log_dir_path()])

        if user_info is None:
            LOG.info('User account is Null. Go Out.')
            self.user_name = None
        else:
            user_info = user_info.split(":")
            self.user_name = user_info[0]

        GnThread.__init__(self, 'procmon', stop, interval)

    def work(self):
        if self.user_name is None:
            LOG.info('User account is None. please check it.')
            return

        command = ''.join(["ps -ef | grep " + self.agent_name + " | grep -v grep | awk '{print $2}'"])
        pid = Shell.execute(command)

        if pid[1] == '':
            command = ''.join(['cd ' + self.path.get_agent_path()])
            pid = Shell.execute(command)
            command = ''.join(['/usr/bin/python ' + self.agent_path + '/' + self.agent_name + ' &'])
            pid = Shell.execute(command)
            LOG.info('gnagent start. pid=' + pid[1])

class ChangeIPThread(GnThread):
    def __init__(self, stop, interval):
        self.obj_network_info = NetworkInfo()
        self.get_local_ip_info_conn = tool.get_local_ip_connected
        self.soap_server_ip = tool.get_soap_server_ip_port()
        self.soap_server_port = 80

        ip_info = self.get_local_ip_info_conn(self.soap_server_ip, self.soap_server_port)
        self.myip = ip_info[0]
        self.myadd = ip_info[1]

        GnThread.__init__(self, 'changeip', stop, interval)

    def work(self):
        ''''''
        ip_list = self.get_local_ip_info_conn(self.soap_server_ip, self.soap_server_port)
        
        if self.myip == ip_list[0]:
            return
        
        LOG.info('Local IP Changed. ORG=' + self.myip + ', NEW=' + ip_list[0])
        info_msg = 'The network interface to communicate with the center has changed. ({} => {})'.join([self.myip, ip_list[0]])
        LOG.info(info_msg)
        
        self.myip = ip_list[0]
        self.myadd = ip_list[1]
        
        # local ip 변경시 즉시 재인증 처리
        send_named_pipe(gnagent.NOTIFYID_REGISTER_REQ, '')


class Keepalive(GnThread):
    def __init__(self, nodeid, stop, timeout, is_unittest=False):
        """
        리눅스 에이전트에서 로컬 IP를 구하기 위해서 시도하는 최대 횟수 : 100 회
        """
        self.number_of_retries = 100

        """
        리눅스 에이전트에서 로컬 IP를 구하기 위해서 시도할 때 대기할 Sleep 시간(초) : 2초 
        100회 까지는 2초 이며 이후는 120초 로 설정됨.
        """
        self.retry_sleep_time = 2

        self.hsocket = None
        self.hpipe = None

        """
        EVENT_HEADER + EVENT_KEEPALIVE
        """
        self.keepaliveformat = 'BBHI48s48sII' + 'IBBBBI'

        """
        EVENT_HEADER + EVENT_NOTIFY
        """
        self.notify_format = 'BBHI48s48sII' + 'BBBBI'
        self.hkeepalive = [                     # keepalive 전송을 위한 struct
            EVENTH_VER10,                       # version
            EVENT_TYPE_KEEPALIVE,               # type
            0,                                  # keepalive length
            0,                                  # sequence number
            '',                                 # source node id
            '',                                 # dest node id
            0,                                  # source ipaddr
            0,                                  # dest ipaddr
            0,                                  # local ipaddr
            PEER_TYPE_GN_WIN_AGENT,             # node type
            PEER_STAT_UP,                       # peer stat
            AGENT_STAT_WORKING,                 # agent stat
            0,                                  # reserved
            0                                   # flags
        ]

        self.receive_buff_size = 70000
        self.obj_net = None
        self.get_local_ip_info_conn = None

        self.server_ip_ntoa = None
        self.server_ip_aton = None
        self.server_port = 3870

        self.retry_sleep_time = 60
        self.local_ip_str = None

        """
        정책서버와 통신하는 네트워크 인터페이스의 정보를 dict 형태로 저장.
        {
            'iface': 'enp3s0',
            'mac': '02:42:b0:55:ab:6e',
            'ip': '172.17.0.1',
            'gateway': '172.29.99.254'
        }
        """
        self.local_ip_dict = {}

        self.local_ip_by_network = None
        self.local_ip_by_host = None

        self.stop = None
        self.timeout = None
        self.is_unittest = None
        self.setup_keepalive(nodeid, stop, timeout, is_unittest)

    def setup_keepalive(self, nodeid, stop, timeout, is_unittest):
        self.obj_net = NetworkInfo()
        self.get_local_ip_info_conn = self.obj_net.get_local_ip_from_route
        self.setNodeid(nodeid)

        server_ip = tool.get_soap_server_ip_port()
        if server_ip is None or len(server_ip) == 0:
            LOG.info('The policy server address could not be found.')
            LOG.info('Set the local address (127.0.0.1).')
            server_ip = '127.0.0.1'

        self.server_ip_ntoa = server_ip
        self.server_ip_aton = struct.unpack('<L', socket.inet_aton(server_ip))[0]
        self.server_port = 3870

        local_ip = self.get_local_ip_info_conn()

        try_count = 0
        while not len(local_ip):
            LOG.info('Failed to collect local network information (IP).')
            LOG.info('Try again in the next cycle {0}. (after {1} seconds).'.format(try_count, self.retry_sleep_time))
            time.sleep(self.retry_sleep_time)
            local_ip = self.get_local_ip_info_conn()
            try_count += 1
            if try_count > self.number_of_retries:
                self.retry_sleep_time = 120

        for idx in range(len(local_ip)):
            if local_ip[idx].get('gateway'):
                self.local_ip_dict = local_ip[idx]
                break

        self.local_ip_by_network = struct.unpack('>L', socket.inet_pton(socket.AF_INET, self.local_ip_dict.get('ip')))[0]
        self.local_ip_by_host = socket.ntohl(self.local_ip_by_network)

        self.stop = stop
        self.timeout = timeout
        self.is_unittest = is_unittest

        if not self.is_unittest:
            GnThread.__init__(self, 'keepalive', self.stop, self.timeout)

        if self.socketInit(self.stop, self.timeout):
            LOG.info('The event (Keep Alive & Notify) setting'
                     ' was successfully initialized. (Local IP = {0})'.format(self.local_ip_dict.get('ip')))
            LOG.info('Start an Keepalive. (Keep Alive & Notify) (Interval = {0})'.format(self.timeout))
        else:
            LOG.info('Failed to create event (Keep Alive & Notify) socket.')

    def socketInit(self, stop, timeout):
        ''''''
        try:
            self.hsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        except socket.error as e:
            self.hsocket = None
            LOG.info('Failed to create event (Keep Alive & Notify) socket. (ERRMSG={0})'.format(str(e)))
            return

        try:
            self.hsocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        except socket.error as e:
            LOG.info('setsockopt() failed. (ERRMSG={0})'.format(str(e)))
            return

        try:
            self.hsocket.bind(('', EVENT_LISTEN_PORT))
        except socket.error as e:
            LOG.info('bind() failed. (ERRMSG={0})'.format(str(e)))
            return

        self.hkeepalive[6] = self.local_ip_by_network
        self.hkeepalive[7] = self.server_ip_aton
        self.hkeepalive[8] = self.local_ip_by_network

        if self.is_unittest:
            return True

        try:
            thevent = threading.Thread(target=self.notify, args=(stop,))
            thevent.start()
            LOG.info('The event reception job has started.')
            return True
        except Exception as e:
            LOG.info('Failed to create thread(Keep Alive & Notify). (ERRMSG={0})'.format(e))
            return

    def strNotify(self, notifyid):
        if notifyid == gnagent.NOTIFYID_REGISTER_REQ:          return '재인증'
        if notifyid == gnagent.NOTIFYID_PMSNOW:                return 'PMS 수행'
        if notifyid == gnagent.NOTIFYID_ACTIONNOW:             return '액션 수행'
        if notifyid == gnagent.NOTIFYID_SYSCONSTNOW:           return '제약사항 수행'
        if notifyid == gnagent.NOTIFYID_SMSNOW:                return 'SMS 수행'
        if notifyid == gnagent.NOTIFYID_AGENTDONOW:            return 'PMS/액션/제약사항/SMS/라이센스 검사(PDMC) 수행'
        if notifyid == gnagent.NOTIFYID_AGENTRUNSTAT:          return 'PMS/액션/라이센스 검사(PDMC) 작업 상태'
        if notifyid == gnagent.NOTIFYID_NEWNOTICE:             return '새로운 공지사항 존재'
        if notifyid == gnagent.NOTIFYID_NEWMESSAGE:            return '새로운 알림 메시지'
        if notifyid == gnagent.NOTIFYID_NODEAUTHCHANGED:       return '인증 상태 변경'
        if notifyid == gnagent.NOTIFYID_SENDDEBUGFILE:         return '지정된 목적지로 디버그 로그 전송'
        if notifyid == gnagent.NOTIFYID_SELECTACTIONNOW:       return '지정 액션 수행'
        if notifyid == gnagent.NOTIFYID_AGENTSTART:            return '에이전트 서비스 시작'
        if notifyid == gnagent.NOTIFYID_AGENTSTOP:             return '에이전트 서비스 중지'
        if notifyid == gnagent.NOTIFYID_AGENTUPDATE:           return '에이전트 업데이트'
        if notifyid == gnagent.NOTIFYID_RECOVERY:              return '센서 장애복구'
        if notifyid == gnagent.NOTIFYID_PERMMESSAGE:           return '고정메시지 업데이트'
        if notifyid == gnagent.NOTIFYID_NODEPOISONSTATUS:      return '노드차단상태'
        if notifyid == gnagent.NOTIFYID_LICSWPERMMESSAGE:      return '라이센스 고정메시지 업데이트'

    def setNodeid(self, nodeid):
        ''''''
        self.nodeid = nodeid

    def notify(self, stop):
        """
        정책서버로부터 이벤트 수신하는 쓰레드 함수.
        """
        while not self.stop.is_set():
            recvdata, addr = self.hsocket.recvfrom(self.receive_buff_size)
            if len(recvdata) == 0:
                continue

            if len(recvdata) == 124:
                notifydata = struct.unpack(self.keepaliveformat, recvdata)
                if notifydata[EVENT_DATA_TYPE] == EVENT_TYPE_KEEPALIVE:
                    continue
            elif len(recvdata) == 120:
                notifydata = struct.unpack(self.notify_format, recvdata)
                if int(notifydata[EVENT_DATA_TYPE]) == EVENT_TYPE_NOTIFY:
                    LOG.info('You have received to Notify Event. (SequenceNumber = {0}, ID = {1})\n'.format(socket.ntohl(notifydata[3]), notifydata[9]))
                    try:
                        msg = hex(socket.ntohs(notifydata[NotifyDataPos.EVENT_DATA_NOTIFYID_FLAG]))
                        send_named_pipe(notifydata[EVENT_DATA_NOTIFYID], msg)
                        LOG.info('The received event (Notify) was successfully sent to the GnAgent. (ID = %s, %d)' % (str(notifydata[9]), notifydata[10]))
                    except Exception as e:
                        LOG.info('Failed to send received event (Notify) to GnAgent. (Error = %s, ID = %s)' % (str(e), str(notifydata[9])))

    def work(self):
        netinfo_socket = self.obj_net.get_local_ip_from_socket(self.server_ip_ntoa, self.server_port)

        if netinfo_socket is None:
            send_named_pipe(gnagent.NOTIFYID_REGISTER_REQ, "0x101")
            return True

        if self.send_keep_alive():
            LOG.info('Keep Alive check the operation status The event transmission was successful.')
        else:
            LOG.info('Keep Alive Activity state check Event transfer failed.')

    def send_keep_alive(self):
        """
        KeepAlive 전송하는 쓰레드 함수.
        """
        self.setNodeid(dataservice.get_data(KEYSTR_NODEID))
        self.hkeepalive[4] = str(self.nodeid)
        self.hkeepalive[10] = PEER_STAT_UP

        structKeepalive = struct.pack(self.keepaliveformat, *self.hkeepalive)

        try:
            if self.hsocket.sendto(structKeepalive, (self.server_ip_ntoa, self.server_port)):
                return True
        except Exception as e:
            LOG.info("Keepalive send fail. ERRMSG=" + str(e))

    def finish(self):
        if len(self.nodeid) == 0:
            LOG.info('No node ID assigned.')
            self.setNodeid('')

        self.hkeepalive[4] = str(self.nodeid)
        self.hkeepalive[10] = PEER_STAT_DOWN
        structKeepalive = struct.pack(self.keepaliveformat, *self.hkeepalive)
        self.hsocket.sendto(structKeepalive, (self.server_ip_ntoa, self.server_port))

        self.hsocket.close()
        LOG.info('Stop the Keepalive thread. (Keep Alive & Notify)')

    def check_connect_local_ip(self):
        net_info = self.obj_net.get_local_ip_from_route()
        local_ip_ret = {}
        str_local_ip = None
        for el in net_info:
            if el['gateway']:
                str_local_ip = el['ip']
                local_ip_ret = el
                break
        if str_local_ip in self.local_ip_dict['ip']:
            return False
        else:
            self.local_ip_dict = local_ip_ret
            return True


def send_named_pipe(eventid, msg):
    ''''''
    try:
        pipedata = [
             eventid,            # event id
             0                   # msg size
         ]
        
        pipeformat = 'II'
        if len(msg) != 0:
            pipedata = [
                 eventid,            # event id
                 0,                  # msg size
                 ''                  # msg 
             ]
            
            pipeformat = pipeformat + str(len(msg)) + 's'
            pipedata[1] = len(msg)
            pipedata[2] = msg

        data = struct.pack(pipeformat, *pipedata)
        path = AgentInstallPathInfo()
        hpipe = os.open(''.join([path.get_agent_path(), '/', gnagent.NAMEDPIPE_AGENT]), os.O_WRONLY)
        os.write(hpipe, data)
        os.close(hpipe)
        LOG.info('The received event (Notify) was successfully sent to the GnAgent.')
    except Exception as e:
        LOG.info('Failed to send received event (Notify) to GnAgent. (Error = %s)' % (str(e)))
        raise


def signal_handler(signum, f):
    ''''''
    LOG.fatal('Signal received. SIG=' + str(signum))
    stop_event.set()


def main(args_list, LOG):
    ''''''
    LOG.info('GnStart start')
    
    # signal
    signal.signal(signal.SIGINT, signal_handler)
    
    global dataservice
    dataservice = Dataservice()
    
    ret = tool.get_soap_server_ip_port()
    if len(ret) is None or ret is None:
        LOG.info('Serverip not found. GnStart exit')
        sys.exit()

    nodeid = dataservice.get_data(KEYSTR_NODEID)
    
    global stop_event
    stop_event = threading.Event()
    
    threads = []

    if '-s' in args_list:
        LOG.info("You should running process GnAgent.")
    else:
        thprocmon = ProcmonThread(stop_event, 3)
        thprocmon.start()
        threads.append(thprocmon)

    keepaliveinterval = dataservice.get_data(CONST.KEYSTR_NODE_KEEPALIVE)
    if not len(keepaliveinterval):
        keepaliveinterval = 120
    thkeepalive = Keepalive(nodeid, stop_event, int(keepaliveinterval))
    thkeepalive.start()

    threads.append(thkeepalive)
    
    signal.pause()
    
    # 모든 thread 가 종료될때까지 대기
    for t in threads:
        if t is None:
            continue
        t.stopwait()
        t.join()
        
    dataservice.close()
    
    LOG.info('GnStart finish')
    
    sys.exit()


if __name__ == '__main__':

    os_info = [pp.get_os_name(), pp.get_os_version()]
    if "centos" in os_info[0] and "6." in os_info[1]:
        """
        현재 프로세스를 데몬으로 생성하는 작업을 함.
        2회 fork()를 통해서 데몬으로 생성함.
        """
        pid = os.fork()

        if pid > 0:
            exit(0)
        else:
            os.chdir('/usr/local/Geni/Genian')
            os.setsid()
            os.umask(0)

            pid = os.fork()
            if pid > 0:
                exit(0)
            else:
                os.chdir('/usr/local/Geni/Genian')
                sys.stdout.flush()
                sys.stderr.flush()


                """
                stdin, stdout, stderr fd를 /dev/null로 리다이렉션 함.
                """
                si = open(os.devnull, 'r')
                so = open(os.devnull, 'a+')
                se = open(os.devnull, 'a+')

                os.dup2(si.fileno(), sys.stdin.fileno())
                os.dup2(so.fileno(), sys.stdout.fileno())
                os.dup2(se.fileno(), sys.stderr.fileno())

    LOG = set_logging_parameter(sys.argv)

    """
    pid 파일은 CentOS 6.x(gnstart.centos-6)에서는 프로세스 중지할 때 참조 함.
    """
    with open("/tmp/Genian.pid", "w") as pid_file:
        pid_file.write(str(os.getpid()))

    main(sys.argv, LOG)



