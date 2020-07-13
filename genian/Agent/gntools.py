# -*- coding: utf-8 -*-
# 한글 주석 사용 가능하도록 encoding 추가

import re
import socket
import fcntl
import struct
import array
import random

import os
import string


from include.common import (AgentInstallPathInfo)
import Lib.crypto as crypto

netinfo_keys = ["iface", "ip", "mac", "gateway", "netmask"]


def get_soap_server_ip_port():
    """
    정책 서버의 주소를 설치 스크립트에서 추출한다.

    IP 주소에 대해서 유효성 검사까지 수행한다.
        유효성 검사시에 제외되는 경우
            -1.2.3.4
            IPv6

    :return: string
     "172.29.99.93"
    """
    path = AgentInstallPathInfo()
    try:
        fp = open(path.get_policy_server_address_file_path(), 'r')
        line = fp.readline()
        fp.close()
        server_ip = line.replace("\n", "", 1)
        # soap_port = 80
    
        cnt = line.count('.')
        if cnt != 3:
            return "None"
    
        # IP 유효성 검사
        ipv4_re = re.compile(
            '\\b(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\\.(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]|[0-9])\\b')
    
        match = ipv4_re.match(server_ip)
    
        # Domain Name 검사
    
        return match.group()
    
    except Exception as e:
        return "None"


def all_interfaces():
    """
    현재 장비의 모든 네트워크 인터페이스 명을 리스트로 반환한다.

    :return: list
        Ubuntu 16.04 의 경우,
        ['enp3s0','docker0','lo']
    """
    max_possible = 128
    bytes = max_possible * 32
    
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    names = array.array('B', '\0' * bytes)
    outbytes = struct.unpack('iL', fcntl.ioctl(
        s.fileno(),
        0x8912,  # SIOCGIFCONF
        struct.pack('iL', bytes, names.buffer_info()[0])))[0]
    
    namestr = names.tostring()
    iface_list = []
    
    for i in range(0, outbytes, 40):
        name = namestr[i:i + 16].split('\0', 1)[0]
        # ip = namestr[i + 20:i + 24]
        # lst.append((name, ip))
        iface_list.append(name)
    return iface_list


def format_ip(addr):

    return str(ord(addr[0])) + '.' + \
           str(ord(addr[1])) + '.' + \
           str(ord(addr[2])) + '.' + \
           str(ord(addr[3]))


def all_interfaces_info():
    max_possible = 128
    bytes = max_possible * 32

    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    names = array.array('B', '\0' * bytes)
    outbytes = struct.unpack('iL', fcntl.ioctl(s.fileno(), 0x8912,  # SIOCGIFCONF
                             struct.pack('iL', bytes, names.buffer_info()[0])))[0]
    namestr = names.tostring()
    iface_list = []
    for i in range(0, outbytes, 40):
        name = namestr[i:i + 16].split('\0', 1)[0]
        iface_list.append(name)

    return iface_list


def netinfo():
    """
    장비의 네트워크 정보를 아래와 같이 리턴한다.

    :return: list
        Ubuntu 16.04 인경우,
            [
                {
                    'iface': 'docker0',
                    'mac': '02:42:b0:55:ab:6e',
                    'ip': '172.17.0.1',
                    'gateway': ''
                },
                {
                    'iface': 'enp3s0',
                    'mac': '02:42:b0:55:ab:6e',
                    'ip': '172.17.0.1',
                    'gateway': '172.29.99.254'
                },
                {
                    'iface': 'lo',
                    'mac': '',
                    'ip': '127.0.0.1',
                    'gateway': ''
                }
            ]
    """
    list_netinfo = []

    list_ifname = all_interfaces_info()
    
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    
    for ifname in list_ifname:
        result1 = {}
        
        ipinfo = fcntl.ioctl(s.fileno(), 0x8915, struct.pack('256s', ifname))[20:24]
        ipaddr = struct.unpack("<L", ipinfo)[0]
        ipaddr = socket.htonl(ipaddr)
        ipaddr = socket.inet_ntoa(ipinfo)
        
        macinfo = fcntl.ioctl(
            s.fileno(),
            0x8927,
            struct.pack('256s', ifname[:15]))
        macaddr = ''.join(['%02x:' % ord(char) for char in macinfo[18:24]])[:-1]
        macaddr = macaddr.upper()
        
        netmask = socket.inet_ntoa(fcntl.ioctl(
            s.fileno(),
            0x891b,  # SIOCGIFADDR
            struct.pack('256s', ifname[:15]))[20:24])

        result1[netinfo_keys[0]] = ifname
        result1[netinfo_keys[1]] = ipaddr
        result1[netinfo_keys[2]] = macaddr
        result1[netinfo_keys[4]] = netmask
        list_netinfo.append(result1)
    
    with open("/proc/net/route") as fh:
        ''''''
        for line in fh:
            ''''''
            fields = line.strip().split()
            if fields[1] != '00000000' or not int(fields[3], 16) & 2:
                continue
            for el in list_netinfo:
                if el['iface'] == fields[0]:
                    el['gateway'] = socket.inet_ntoa(struct.pack("<L", int(fields[2], 16)))
                else:
                    el['gateway'] = ""

    return list_netinfo


