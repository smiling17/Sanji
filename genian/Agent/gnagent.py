# -*- coding: utf-8 -*-
# 한글 주석 사용 가능하도록 encoding 추가

import sys
import os

sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))

import platform
import signal
import errno
import json
import hashlib
import select
import socket
import struct
import sqlite3
import subprocess
import threading
import xml.etree.ElementTree as ET
from datetime import timedelta
import Queue
import getpass
import time
import datetime
import shutil
import logging

from gnprocess import get_proc_list
import gntools

from gnconfigure import (
    NotifyAgentDoNowFlag,
    notify_q,
    agent_notify_id_dict,
    NotifyIdRegisterReqFlag,
)

from gnschedule import GnScheduleThread
from Lib.notifyifchange import net_if_change_check
from Lib.networkinfo import NetworkInfo
from Lib.debug import set_logging, get_log, set_logging_parameter
import Lib.encoding as encoding
import Lib.platform_profile as platform_profile
from include.common import (
    PATH_VERIFY,
    PATH_VERIFY_TRUNC,
    AgentInstallPathInfo,
    plugin_name,
)
import include.constants as CONST
import Lib.crypto as crypto
import Lib.fileversioninfo as version

# namedpipe
NAMEDPIPE_AGENT = 'agent_pipe'
NAMEDPIPE_START = 'start_pipe'

SIZE_UPDATE_MAX = 200
# Check Tool
IDS_CHECK_TOOL = "command -v "
IDS_DEVNULL = " >/dev/null"

ID_COMMAND = 0
ID_COMMAND_OPT_1 = 1
ID_COMMAND_OPT_2 = 2

# SOAP Function ID
ID_SOAP_LOGON = 0
ID_SOAP_GETMYPROFILE = 1
ID_SOAP_UPDATEINFO = 2
ID_SOAP_PROFILERESULT = 3
ID_SOAP_INSTALLINFO = 4
ID_SOAP_CUSTOMINFO = 5

# SOAP Function ID
IDS_SOAP = [
    "logon",
    "getmyprofile",
    "updateinfo",
    "profileresult",
    "installinfo",
    "custominfo",
]

# System application ID
ID_COMM = 0

# System application string
IDS_COMM_WGET = "wget"
IDS_COMM_CURL = "curl"

ID_RESULT_FAIL = 0
ID_RESULT_SUCCESS = 1

ID_RESULT = 0
ID_RESULT_STR = 1

soapStub = {
    ID_SOAP_LOGON: [
        "urn:logon",
        "nodeid",
        "mac",
        "macspoof",
        "hwstr",
        "name",
        "platform",
        "osid",
        "locale",
        "ipaddr"
    ],
    ID_SOAP_GETMYPROFILE: [
        "urn:getmyprofile",
        "nodeid",
        "requst",
        "version"
    ],
    ID_SOAP_UPDATEINFO: [
        "urn:update-information",
        "infolist"
    ],
    ID_SOAP_PROFILERESULT: [
        "urn:profileresult",
        "nodeid",
        "err",
        "pfres"
    ],
    ID_SOAP_INSTALLINFO: [
        "urn:installinfo",
        "nodeid",
        "ipaddr",
        "swtype",
        "newinstall"
    ],
    ID_SOAP_CUSTOMINFO: [
        "urn:getcustominfo",
        "ipaddr",
    ],
}

# HTTP Header String
HTTP_HEAD = """' --no-cache -H "Content-Type:text/xml;charset=UTF-8" -H "SOAPAction: """

# SOAP String
SOAP_HEAD = """<soapenv:Envelope xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" xmlns:urn="urn:ctdagt">\
<soapenv:Header/>\
<soapenv:Body>\
"""
SOAP_TAIL = """</soapenv:Body>\
</soapenv:Envelope>
"""

AGENT_ACTION_INTERVAL = 0       # 액션수행 설정 - 수행주기
AGENT_ACTION_PLUGININFO = 1     # 액션수행 설정 - 플러그인 정보
AGENT_ACTION_PLUGINCONF = 2     # 액션수행 설정 - 플러그인 설정

PLUGIN_ACTION_INTERVALTYPE = 'action-intervaltype'  # 수행주기 옵션
PLUGIN_ACTION_INTERVAL = 'action-interval'          # 수행주기
PLUGIN_ACTION_TIMEGROUP = 'action-timegroup'        # 수행시간대
PLUGIN_ACTION_RETRYTIME = 'action-retrytime'        # 재시도시간
PLUGIN_ACTION_RETRYLIMIT = 'action-retrylimit'      # 재시도회수
PLUGIN_ACTION_TIMETYPE = 'actiontime-type'
PLUGIN_ACTION_TIMEDATE = 'actiontime-date'
PLUGIN_ACTION_TIMEDOM = 'actiontime-dom'
PLUGIN_ACTION_TIMEDOW = 'actiontime-dow'
PLUGIN_ACTION_TIMEWOM = 'actiontime-wom'
PLUGIN_ACTION_TIMEDOW2 = 'actiontime-dow2'
PLUGIN_ACTION_TIMETIME = 'actiontime-time'

PLUGIN_ACTION_INTERVALTYPE_ONTIME = '1'     # 한번만 수행
PLUGIN_ACTION_INTERVALTYPE_FIXEDTIME = '2'  # 지정시각 수행
PLUGIN_ACTION_INTERVALTYPE_PERIODRUN = '3'  # 주기적 수행
PLUGIN_ACTION_INTERVALTYPE_ALWAYSRUN = '4'  # 항상 수행
PLUGIN_ACTION_INTERVALTYPE_OSSTART = '5'    # 윈도우 시작시 한번만 수행

# 에이전트 수행 결과 타입
GN_AGENT_RESULTTYPE_PMS = 0     # PMS
GN_AGENT_RESULTTYPE_ACTION = 1  # 액션
GN_AGENT_RESULTTYPE_SMS = 2     # SMS (사용안함)
GN_AGENT_RESULTTYPE_CONST = 3   # 제약사항 (사용안함)
GN_AGENT_RESULTTYPE_PMSHISTORY = 4  # PMS 내역

# 에이전트 수행 결과 코드
GN_AGENT_RESULTCODE_SUCCESS = 10        # 성공
GN_AGENT_RESULTCODE_GENERAL = 11        # 실패
GN_AGENT_RESULTCODE_NOTSUPPORTED = 12   # 기능 지원안함
GN_AGENT_RESULTCODE_NEEDREBOOT = 13     # 리부팅 필요

# 프로파일 작업 타입
GN_PROFILE_WORKTYPE_PMS = 1     # PMS
GN_PROFILE_WORKTYPE_ACTION = 2  # 액션
GN_PROFILE_WORKTYPE_CONST = 3   # 제약사항 (사용안함)
GN_PROFILE_WORKTYPE_ELASTICSEARCH = 4   # 액션수행결과를 Elasticsearch 로 전송

# 프로파일 결과
GN_PROFILE_RESULT_PFTYPE = "pftype"     # profileresult_t.pftype
GN_PROFILE_RESULT_PFID = "pfid"         # profileresult_t.pfid
GN_PROFILE_RESULT_PFGROUPID = "pfgorupid"       # profileresult_t.result_code
GN_PROFILE_RESULT_PFGROUPOP = "groupop"         # profileresult_t.result_msg
GN_PROFILE_RESULT_RESULTCODE = "result-code"    # profileresult_t.result_code
GN_PROFILE_RESULT_RESULTMSG = "result-msg"      # profileresult_t.result_msg
GN_PROFILE_RESULT_ACTIVE = "active"             # profileresult_t.active

KEYSTR_ACTION_RESULT = 'send.action.result'
KEYSTR_PLUGIN_RESULT = 'send.plugins.data'
KEYSTR_NODEID = 'node.id'
KEYSTR_NODE_NOUPDATE = 'node.aanoupdate'
KEYSTR_PROFILE_INTERVAL = 'profile.policyinterval'      # 정책 수신 주기
KEYSTR_PROFILE_AUTHSTATUS = 'profile.authstatus'
KEYSTR_PROFILE_VERSION = 'profile.version'

NOTIFYID_REGISTER_REQ = 11      # 재인증
NOTIFYID_PMSNOW = 36            # PMS 수행
NOTIFYID_ACTIONNOW = 37         # 액션 수행
NOTIFYID_SYSCONSTNOW = 38       # 제약사항 수행
NOTIFYID_SMSNOW = 39            # SMS 수행
NOTIFYID_AGENTDONOW = 44        # PMS/액션/제약사항/SMS/라이센스 검사(PDMC) 수행
NOTIFYID_AGENTRUNSTAT = 45      # PMS/액션/라이센스 검사(PDMC) 작업 상태
NOTIFYID_NEWNOTICE = 46         # 새로운 공지사항 존재
NOTIFYID_NEWMESSAGE = 53        # 새로운 알림 메시지
NOTIFYID_NODEAUTHCHANGED = 55   # 인증 상태 변경
NOTIFYID_SENDDEBUGFILE = 66     # 지정된 목적지로 디버그 로그 전송
NOTIFYID_SELECTACTIONNOW = 72   # 지정 액션 수행
NOTIFYID_AGENTSTART = 73        # 에이전트 서비스 시작
NOTIFYID_AGENTSTOP = 74         # 에이전트 서비스 중지
NOTIFYID_AGENTUPDATE = 75       # 에이전트 업데이트
NOTIFYID_RECOVERY = 76          # 센서 장애복구
NOTIFYID_PERMMESSAGE = 77       # 고정메시지 업데이트
NOTIFYID_NODEPOISONSTATUS = 81  # 노드차단상태
NOTIFYID_LICSWPERMMESSAGE = 85  # 라이센스 고정메시지 업데이트

LOCAL_NOTIFYID_AGENTUPDATE = 900   # 에이전트 주기적 업데이트

# timeobj
TIMEOBJ_ANYTIME = 'ⓐANYTIME'

# dataservice
dataservice = None

# threads
threads = {}

# stop event
stop_event = None

threadDelegate = None


class Shell:
    '''
    Shell
    '''
    @staticmethod
    def rm(name, is_dir=False):
        '''
        rm
        '''
        if (is_dir == True):
            command = ''.join(["rm -rf " + name + "/*"])
        else:
            command = ''.join(["rm -f " + name])

        list_result = Shell.execute(command)
        if (list_result[ID_RESULT] == ID_RESULT_FAIL):
            LOG.error("rm command fail. cmd='" + command + "'")

    @staticmethod
    def execute(command):
        '''
        execute
        '''
        popen = subprocess.Popen(
            command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        (stdoutdata, stderrdata) = popen.communicate()

        if (len(stderrdata) == 0):
            return [ID_RESULT_SUCCESS, stdoutdata]
        else:
            return [ID_RESULT_FAIL, stderrdata]


class Dataservice:
    ''''''
    conn = None

    def __init__(self):
        ''''''
        self.path = AgentInstallPathInfo()
        self.logger = logging.getLogger("crumbs")
        self.conn = sqlite3.connect(self.path.get_database_path(), isolation_level=None, check_same_thread=False)
        query = 'CREATE TABLE IF NOT EXISTS INFODATA (KEYPATH varchar(1024), VALUE Text, Primary Key(KEYPATH))'
        cur = self.conn.cursor()
        cur.execute(query)

    def close(self):
        self.conn.close()

    def get_data(self, keypath, default=''):
        ''''''
        data = [keypath]
        cur = self.conn.cursor()
        query = 'SELECT VALUE FROM INFODATA WHERE KEYPATH = ?'
        cur.execute(query, data)
        rows = cur.fetchall()
        for row in rows:
            if not row[0]:
                return default
            else:
                return row[0]
        return default

    def set_data(self, keypath, value):
        ''''''
        data = (keypath, value)
        cur = self.conn.cursor()
        query = 'INSERT OR REPLACE INTO INFODATA (KEYPATH, VALUE) VALUES (?, ?)'
        cur.execute(query, data)

    def del_data(self, keypath):
        ''''''
        data = [keypath]
        cur = self.conn.cursor()
        query = 'DELETE FROM INFODATA WHERE KEYPATH = ?'
        cur.execute(query, data)

    def delete_action_result_data(self):
        data = []
        cur = self.conn.cursor()
        query = 'select * from infodata where KEYPATH like "keypath%"'
        cur.execute(query, data)
        rows = cur.fetchall()

        for row in rows:
            if not row[0]:
                return None
            else:
                query = 'delete from infodata where KEYPATH=' + str('\'') + str(row[0].decode('utf-8') + str('\''))
                cur.execute(query, data)
        return None

class pluginbase(Shell):
    """
    profile_result : [0] = PFTYPE_?, [1] = result-code, [2] = result-msg
    """
    profile_result = {}

    """
    updateinfo result
    """
    list_data = []

    """
    plugin config
    """
    pluginconfig = None

    """
    action 수행결과가 변경되지 않더라도 결과 전송
    """
    always_profileresult_send = False

    def __init__(self):
        ''''''
        self.create_dicts()

    def create_dicts(self):
        ''''''
        pass

    def run(self):
        ''''''
        pass

    def check_tool(self, toolid):
        return (os.system(''.join([IDS_CHECK_TOOL, toolid, IDS_DEVNULL])) == 0)

    def set_profileresult(self, worktype, resultcode, resultmsg):
        ''''''
        self.profile_result[GN_PROFILE_RESULT_PFTYPE] = worktype
        self.profile_result[GN_PROFILE_RESULT_RESULTCODE] = resultcode
        self.profile_result[GN_PROFILE_RESULT_RESULTMSG] = resultmsg


class WaitableEvent:
    ''''''

    def __init__(self):
        ''''''
        self._read_fd, self._write_fd = os.pipe()

    def wait(self, timeout=None):
        ''''''
        rfds, wfds, efds = select.select([self._read_fd], [], [], timeout)
        return self._read_fd in rfds

    def isSet(self):
        ''''''
        return self.wait(0)

    def clear(self):
        ''''''
        if self.isSet():
            os.read(self._read_fd, 1)

    def set(self):
        ''''''
        if not self.isSet():
            os.write(self._write_fd, b'1')

    def fileno(self):
        '''
        Return the FD number of the read side of the pipe, allows this object to
        be used with select.select().
        '''
        return self._read_fd

    def __del__(self):
        ''''''
        os.close(self._read_fd)
        os.close(self._write_fd)


class GnThreadDelegate:
    '''
    별도의 parameter를 필요로하지 않는 작업들을 수행
    thread함수 내에서 수행되는 것들로 
    thread stopwait, join 등을 사용하지 않는다.
    '''

    def policyinterval(self, gnthread):
        if gnthread.name != 'policyinterval':
            return

        agentprofile()


    @staticmethod
    def policyrestart(interval):
        if threads == None:
            return

        thread = threads.get('policyinterval')
        interval = interval * 60

        if thread == None:
            thread = GnThread('policyinterval', stop_event,
                              interval, threadDelegate.policyinterval)
            thread.start()
            threads['policyinterval'] = thread
            dataservice.set_data(KEYSTR_PROFILE_INTERVAL, interval)
            return

        value = dataservice.get_data(KEYSTR_PROFILE_INTERVAL, '0')
        if interval != int(value):
            """
            thread내에서 호출되는 함수이므로 시간값만 변경 (분단위)
            """
            thread.timeout = interval
            dataservice.set_data(KEYSTR_PROFILE_INTERVAL, interval)


class GnThread(threading.Thread):
    ''''''
    name = None
    stop = None			# app 종료 이벤트
    timeout = None		# thread 시작 대기 시간
    isexit = False		# thread 중지 여부
    isrun = False
    isstart = False
    delegate = None

    def __init__(self, name, stop, timeout, delegate=None):
        ''''''
        self.name = name
        self.timeout = timeout
        self.stop = stop
        self.event = WaitableEvent()
        self.event.clear()
        self.isrun = False
        self.delegate = delegate
        self.log = get_log()

        threading.Thread.__init__(self, name=self.name)

    def __delete(self):
        threading.Thread.__delete(self)
        self.isstart = False

    def start(self):
        if not self.isstart:
            threading.Thread.start(self)
            self.isstart = True

    def stopwait(self):
        ''''''
        self.isexit = True
        self.event.set()

    def work(self):
        ''''''
        pass

    def finish(self):
        ''''''
        self.isrun = False
        self.isstart = False

    def run(self):
        ''''''
        while not self.stop.is_set() and self.isexit == False:
            ''''''
            if (self.timeout != 0):
                self.event.wait(self.timeout)

            if not self.stop.is_set() and self.isexit == False:
                try:
                    self.work()

                    if self.delegate != None:
                        self.delegate(self)

                except Exception as e:
                    self.log.error("Thread run failed. ERROR=" + str(e) + ", name=" + str(self.name))
                    raise

            # timeout = 0 이면 한반만 수행
            if (self.timeout == 0):
                break

        self.isrun = False
        self.isstart = False
        self.finish()


class SoapDelegate:
    '''
        Soap Response 패킷중 xml 부분에 대한 파싱 클래스.
    '''

    def extract_dataset(self, attrib):
        '''
        extract_dataset
        '''
        tree = ET.parse(attrib)
        root = tree.getroot()
        return root[1][0]

    def logon(self):
        '''
        logon
        '''
        try:
            dataset = self.extract_dataset(IDS_SOAP[ID_SOAP_LOGON])
            for d in dataset:
                ''''''
                if (d.tag == "nodeid"):
                    self.dict_env["nodeid"] = d.text
                if (d.tag == "result"):
                    ''''''
                    if (d.text == "0"):
                        LOG.info("LOGON SUCCEED, NODEID : " + self.dict_env["nodeid"])
                    else:
                        LOG.error("LOGON FAIL")
        except Exception as msg:
            LOG.error("Exception MSG=" + str(msg))

    def getmyprofile(self):
        """
        SOAP_GETMYPROFILE 에 대한 리턴 값으로 아래와 같은 XML Element 객체를 리턴 받아서 파싱한다.
        각 요소에 대해서 파싱하여 저장하지 않고 필요한 Element 만 파싱하여 저장한다.
        :return: XML Element Dictionary Objects
        """
        dataset = self.extract_dataset(IDS_SOAP[ID_SOAP_GETMYPROFILE])

        self.dict_env['aalist'] = {}
        self.dict_env['tolist'] = {}

        LOG.debug('agent profile received')

        for d in dataset:
            ''''''
            tag = ''
            text = ''

            if d == None:
                continue
            tag = '' if type(d.tag) != str else str(d.tag)
            text = '' if type(d.text) != str else str(d.text)

            if d.tag == "policyinterval":
                # 정책변경 검사주기 (시간)
                GnThreadDelegate.policyrestart(int(d.text))
            if d.tag == "kainterval":
                dataservice.set_data(CONST.KEYSTR_NODE_KEEPALIVE, d.text)

            elif d.tag == "aalist":
                ''''''
                for x in d:
                    ''''''
                    if x.tag == "pfid":
                        ''''''
                        action_elem = {}
                        action_plugin = {}
                        pluginconf = {}
                        self.dict_env['aalist'][x.text] = [
                            action_elem, 			# 수행주기
                            action_plugin, 			# plugin 정보
                            pluginconf]			# plugin 설정
                    elif x.tag == "action-plugin":
                        ''''''
                        for y in x:
                            action_plugin[y.tag] = y.text
                    elif x.tag == "pluginconf":
                        ''''''
                        pluginkey = ""
                        for y in x:
                            ''''''
                            if y.tag == "key":
                                ''''''
                                pluginkey = y.text
                            elif pluginkey:
                                pluginconf[pluginkey] = y.text
                                pluginkey = ""

                    else:
                        action_elem[x.tag] = x.text
            elif d.tag == "tolist":
                ''''''
                for x in d:
                    if x.tag == 'name':
                        timeobj = {}
                        self.dict_env['tolist'][x.text] = timeobj
                    else:
                        timeobj[x.tag] = x.text
            elif d.tag == "aa-noupdate":
                '''
                aa-noupdate 정보값을 db에 저장.
                '''
                self.dict_env['aa-noupdate'] = d.text
                dataservice.set_data(KEYSTR_NODE_NOUPDATE, self.dict_env.pop('aa-noupdate'))
            elif d.tag == "version":
                dataservice.set_data(KEYSTR_PROFILE_VERSION, d.text)

    def updateinfo(self):
        '''
        updateinfo
        '''
        pass

    def profileresult(self):
        '''
        profileresult
        '''
        pass

    def installinfo(self):
        '''
        installinfo soap api를 통해서 서버에 있는 에이전트 관련 전체정보를 받아와서 리눅스 에이전트 관련 정보만 추출한다.
        '''
        self.dict_env['installinfo'] = {}
        dataset = self.extract_dataset(IDS_SOAP[ID_SOAP_INSTALLINFO])
        for vinfos in dataset.findall('vinfo'):
            if "GenianLinux" == vinfos.find('modulename').text:
                vinfo_list = list(vinfos)
                for i in vinfo_list:
                    self.dict_env['installinfo'][i.tag] = i.text

    def custominfo(self):
        self.dict_env['custominfo'] = {}
        dataset = self.extract_dataset(IDS_SOAP[ID_SOAP_CUSTOMINFO])

        results = []
        for el in dataset:
            results.append((el.tag, el.text))

        self.dict_env['custominfo'] = results

    def __init__(self):
        ''''''
        self.dict_env = {
        }


class Soap:
    '''	
        Soap Request 생성하는 클래스.(wget명령으로 전송하기 위한 body, xml 생성)
    '''

    def create_delegate(self):
        '''
        create_delegate
        '''
        self.delegate = SoapDelegate()
        self.delegate.dict_env = self.dict_env
        self.dict_soap[ID_SOAP_LOGON] = self.delegate.logon
        self.dict_soap[ID_SOAP_GETMYPROFILE] = self.delegate.getmyprofile
        self.dict_soap[ID_SOAP_UPDATEINFO] = self.delegate.updateinfo
        self.dict_soap[ID_SOAP_PROFILERESULT] = self.delegate.profileresult
        self.dict_soap[ID_SOAP_INSTALLINFO] = self.delegate.installinfo
        self.dict_soap[ID_SOAP_CUSTOMINFO] = self.delegate.custominfo

    def create_dicts(self):
        '''
        create_dicts
        '''
        self.dict_soap = {}
        self.dict_env = {}

    def set_url(self):
        '''
        set_url
        '''
        self.soap_url = "http://" + gntools.get_soap_server_ip_port() + "/agt"

    def create_xml(self, soap_body):
        '''
        create_xml
        '''
        return ''.join([SOAP_HEAD, soap_body, SOAP_TAIL])

    def add_sub_element(self, parent, elem_name, elem_text=None):
        '''
        add_sub_element
        '''
        elem = ET.SubElement(parent, elem_name)
        elem.text = elem_text
        return elem

    def create_elements(self, soap_id, parent, elem, params, keyquery=""):
        '''
        create_elements
        '''
        if (soap_id == ID_SOAP_UPDATEINFO):
            ''''''
            if (elem == "infolist"):
                ''''''
                for paramlist in params:
                    ''''''
                    for param in paramlist:
                        ''''''
                        sub = self.add_sub_element(parent, elem)
                        self.add_sub_element(sub, "nodeid", self.dict_env.get("nodeid"))
                        self.add_sub_element(sub, "infoid", param[0])
                        self.add_sub_element(sub, "keyquery", keyquery)
                        # 2 = ILF_HAVESYSINFO
                        self.add_sub_element(sub, "flags", "2")

                        list_column = param[1]
                        for column in list_column:
                            self.add_sub_element(sub, "columnlist", column)

                        list_data = param[2]
                        for list_sub_data in list_data:
                            ''''''
                            child = self.add_sub_element(sub, "datasetlist")
                            for data in list_sub_data:
                                self.add_sub_element(child, "dataset", data)
                            self.add_sub_element(child, "flags", "0")

        elif soap_id == ID_SOAP_PROFILERESULT:
            ''''''
            if elem == "nodeid":
                self.add_sub_element(parent, elem, self.dict_env.get("nodeid"))
            elif elem == "err":
                ''''''
                sub = ET.SubElement(parent, elem)
                self.add_sub_element(sub, "type", str(GN_AGENT_RESULTTYPE_ACTION))
                self.add_sub_element(sub, "errorcode", str(GN_AGENT_RESULTCODE_SUCCESS))
                self.add_sub_element(sub, "errormsg", "")
                self.add_sub_element(sub, "nexttime", "0")
            elif elem == "pfres":
                ''''''
                for paramlist in params:
                    sub = ET.SubElement(parent, elem)
                    for p, v in paramlist.items():
                        ''''''
                        self.add_sub_element(sub, p, str(v))
        else:
            self.add_sub_element(parent, elem, params)

    def call(self, soap_id, params, keyquery=""):
        '''	
        call
        '''
        delegate = self.dict_soap.get(soap_id)

        first = True
        i = 0
        for elem in soapStub[soap_id]:
            ''''''
            if (first):
                ''''''
                root = ET.Element(elem)
                parent = root
                if soap_id == ID_SOAP_LOGON:
                    parent = ET.SubElement(root, IDS_SOAP[ID_SOAP_LOGON])
                first = False
            else:
                ''''''
                if soap_id == ID_SOAP_UPDATEINFO or soap_id == ID_SOAP_PROFILERESULT:
                    self.create_elements(soap_id, parent, elem, params, keyquery)
                else:
                    self.create_elements(soap_id, parent, elem, params[i])
                i = i + 1

        xml = self.create_xml(ET.tostring(root))
        cmd = self.tool.dict_tool[ID_COMM]
        param = self.tool.dict_cmd[ID_COMM][cmd]

        soapfile = 'soapfile' + str(datetime.datetime.now().strftime('%Y%m%d%H%M%S%f'))

        sendfile = open(soapfile, 'w')
        sendfile.write(xml)
        sendfile.close()

        str_list = [
            param[ID_COMMAND],
            soapfile, HTTP_HEAD,
            delegate.__name__,
            '" ',
            self.soap_url,
            param[ID_COMMAND_OPT_1],
            delegate.__name__
        ]

        request = ''.join(str_list)

        LOG.info("Request CMD=" + str(request))

        list_result = Shell.execute(request)
        os.unlink(soapfile)

        if list_result[ID_RESULT] == ID_RESULT_SUCCESS:
            ''''''
            delegate()
            Shell.rm(IDS_SOAP[soap_id])
            return ID_RESULT_SUCCESS
        return ID_RESULT_FAIL

    def __init__(self, tool):
        '''
        __init__
        '''
        self.create_dicts()
        self.create_delegate()
        self.tool = tool
        self.set_url()


class Tool:
    '''
    Tool
    '''

    def create_dicts(self):
        '''	
        create_dicts
        '''
        self.dict_cmd = {
            ID_COMM: {
                IDS_COMM_WGET: ["wget -q --post-file='", " -O "],
                IDS_COMM_CURL: ["curl -s --data '", " -o "]
            }
        }

        self.dict_tool = {
            ID_COMM: None
        }

    def create_tool(self):
        '''
        create_tool
        '''
        for tool_id, cmds in self.dict_cmd.items():
            '''
            '''
            for cmd in cmds:
                ''''''
                if os.system(''.join([IDS_CHECK_TOOL, cmd, IDS_DEVNULL])) != 0:
                    continue
                self.dict_tool[tool_id] = cmd
                break

        result = True
        for tool_id, cmd in self.dict_tool.items():
            ''''''
            if cmd == None:
                ''''''
                result = False

        return result

    def __init__(self):
        '''
        __init__
        '''
        self.create_dicts()


class Util:
    ''''''
    @staticmethod
    def weekofmonth(year, month, day):
        yearhigh = year / 100
        yearlow = year % 100

        if month <= 2:
            yearlow -= 1
            month += 12

        week = (((21*yearhigh/4) + (5*yearlow/4) + (26*(month+1)/10) + 1 - 1) % 7)
        return ((day-1+week)/7) if week > 3 else ((day-1+week)/7)+1

    @staticmethod
    def lastweekofmonth(year, month):
        days = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
        leap = 0

        if (2 == month) and (0 == (year % 4) and 0 != (year % 100) or 0 == (year % 400)):
            leap = 1
        return Util.weekofmonth(year, month, days[month-1]+leap)

    @staticmethod
    def islastweekofmonth(year, month, day):
        wom = Util.weekofmonth(year, month, day)
        if wom > 0:
            wom = pow(2, wom - 1)
            if Util.lastweekofmonth(year, month) == wom:
                return wom | 0x00000020
        return wom


class Node:
    def __init__(self):
        self.net_if = {}
        self.obj_network_info = None
        self.soap_server_ip = None
        self.soap_server_port = None
        self.net_if_keys = ['ipstr', 'ip', 'name', 'mac']
        self.local_ip_ntoa = None
        self.local_ip_aton = None
        self.ip_addr = None
        self.mac_addr = None
        self.nodeid = None
        self.hostname = None
        self.platform = None
        self.osid = None
        self.lang = None

    def create_net_info(self):
        local_ip_list = self.obj_network_info.get_local_ip_from_socket(self.soap_server_ip, self.soap_server_port)

        if local_ip_list is None:
            return None

        for idx in range(len(local_ip_list)):
            self.net_if[self.net_if_keys[idx]] = local_ip_list[idx]

        return True

    def create_node_info(self):
        if self.create_net_info():
            self.ip_addr = str(self.net_if["ip"])
            self.mac_addr = self.net_if["mac"]
        else:
            self.ip_addr = '0.0.0.0'
            self.mac_addr = 0

        self.hostname = socket.gethostname()
        self.platform = platform.system()
        self.lang = encoding.get_system_lang()[0]
        self.osid = platform_profile.get_system_osid()
        LOG.info("HOST NAME={0}, PLATFORM={1}, LANG={2}, OSID={3}".format(self.hostname, self.platform, self.lang, self.osid))

    def node_info_setup(self):
        """
        각종 설정과 외부 함수 호출에 대해서 __init__함수와 분리하여 처리한다.
        """
        self.obj_network_info = NetworkInfo()
        self.soap_server_ip = gntools.get_soap_server_ip_port()
        self.soap_server_port = 80

        ip_info = self.obj_network_info.get_local_ip_from_socket(self.soap_server_ip, self.soap_server_port)
        if ip_info is None:
            return

        self.local_ip_ntoa = ip_info[0]
        self.local_ip_aton = ip_info[1]


class GnNotify(GnThread):
    """
    GnNotify 객체의 Method Name을 agent_notify_id_dict{} 에서 사용하므로, Method Name 변경시
    agent_notify_id_dict{} 에서도 함께 변경 되어야 한다.
    """
    def __init__(self, name, stop, interval, sch_q, is_unittest=None):
        self.interval = interval
        self.jobs = notify_q
        self.sch = sch_q
        self.case_name = ""
        self.case = self.case_default
        self.noti_id = (0,)
        self.obj_network_info = None
        self.local_ip_netinfo = []
        self.log = get_log()

        if is_unittest is None:
            GnThread.__init__(self, name, stop, self.interval)

    def work(self):
        while True:
            try:
                notifyId = self.jobs.get()
                result = self.NotifyReceive(notifyId)
            finally:
                self.jobs.task_done()

    def setup_network_info(self):
        self.obj_network_info = NetworkInfo()

        for el in self.obj_network_info.get_local_ip_from_route():
            if el.get('gateway'):
                self.local_ip_netinfo = el
                break

    def AgentActionDoNow(self):
        """
        에이전트 액션 즉시 수행
        plugin 중지
        """
        hplugin = threads['pluginmgr']
        hplugin.del_agentaction()

        agentlogon()
        if node.nodeid == None:
            ''''''
            LOG.error("NodeID doesn't exist.")
            agentexit()
            return

        agentprofile()

        # plugin 재설정
        hplugin.set_agentaction(soap.dict_env['aalist'], soap.dict_env['tolist'])
        hplugin.start()

    def AgentDoNow(self, id):
        '''
        노드대상작업지시 명령어중 즉시 수행 관련 명령.
        '''

        if int(id, 16) & NotifyAgentDoNowFlag.NOTYFLAGS_ACTIONNOW:
            """
            에이전트 액션 즉시 수행
            """
            LOG.info('Agent service event (Notify) received that is Perform agent action immediately.')
            self.AgentActionDoNow()

        if int(id, 16) & NotifyAgentDoNowFlag.NOTYFLAGS_PMSNOW:
            """
            운영체제 업데이트 즉시 수행
            """
            LOG.info('Agent service event (Notify) received that is Immediately update operating system. Not implemented.')

        if int(id, 16) & NotifyAgentDoNowFlag.NOTYFLAGS_SMSNOW:
            """
            노드 자산정보 즉시전송
            """
            LOG.info('Agent service event (Notify) received that is Immediate transfer of node asset information.')
            self.sch.put(schedule_q, NotifyAgentDoNowFlag.NOTYFLAGS_PMSNOW)

        if int(id, 16) & NotifyAgentDoNowFlag.NOTYFLAGS_PROFILE:
            """
            노드정책 재적용
            """
            LOG.info('Agent service event (Notify) received that is Reapply the node policy.')
            agentprofile()

    def NotifyReceive(self, notify_id):
        """
        pythonic하게 switch문 구현함.
        :param notifyId : <type 'tuple'>: (44, '0x400')
        :return: NotifyAgentDoNowFlag ID 값에 맵핑되는 함수의 주소를 리턴함.
        """
        try:
            self.case_name = "".join([agent_notify_id_dict.get(notify_id[0])])
        except TypeError as e:
            LOG.info('Agent raise exception ERRMSG=' + str(e))
            self.case_name = "default"

        if self.case_name in ("register_req", "agent_do_now"):
            """
            에이전트 수행결과 관련 DB 값을 삭제함.
            """
            dataservice.delete_action_result_data()

        self.case = getattr(self, self.case_name, self.case_default)
        self.noti_id = notify_id
        return self.case()

    def register_req(self):
        """
        에이전트 재등록 요청
        noti_id : (event_id, event_msg)
        event_id : 11(gnagent.NOTIFYID_REGISTER_REQ)
        event_msg : 0x0 : 정책서버로부터오는 이벤트
                    0x100 : notifyifchange 모듈에서 인터페이스 변경시 발생.
                    0x101 : KeepAlive 전송하기 전에 네트워크 체크시 실패하면 발생함.
        """
        LOG.info('GnAgent received a center reconnection request message.')
        netinfo = self.obj_network_info.get_local_ip_from_route()

        if not len(netinfo):
            LOG.info("Could not find network information. NOTIFY_ID=" + str(self.noti_id))
            return True

        result = {}
        for el in netinfo:
            if el.get('gateway'):
                result = el

        if self.noti_id[1] in "0x100, 0x101":
            if result.get('ip') in self.local_ip_netinfo.get('ip'):
                LOG.info('There are no changes to the local IP. ( Local IP = %s, IF_NAME = %s)' % (self.local_ip_netinfo.get('ip'), self.local_ip_netinfo.get('iface')))
                return True

        self.local_ip_netinfo = result

        agentlogon()
        if not len(node.nodeid):
            """
            Center Logon  실패시 추가 작업이 필요함.
                - 센터 재 접속 타이머 설정
                - 에이전트 failsafe 타이머 설정
            """
            LOG.error("NodeID doesn't exist.")
            return False
        agentprofile()

    def agent_update(self):
        """
        에이전트 업데이트
        """
        LOG.info('Agent service update event (Notify) received.')
        add_jobs(jobs_q, NOTIFYID_AGENTUPDATE)

    def agent_do_now(self):
        """
        에이전트 액션 즉시 수행
        """
        LOG.info('Agent service event (Notify) received that is Perform agent action immediately.')
        self.AgentDoNow(self.noti_id[1])

    def node_auth_changed(self):
        """
        에이전트 인증 상태 변경
        """
        LOG.info('Agent service event (Notify) received that is Changing agent authentication status.')
        agentprofile()

    def case_default(self):
        """
        에이전트 이벤트 예외 처리를 위한 케이스
        """
        LOG.info('An unknown event has been received. (Notify ID = %s,Flage = %s)' % (self.noti_id, self.case_name))


class Namedpipe(GnThread):
    ''''''
    name = None
    interval = None
    hpipe = None

    def __init__(self, name, stop, interval, notify_q):
        self.path = AgentInstallPathInfo()

        self.name = ''.join([self.path.get_agent_path(), "/", name])
        self.interval = interval
        self.stop_event = stop
        self.jobs = notify_q

        try:
            Shell.rm(self.name, False)
            os.mkfifo(self.name, 0o666)
            os.chmod(self.name, 0o666)
            self.hpipe = os.open(self.name, os.O_RDWR | os.O_NONBLOCK)
        except OSError as e:
            LOG.error('Failed to create event socket. (ERROR = %s)' % (str(e)))

        GnThread.__init__(self, name, stop_event, self.interval)

    def work(self):
        LOG.info('The event reception job has started.')
        while not self.stop_event.is_set() and self.isexit == False:
            try:
                rlist = [self.hpipe]
                wlist = []
                xlist = []
                rlist, wlist, xlist = select.select(rlist, wlist, xlist)
                if self.hpipe in rlist:
                    try:
                        buf = os.read(self.hpipe, 8)
                    except OSError as e:
                        if e.errno == errno.EAGAIN or e.errno == errno.EWOULDBLOCK:
                            buf = None
                        else:
                            LOG.info('Event reception failed. (ERROR = %s)' % (str(e)))
                            raise

                    '''
                    함수가 주기적으로 실행되지 않는다.
                    select()함수 추가로 인하여 리턴하는 코드를 삭제한다
                    '''
                    if buf is None or len(buf) == 0:
                        continue

                    pipeformat = 'II'
                    # msgtype 과 msg 내용의 길이 조회
                    data = struct.unpack(pipeformat, buf)

                    if data[0] == 0:
                        continue

                    if data[1] > 0:
                        buf = os.read(self.hpipe, data[1])
                        pipeformat = str(data[1]) + 's'
                        flag = struct.unpack(pipeformat, buf)

                    eventid = data[0]
                    notifys = (eventid, flag[0])
                    add_jobs(self.jobs, notifys)

            except OSError as e:
                if e.errno == errno.EAGAIN or e.errno == errno.EWOULDBLOCK or e.errno == errno.EINTR:
                    LOG.info("Select.error exception ERR=" + str(e))
                else:
                    LOG.error("Named Pipe exception ERR=" + str(e))
                    raise

    def finish(self):
        ''''''
        os.close(self.hpipe)
        Shell.rm(self.name, False)


class PluginResultsendThread(GnThread):
    ''''''
    interval = 10

    def __init__(self, stop, interval):
        ''''''
        self.interval = interval
        GnThread.__init__(self, 'Plugin_Result_sender', stop, self.interval)
        LOG.info(''.join(["Plugin Result Sender success. INTERVAL=%d" % self.interval]))

    def work(self):
        ''''''
        data = dataservice.get_data(KEYSTR_PLUGIN_RESULT)
        if len(data) == 0:
            return
        buf = json.loads(data)

        keyquery = ''
        result = soap.call(ID_SOAP_UPDATEINFO, buf, keyquery)
        if result != ID_RESULT_SUCCESS:
            ''''''
            LOG.debug('Update Info send failed.')
            return

        dataservice.del_data(KEYSTR_PLUGIN_RESULT)
        LOG.debug('Update Info send success.')

        # action result send
        data = dataservice.get_data(KEYSTR_ACTION_RESULT)
        if len(data) == 0:
            return
        buf = json.loads(data)

        keyquery = ''
        result = soap.call(ID_SOAP_PROFILERESULT, buf, keyquery)
        if result != ID_RESULT_SUCCESS:
            ''''''
            LOG.debug('Action Result send failed.')
            return
        dataservice.del_data(KEYSTR_ACTION_RESULT)
        LOG.debug('Action Result send success.')


class PluginThread(GnThread):
    ''''''
    actionid = 0
    pluginid = 0
    modulename = ''
    pluginname = ''
    pluginconf = None
    timeobject = None

    stop = None
    module_plugin = None
    module_class = None

    config = []

    def __init__(self, actionid, pluginid, pluginname, pluginconf, stop):
        self.actionid = actionid
        self.pluginid = pluginid
        self.modulename = None
        self.pluginname = pluginname
        self.pluginconf = pluginconf

        self.stop = stop

        for each in plugin_name:
            if str(self.pluginid) in each[0]:
                self.modulename = "Plugin" + "." + "ACT" + str(self.pluginid) + "_" + each[1] + "." + pluginname
                break

        self.module_plugin = __import__(self.modulename, fromlist=[self.pluginname])
        self.module_class = getattr(self.module_plugin, self.pluginname)

    def pluginstart(self, config, timeobject):
        ''''''
        self.config = config
        self.timeobject = timeobject

        intervaltype = config[PLUGIN_ACTION_INTERVALTYPE]
        interval = 0

        if intervaltype == PLUGIN_ACTION_INTERVALTYPE_PERIODRUN:
            # 주기적 수행은 초단위
            interval = int(config[PLUGIN_ACTION_INTERVAL])
        elif intervaltype == PLUGIN_ACTION_INTERVALTYPE_FIXEDTIME:
            # 지정시간은 분단위로 구현
            interval = 60
        elif intervaltype == PLUGIN_ACTION_INTERVALTYPE_ALWAYSRUN:
            """
            수행 주기 : 항상 실행
            항상 수행으로 설정할 경우, 수행 주기는 1초로 설정함.
            """
            interval = 1

        GnThread.__init__(self, self.pluginname, self.stop, interval)

        LOG.info("Plugin init success. ACTIONID=" + str(self.actionid) +
                 ", PLUGINID=" + str(self.pluginid) +
                 ", NAME=" + self.pluginname +
                 ", INTERVALTYPE=" + intervaltype +
                 ", INTERVAL=" + str(interval))

    def immediate(self):
        ''''''
        thimmediate = threading.Thread(target=self.work, args=())
        thimmediate.start()

    def inschedule(self):
        ''''''
        # schedule 에서는 초단위는 계산하지 않음
        # 현재 시간
        nowdate = datetime.datetime.now().timetuple()
        intervaltype = self.config[PLUGIN_ACTION_INTERVALTYPE]

        try:
            if intervaltype == PLUGIN_ACTION_INTERVALTYPE_ONTIME:
                # 한번만 수행은 쓰레드가 1회 호출되고 더이상 호출되지 않음
                return True
            elif intervaltype == PLUGIN_ACTION_INTERVALTYPE_OSSTART:
                # OS 시작시 수행
                # 직전 수행시간 조회 후 수행시간 정보가 없으면 무조건 수행
                keypath = 'keypath.' + str(self.actionid) + '.profile.rundatetime'
                rundate = dataservice.get_data(keypath)
                if not rundate:
                    return True

                # 이전 수행 시간
                olddate = datetime.datetime.strptime(
                    rundate, '%Y%m%d%H%M%S').timetuple()

                # 부팅시간
                result = Shell.execute("cat /proc/uptime | awk -F' ' '{print $1}'")
                boottime = (datetime.datetime.now() - timedelta(seconds=long(float(result[1].replace("\n", ''))))).timetuple()

                if boottime > olddate:
                    return True

            elif intervaltype == PLUGIN_ACTION_INTERVALTYPE_PERIODRUN or intervaltype == PLUGIN_ACTION_INTERVALTYPE_ALWAYSRUN:
                # 주기적 수행은 재시도 하지 않음
                timegrp = self.config[PLUGIN_ACTION_TIMEGROUP]
                timeobj = self.timeobject[timegrp]
                nowtime = nowdate.tm_hour * 60 + nowdate.tm_min

                if timegrp.encode('utf8') == TIMEOBJ_ANYTIME:
                    return True

                # 월,화,수,목,금,토,일
                wday = [2, 4, 8, 16, 32, 64, 1]

                if (int(timeobj['day']) & wday[nowdate.tm_wday]) == wday[nowdate.tm_wday]:
                    if int(timeobj['fromtime']) <= nowtime and nowtime <= int(timeobj['totime']):
                        return True
                return False

            elif intervaltype == PLUGIN_ACTION_INTERVALTYPE_FIXEDTIME:
                # 지정시각 수행
                timetype = int(self.config[PLUGIN_ACTION_TIMETYPE])
                timetime = self.config[PLUGIN_ACTION_TIMETIME].split(':')
                hour = int(timetime[0])
                minute = int(timetime[1])

                retval = False

                if timetype == 1:
                    # 매일+시각
                    retval = True
                elif timetype == 2:
                    # 지정일+시각
                    values = self.config[PLUGIN_ACTION_TIMEDOM].split(',')
                    timedom = []

                    for val in values:
                        vals = val.split('-')
                        start = vals[0]
                        end = vals[1] if len(vals) == 2 else vals[0]
                        timedom.extend(range(int(start), int(end)+1))

                    for day in timedom:
                        if nowdate.tm_day == day:
                            retval = True
                            break
                elif timetype == 3:
                    # 날짜+시각
                    value = self.config[PLUGIN_ACTION_TIMEDATE]
                    timedate = datetime.datetime.strptime(value, '%Y-%m-%d').timetuple()

                    if nowdate.tm_year == timedate.tm_year and nowdate.tm_mon == timedate.tm_mon and nowdate.tm_mday == timedate.tm_mday:
                        retval = True
                elif timetype == 4:
                    # 요일+시각, windows 기준으로 일=0 으로 설정됨
                    values = self.config[PLUGIN_ACTION_TIMEDOW].split(',')
                    timedow = [int(6) if int(num) == 0 else int(num) - 1 for num in values]

                    for wday in timedow:
                        if nowdate.tm_wday == wday:
                            retval = True
                            break
                elif timetype == 5:
                    # 주요일+시각
                    values = self.config[PLUGIN_ACTION_TIMEWOM].split(',')
                    wom = Util.weekofmonth(
                        nowdate.tm_year, nowdate.tm_mon, nowdate.tm_mday)
                    lastwom = Util.islastweekofmonth(nowdate.tm_year, nowdate.tm_mon, nowdate.tm_mday)

                    for timewom in values:
                        if 6 == int(timewom) and lastwom > 5:
                            # last week
                            retval = True
                            break
                        elif wom == int(timewom):
                            retval = True
                            break

                    if retval == True:
                        # 주가 일치하더라도 주중요일이 일치해야함으로 retval 을 초기화
                        retval = False
                        values = self.config[PLUGIN_ACTION_TIMEDOW2].split(',')
                        timedow = [int(6) if int(num) == 0 else int(num) - 1 for num in values]

                        for wday in timedow:
                            if nowdate.tm_wday == wday:
                                retval = True
                                break

                # 오늘 수행여부와 관계없이 무조건 실행하지 않음
                if retval == False:
                    return False

                # 직전 수행시간 조회 후 수행시간 정보가 없으면 무조건 수행
                keypath = 'keypath.' + \
                    str(self.actionid) + '.profile.rundatetime'
                rundate = dataservice.get_data(keypath)

                if not rundate:
                    return True

                # 이전 수행 시간
                olddate = datetime.datetime.strptime(rundate, '%Y%m%d%H%M%S').timetuple()

                # 오늘 이전에 수행했으면 다시 수행
                nowday = datetime.datetime(nowdate.tm_year, nowdate.tm_mon, nowdate.tm_day, 0, 0, 0)
                oldday = datetime.datetime(olddate.tm_year, olddate.tm_mon, olddate.tm_day, 0, 0, 0)

                if oldday < nowday:
                    return True

                # 오늘 수행했는지 확인해서 이전 시간에 수행했으면 다시 수행
                if nowdate.tm_year == olddate.tm_year and nowdate.tm_mon == olddate.tm_mon and nowdate.tm_mday == olddate.tm_mday:
                    if (olddate.tm_hour < hour or (olddate.tm_hour == hour and olddate.tm_min < minute)):
                        return True

        except Exception as e:
            LOG.error("Action schedule parsing error. ACTIONID=" + str(self.actionid) +
                      ", PLUGINID=" + str(self.pluginid) +
                      ", INTERVALTYPE=" + str(intervaltype) +
                      ', ERROR=' + str(e))
            return False

        return False

    def work(self):
        ''''''
        if self.isrun == True:
            # already run
            return

        # 주기적 수행 조건 검사
        if self.inschedule() == False:
            return

        self.isrun = True

        plugin = self.module_class()
        plugin.pluginconfig = self.pluginconf
        plugin.run()

        """
        action 수행 결과
        """
        plugin.profile_result[GN_PROFILE_RESULT_PFID] = self.actionid
        plugin.profile_result[GN_PROFILE_RESULT_PFGROUPID] = ""
        plugin.profile_result[GN_PROFILE_RESULT_PFGROUPOP] = 0
        plugin.profile_result[GN_PROFILE_RESULT_ACTIVE] = 0


        keypath = 'keypath.' + str(self.actionid) + '.profile.result'
        storehash = dataservice.get_data(keypath)
        resulthash = hashlib.sha256(str(plugin.profile_result)).hexdigest()

        issend = (storehash != resulthash) or (plugin.always_profileresult_send)
        if issend == 1:
            ''''''
            data = dataservice.get_data(KEYSTR_ACTION_RESULT)
            if len(data) == 0:
                data = '[]'
            buf = json.loads(data)
            buf.append(plugin.profile_result)
            dataservice.set_data(KEYSTR_ACTION_RESULT, json.dumps(buf))

            dataservice.set_data(keypath, resulthash)

            # plugin 수행 시간 저장
            keypath = 'keypath.' + str(self.actionid) + '.profile.rundatetime'
            dataservice.set_data(keypath, datetime.datetime.now().strftime('%Y%m%d%H%M%S'))

            LOG.debug("Action result. ACTIONID=" + str(self.actionid) +
                  ", PLUGIN_NAME=" + self.pluginname +
                  ", PLUGINID=" + str(self.pluginid) +
                  ", ISSEND=" + str(issend) +
                  ', HASH=' + resulthash)


        keypath = 'keypath.' + str(self.actionid) + '.updateinfo.result'
        storehash = dataservice.get_data(keypath)
        resulthash = hashlib.sha256(str(plugin.list_data)).hexdigest()

        issend = (storehash != resulthash) or (plugin.always_profileresult_send)
        if issend == 1:
            ''''''
            data = dataservice.get_data(KEYSTR_PLUGIN_RESULT)
            if len(data) == 0:
                data = '[]'
            buf = json.loads(data)
            buf.append(plugin.list_data)
            dataservice.set_data(KEYSTR_PLUGIN_RESULT, json.dumps(buf))

            dataservice.set_data(keypath, resulthash)

            LOG.debug("Update info. ACTIONID=" + str(self.actionid) +
                  ", PLUGIN_NAME=" + self.pluginname +
                  ", PLUGINID=" + str(self.pluginid) +
                  ", ISSEND=" + str(issend) +
                  ', HASH=' + resulthash)

        self.isrun = False


class PluginManager(GnThread):
    ''''''
    plugininfo = {}
    plugin_threads = []

    def __init__(self, stop, timeout):
        GnThread.__init__(self, 'pluginmgr', stop, timeout)

    def add_plugin(self, pluginid, pluginname):
        self.plugininfo[pluginid] = pluginname

    def set_agentaction(self, agentaction, timeobject):
        # plugin별 thread 생성 후 대기
        for pfid, actions in agentaction.items():
            pid = int(actions[AGENT_ACTION_PLUGININFO]['key'])
            # plugin config
            pluginconf = actions[AGENT_ACTION_PLUGINCONF]
            # 수행주기 옵션
            intervaltype = actions[AGENT_ACTION_INTERVAL][PLUGIN_ACTION_INTERVALTYPE]
            pluginname = self.plugininfo[pid]

            try:
                thplugin = PluginThread(pfid, pid, pluginname, pluginconf, self.stop)
                thplugin.pluginstart(actions[AGENT_ACTION_INTERVAL], timeobject)
                self.plugin_threads.append(thplugin)
            except Exception as e:
                LOG.error("Plugin not found. ACTIONID=" + str(pfid) +
                          ", PLUGINID=" + str(pid) +
                          ", PLUGINNM=" + str(pluginname) +
                          ", ERROR='" + str(e) + "'")

    def del_agentaction(self):
        for t in self.plugin_threads:
            LOG.info('Plugin finish. PLUGINID=' + str(t.pluginid) + ', PLUGINNM=' + str(t.pluginname))
            t.stopwait()
            t.join()

        del self.plugin_threads[:]

    def start(self):
        ''''''
        for t in self.plugin_threads:
            t.start()

    def immediate(self):
        for t in self.plugin_threads:
            if t.isrun == True:
                LOG.info("Plugin already run. PLUGINID=" + str(t.pluginid) + ", PLUGINNM=" + str(t.pluginname))
            else:
                LOG.info("Plugin immediate run. PLUGINID=" + str(t.pluginid) + ", PLUGINNM=" + str(t.pluginname))
                t.immediate()

    def work(self):
        ''''''
        LOG.info('Plugin manage. COUNT=' + str(len(self.plugin_threads)))

    def finish(self):
        self.del_agentaction()


class GnUpdateRunner(GnThread):
    '''
    주기적으로 실행하면서 GnUpdate 인스턴스를 실행한다.
    interval : GnUpdate 인스턴스를 실행하는 주기(sec)
    '''

    def __init__(self, name, stop, interval):
        self.interval = interval
        self.jobs = jobs_q
        self.results = results_q

        GnThread.__init__(self, name, stop, self.interval)

    def work(self):
        while True:
            try:
                notifyId = self.jobs.get()
                result = self.check_for_update(notifyId)
                self.results.put(result)
            finally:
                self.jobs.task_done()

    def check_for_update(self, notifyId):

        vermgr = GnUpdate()
        vermgr.get_server_ip_port()
        if vermgr.search_local_ip() is None:
            LOG.info("Local IP lookup failed.")
            return False

        vermgr.check_update_network()
        vermgr.check_update_version()
        vermgr.install_product_info()

        if notifyId is NOTIFYID_AGENTUPDATE:
            vermgr.agent_update_for_action(False)
        else:
            vermgr.agent_update_for_action(True)

        if vermgr.nCompare is 1:
            vermgr.installing_updates()
            LOG.info(" INSTALLING UPDATES SUCCESSFUL.")
        else:
            LOG.info("Linux Agent do not [update].")

        return True


class GnUpdate(object):
    def __init__(self):
        self.server_ip = 0
        self.nCompare = 0
        self.netinfo = gntools.netinfo()
        self.path = AgentInstallPathInfo()
        try:
            self.user_info = gntools.get_account_info().split(':')
        except Exception as e:
            LOG.info("No account information was found. ERRMSG=" + str(e))
            self.user_info = ['', ]

    def check_install_agent(self):
        """
        GnStart의 설치 경로 확인.
            미 설치시에는 리눅스 에이전트를 재설치해야 한다.
        GnStart가 서비스로 등록되었는지 확인함.
            서비스로 미 등록시에 리눅스 에이전트를 재설치해야 한다.
        """
        if gntools.search('/etc/init.d', 'gnstart'):
            LOG.info("Registered to GnStart service successfully.")
        else:
            LOG.info("Failed to register with GnStart service.")
            return False

        if gntools.installed_agent_path(self.path.get_agent_path(), self.path.p["AGENT_DIR_NAME"]):
            LOG.info("GnStart is installed.")
        else:
            LOG.info("GnStart is not installed.")
            return False
        return True

    def restart_agent(self):
        pass

    def check_run_agent(self):
        """
        GnStart 프로세스의 실행 여부를 확인한다.
        """
        if len(self.user_info) == 0:
            self.user_info = ['', ]

        proc_lists = get_proc_list()
        user_proc_list = [x for x in proc_lists if x.user == self.user_info[0]]

        if user_proc_list:
            LOG.info("GnStart service started successfully.")
            return True
        else:
            LOG.info("Failed to start GnStart service.")

    def search_local_ip(self):
        for el in self.netinfo:
            if el['gateway']:
                LOG.info("Local IP = " + str(el['ip']))
                return el['ip']
        LOG.info("Failed to retrieve local IP.")

    def get_server_ip_port(self):
        """
        정책서버 IP를 문자열로 저장 함.
        Agent/soapserver 파일에서 읽어옴.
        저장된 문자열 형식 : 172.29.99.251
        """
        try:
            fp = open(self.path.get_policy_server_address_file_path(), 'r')
            line = fp.readline()
            fp.close()
            self.server_ip = line.replace("\n", "", 1)
            return 1

        except Exception as e:
            LOG.error("Policy Server IP lookup failed. ERRMSG=" + str(e))
            return 0

    def check_update_network(self):
        """
        업데이트 가능한 네트워크 그룹인지 체크한다.
        0 - 업데이트 가능한 네트워크그룹이 전체임.
        1 - 업데이트 가능한 특정 네트워크 그룹이 있음.
        """
        agentprofile()

        noupdate = dataservice.get_data(KEYSTR_NODE_NOUPDATE)
        return noupdate

    def install_product_info(self):
        params = [
            node.ip_addr,
        ]

        try:
            soap.call(ID_SOAP_CUSTOMINFO, params)
            LOG.info('You have received agent custom information.')
        except Exception as e:
            LOG.info('Failed to receive custom information. ERR=%s' + str(e))
            raise

        custominfo = soap.dict_env.get("custominfo")

        for el in custominfo:
            if "productname" in el:
                dataservice.set_data("custominfo.productname", el[1])

            if "productcopyright" in el:
                dataservice.set_data("custominfo.productcopyright", el[1])

    def check_update_version(self):
        """
        installinfo를 통해서 서버에 있는 에이전트의 최신 버전 정보를 가져온다.
        """
        params = [
            node.nodeid,
            node.ip_addr,
            "",
            ""
        ]

        try:
            soap.call(ID_SOAP_INSTALLINFO, params)
            LOG.info('You have received agent installation information.')
        except Exception as e:
            LOG.info('Failed to receive installation information. ERR=%s', str(e))
            raise

        installinfo = soap.dict_env.get("installinfo")

        if installinfo.get('version'):
            installinfo_ver = (installinfo.get('version')).split('-')
            dataservice.set_data("installinfo.version", installinfo_ver[0])
        else:
            LOG.info("Agent installation information does not exist.")

    def agent_update_for_action(self, action):
        node_ver = dataservice.get_data("node.version")

        if len(node_ver) == 0:
            return

        v = map(int, node_ver.split('.'))

        installinfo_ver = dataservice.get_data("installinfo.version")
        if installinfo_ver is None:
            return

        if len(installinfo_ver) == 0:
            return

        i = map(int, installinfo_ver.split('.'))

        if action:
            '''
            현재 버전(node_ver)이 서버에 있는 버전(installinfo_ver)보다 낮은 경우, 업그레이드 실시함.
            :return:
            '''
            LOG.info('CHECK FOR [UPGRADE].')
            if (v[0]) < (i[0]):
                self.nCompare = 1
                return

            if (v[1]) < (i[1]):
                self.nCompare = 1
                return

            if (v[2]) < (i[2]):
                self.nCompare = 1
                return

            if (v[3]) < (i[3]):
                self.nCompare = 1
                return

            LOG.info('Agent version is not Bigger.[INSTALL=%s < NODE=%s]' % (installinfo_ver, node_ver))
        else:
            '''
            버전이 다른 경우, 업데이트 실시함.
            :return:
            '''
            LOG.info('CHECK FOR [UPDATE].')
            if (v[0]) != (i[0]):
                self.nCompare = 1
                return

            if (v[1]) != (i[1]):
                self.nCompare = 1
                return

            if (v[2]) != (i[2]):
                self.nCompare = 1
                return

            if (v[3]) != (i[3]):
                self.nCompare = 1
                return

            LOG.info('Agent version is SAME.[INSTALL=%s , NODE=%s]' % (installinfo_ver, node_ver))

    def installing_updates(self):
        """
        기  능 : 에이전트 설치 또는 업데이트
        """

        dict_installinfo = soap.dict_env.get("installinfo")
        file_name_list = (dict_installinfo.get('downloadurl')).split("/")

        """
        rm -rf lnxagent.tar
        """
        command = ''.join(["rm -rf " + (file_name_list[5])])
        result = Shell.execute(command)

        """
        wget https://172.29.99.93:443/Agent/GnAgent/lnxagent.sh --no-check-certificate
        """
        command = ''.join(["wget " + (dict_installinfo.get('downloadurl')) + " --no-check-certificate"])
        result = Shell.execute(command)
        file_size = (dict_installinfo.get('downloadsize'))
        file_hash = ((dict_installinfo.get('checkpath')).split('|'))[1]

        command = ''.join(["sha256sum " + (file_name_list[5])])
        result = Shell.execute(command)
        downloaded_file_hash = result[1].split(" ")
        if downloaded_file_hash[0] != file_hash:
            LOG.info("do not match hash %s, %s" % (downloaded_file_hash, file_hash))
            return

        statinfo = os.stat(file_name_list[5])
        if file_size != str(statinfo.st_size):
            LOG.info("error size not match %s, %s" % (file_size, statinfo.st_size))
            return

        command = ''.join(['whoami'])
        result = Shell.execute(command)

        username = result[1].replace("\n", "", 1)
        command = ''.join(['chown ' + username + ':' + username + ' ' + file_name_list[5]])
        result = Shell.execute(command)

        command = ''.join(["chmod 755 " + file_name_list[5]])
        LOG.info("%s" % (command))
        result = Shell.execute(command)
        LOG.info("%s" % (result[1]))

        file_name = os.path.splitext(file_name_list[5])
        src_file = ''.join([self.path.get_agent_path() + '/' + file_name_list[5]])
        dst_file = ''.join(["/tmp" + '/' + file_name[0] + '_' + self.server_ip + file_name[1]])
        shutil.copy(src_file, dst_file)
        """
        Agent/lnxagent.sh 172.29.99.93 &
        Shell.execute(command) 대신 os.system(command) 명령으로 대체함.
        이 유: lnxagent.sh 172.29.99.93 & 명령어를 실행 할 때 알 수없는 오류발생으로 스크립트가 중단됨.
        """
        command = ''.join(["sudo " + dst_file + " &"])
        LOG.info("command= " + str(command))

        result = os.system(command)
        LOG.info("RESULT MSG= " + str(result[1]))

        sys.exit()


class GnUpdateKickingThread(GnThread):
    """
        Genian Agent의 자동 업그레이드 여부를 주기적으로 체크함.
    """
    def __init__(self, name, stop, interval):
        ''''''
        self.jobs = jobs_q
        self.results = results_q
        GnThread.__init__(self, name, stop, interval)

    def work(self):
        '''
        에이전트에서 주기적으로 업데이트 체크 함.
        주기 : 6시간.
        '''
        add_jobs(self.jobs, LOCAL_NOTIFYID_AGENTUPDATE)
        process(self.jobs, self.results)


class GnNotifyIFChangeThread(GnThread):
    ''''''

    def __init__(self, name, stop, interval, queue):
        ''''''
        self.jobs = queue
        GnThread.__init__(self, name, stop, interval)

    def work(self):
        """
        네트워크 인터페이스 상태가 변경되면 NetLink Event를 수신하는 쓰레드 함수.
        체크 대상 : 장비에 있는 모든 네트워크 인터페이스 (ethx, lo, wlanx, vlanx, etc)
        """
        net_if_change_check(self.jobs)


def add_jobs(jobs_q, notifyId):
    '''
    GnUpdate 쓰레드의 입력큐에 처리 할 태스크 추가.
    :param notifyId:
    :return:
    '''
    jobs_q.put(notifyId)
    return


def output(results_q):
    '''
    GnUpdate 쓰레드의 입력큐에 있는 태스크 처리 결과를 출력함.
    :param results:
    :return:
    '''
    done = 0
    while not results_q.empty():
        result = results_q.get_nowait()
        done += 1
    return done


def process(jobs_q, results_q):
    '''
    GnUpdate 쓰레드의 입력큐에 있는 태스크를 가져와서 처리함.
    :return:
    '''
    canceled = False
    try:
        jobs_q.join()
    except Exception as e:
        canceled = True
        LOG.error("Queue Exception. ERRMSG=%s" % (str(e)))
    if results_q is not None:
        if canceled:
            done = results_q.qsize()
        else:
            done = output(results_q)
    return


def process_noti(notify_q):
    '''
    GnNotify 쓰레드의 입력큐에 있는 태스크를 가져와서 처리함.
    :return:
    '''
    try:
        notify_q.join()
    except Exception as e:
        LOG.error("Queue Exception. ERRMSG=%s" % (str(e)))

    return


def verify_agent():
    """
    verify_agent
    agent 무결성 검증
    Verify.enc 파일의 작성된 파일목록의 hash값을 비교하여 변경여부를 확인한다.
    """
    p = AgentInstallPathInfo()

    try:
        file_infos = version.get_plugin_version_list_from_file()
        for el in file_infos:
            el.file_path = el.file_path.replace("%TARGET%/", p.get_agent_path() + "/")

            if "gnagent.py" in el.file_path:
                dataservice.set_data("node.version", el.file_version)

            try:
                if os.path.isfile(el.file_path):
                    cf = open(el.file_path, 'r')
                    cf.close()
                else:
                    continue
            except Exception as e:
                LOG.error("File not found. FILE=" + el.file_path + ", ERRMSG=" + str(e))
                agentexit()

            # file hash check
            command = ''.join(["openssl dgst -sha256 ", el.file_path, " | awk '{print $2}'"])
            shell_result = Shell.execute(command)
            hashvalue = shell_result[1].replace('\n', '').upper()
            filehash = el.file_hash.upper()

            if filehash != hashvalue:
                LOG.error("File hash mismatch. FILE=" + el.file_path + " ORGHASH=" + filehash + " DSTHASH=" + hashvalue)
                agentexit()

        LOG.info("Agent Verify Success.")
        return 1

    except Exception as e:
        LOG.error("Verify file not found. FILE=" + PATH_VERIFY + " " + str(e))
        return 0


def servercheck():
    '''
    정책서버 IP 확인
    '''
    try:
        path = AgentInstallPathInfo()
        fp = open(path.get_policy_server_address_file_path(), 'r')
        line = fp.readline()
        fp.close()

        global SERVER_IP, SERVER_ADDR
        SERVER_IP = line.replace("\n", "", 1)
        SERVER_ADDR = struct.unpack('>L', socket.inet_aton(SERVER_IP))[0]
        return 1
    except Exception as e:
        LOG.error("soapserver isn't found. " + str(e))
        return 0


def signalhandler(signum, f):
    ''''''
    LOG.fatal('signal received. SIG=' + str(signum))
    stop_event.set()


def agentlogon():
    LOG.info("agentlogon CURRENT DIR= " + str(os.getcwd()))
    LOG.info("agentlogon CURRENT USER= " + str(getpass.getuser()))
    params = [
        "",
        node.mac_addr,
        "",
        "",
        node.hostname,
        node.platform,
        node.osid,
        node.lang,
        node.ip_addr
    ]

    try:
        soap.call(ID_SOAP_LOGON, params)
        node.nodeid = soap.dict_env.get("nodeid")

        # db store nodeid
        dataservice.set_data(KEYSTR_NODEID, node.nodeid)

        LOG.info('agent logon success. node.nodeid=' + str(node.nodeid))
    except Exception as msg:
        LOG.info('agent logon failed. ERRMSG=' + str(msg))
        raise


def agentprofile():
    ''''''
    params = [
        node.nodeid,
        "20",
        ""
    ]

    try:
        soap.call(ID_SOAP_GETMYPROFILE, params)
        LOG.info('agent profile received success.')
        """
        정책 서버에 접속한 결과를 저장하는 클래스 필요.
        bool CGnJoin::CenterJoin(bool bRejoin) - Agent/GnAgent/GnJoin.cpp 함수 참조.
        """
    except Exception as msg:
        LOG.info('agent profile received failed.' + str(msg))
        raise


def agentexit():
    '''
    agentexit
    '''
    LOG.info("Agent Stop.")
    sys.exit()


def main(args):
    '''
    main
    '''
    LOG.info("Agent Start.")

    # signal
    signal.signal(signal.SIGINT, signalhandler)

    global dataservice
    dataservice = Dataservice()

    global threadDelegate
    threadDelegate = GnThreadDelegate()

    global stop_event
    stop_event = threading.Event()

    if verify_agent() == 0:
        agentexit()

    ret = gntools.get_soap_server_ip_port()
    if len(ret) is None or ret is None:
        LOG.info('Serverip not found. GnStart exit')
        sys.exit()

    global node
    node = Node()
    node.node_info_setup()
    node.create_node_info()

    global tool
    tool = Tool()
    if tool.create_tool() == False:
        agentexit()

    global soap
    soap = Soap(tool)

    agentlogon()

    while node.nodeid is None:
        node.node_info_setup()
        time.sleep(5)

    agentprofile()

    global jobs_q
    jobs_q = Queue.Queue()

    global results_q
    results_q = Queue.Queue()

    global schedule_q
    schedule_q = Queue.Queue()

    # thread start

    thnamepipe = Namedpipe(NAMEDPIPE_AGENT, stop_event, 0, notify_q)
    thnamepipe.start()

    thsender = PluginResultsendThread(stop_event, 10)
    thsender.start()

    thplugin = PluginManager(stop_event, 600)

    thplugin.add_plugin(7001, "osinfo")
    thplugin.add_plugin(7002, "hwinfo")
    thplugin.add_plugin(7003, "swinfo")
    thplugin.add_plugin(7004, "netinfo")
    thplugin.add_plugin(7005, "scriptctrl")

    thplugin.set_agentaction(soap.dict_env['aalist'], soap.dict_env['tolist'])
    thplugin.start()

    notifymgr = GnNotify("GnNotify Mgr", stop_event, 1, schedule_q)
    notifymgr.setup_network_info()
    notifymgr.start()

    versionmgr = GnUpdateRunner("gnupdate runnger", stop_event, 10)
    versionmgr.start()

    thgnupdatekicking = GnUpdateKickingThread("gnupdate-kicking", stop_event, 21600)
    thgnupdatekicking.start()

    thschedule = GnScheduleThread("gnSchedule", schedule_q, notify_q)
    thschedule.setDaemon(True)
    thschedule.start()

    """
    윈도우 에이전트의 NetIfChange() 쓰레드의 실행주기에 따라서 기존 1초 -> 5초로 변경함.
    """
    thnotifyifchange = GnNotifyIFChangeThread("threadNetIf", stop_event, 5, notify_q)
    thnotifyifchange.start()

    threads['namedpipe'] = thnamepipe
    threads['resultsender'] = thsender
    threads['pluginmgr'] = thplugin
    threads['versionmgr'] = versionmgr
    threads['updatekicking'] = thgnupdatekicking
    threads['gnNotifyMgr'] = notifymgr
    threads['gnSchedule'] = thschedule
    threads['gnnotifyifchange'] = thnotifyifchange


    """
    사용자 인증 모듈을 사용하지 않음.(GUI, Console를 통한 인증 기능 구현 필요)
    현재 구현 상태는 Web browser로만 가능함.
    """
    authstatus = "0"

    """
    authstatus == "1" : 사용자 인증이 되지 않은 상태.
    authstatus == "0" : 사용자 인증이 완료된 상태.
    """
    if authstatus != "0" or len(authstatus) == 0:
        send_notify = (44, '0x400')
        add_jobs(notify_q, send_notify)

    signal.pause()

    # 모든 thread 가 종료될때까지 대기
    for key in threads.keys():
        t = threads[key]
        t.stopwait()
        t.join()

    dataservice.close()
    logging.shutdown()
    agentexit()


if __name__ == '__main__':

    LOG = set_logging_parameter(sys.argv)
    if '-d' in sys.argv:
        path = AgentInstallPathInfo()
        path.set_devopt("unittest")
    main(sys.argv)