def get_account_info():
    """
    user.enc 파일에서 계정 정보를 추출하는 함.
    :return: user.enc 파일이 있는 경우 : string 객체 [계정 이름:비밀번호:그룹명]
              user.enc 파일이 없는 경우 : None 객체
    """
    path = AgentInstallPathInfo()

    """ /usr/local/Geni/Genian/user.enc """
    enc_file = path.get_user_enc_file_path()

    if os.path.isfile(enc_file):
        result = crypto.decryption(enc_file)
        return result[0]


def rand_str(len, characters):
    return "".join([random.choice(characters) for _ in range(len)])


def installed_agent_path(dir_name, name):
    """
    GnStart의 설치 경로를 리턴.
    :param dir_name: 검색할 폴더 명.
            name : 찾을 폴더 명.
    :return:
            전체 경로(우분투) : /usr/local/Geni/Genian/
    """
    path = search(dir_name, name)
    return path


def search(dir_name, name):
    """
    특정 디렉토리를 검색하여 name 폴더를 찾는다.
    하위 폴더 검색은 하지 않음
    :param dir_name: 검색할 폴더 명.
            name : 찾을 폴더 명.
    :return: 검색된 폴더의 전체 경로
    """
    try:
        filenames = os.listdir(dir_name)
        if name in filenames:
            return os.path.join(dir_name, name)
    except Exception as e:
        """
        PermissionError
        """
        print "The folder was not found. ERRMSG=", str(e)




def get_local_ip_connected(ip, port):
    """
    deprecated api
    :param ip: 정책 서버 주소(string)
    :param port: 정책 서버 포트 (int)
    :return: list
         [ip_ntoa, ip_aton]
    """
    ip_info = []

    if is_ipv4(ip) is False:
        return None

    try:
        ss = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    except AttributeError as e:
        print 'socket.socket() exception - AttributeError ERRMSG=' + str(e)
    except socket.error as err:
        print 'socket.socket() exception - No socket information available. ERRMSG=' + str(err)
        return None

    try:
        ss.connect((ip, port))
    except AttributeError as e:
        print 'ss.connect() exception AttributeError ERRMSG=' + str(e)
    except socket.error as err:
        print 'ss.connect() exception - Failed to get socket information ERRMSG=' + str(err)
        return None

    ip_ntoa = ss.getsockname()[0]
    ip_info.append(ip_ntoa)
    ip_info.append(struct.unpack('<L', socket.inet_aton(ip_ntoa))[0])

    ss.close()
    return ip_info


def is_ipv4(value):
    """
    IPv4 에 대한 유효한 주소인지 검사한다.
    """
    try:
        socket.inet_pton(socket.AF_INET, value)
    except AttributeError:
        try:
            socket.inet_aton(value)
        except socket.error:
            return False
        return value.count('.') == 3
    except socket.error:
        return False
    return True


def iface_status(name):
    """
    인터페이스 의 UP/DOWN 상태를 체크한다.
    """
    # Set some symbolic constants
    SIOCGIFFLAGS = 0x8913
    null256 = '\0'*256

    # Get the interface name from the command line
    # ifname = sys.argv[1]
    ifname = name

    # Create a socket so we have a handle to query
    s = socket(AF_INET, SOCK_DGRAM)

    # Call ioctl(  ) to get the flags for the given interface
    result = fcntl.ioctl(s.fileno(), SIOCGIFFLAGS, ifname + null256)

    # Extract the interface's flags from the return value
    flags, = struct.unpack('H', result[16:18])

    # Check "UP" bit and print a message
    up = flags & 1
    print ('DOWN', 'UP')[up]


def get_policy_server_addr_from_filename(path):
    """
    특수문자와 아스키 문자, 줄바꿈 문자
    >>> string.punctuation
    '!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~'

    >>> string.ascii_letters
    'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'

    >>> string.whitespace
    '\t\n\x0b\x0c\r '
    """
    DELMINATOR_UNDER_BAR = "_"
    DELMINATOR_SPACE = " "
    DELMINATOR_NUMBER_DOT = "0123456789."

    str_rev = path
    splited_list_under_bar = str_rev.split(DELMINATOR_UNDER_BAR)
    splited_list_space = splited_list_under_bar[1].split(DELMINATOR_SPACE)
    for c in splited_list_space[0]:
        if not c in DELMINATOR_NUMBER_DOT:
            print("False")
            return

    return splited_list_space[0]




