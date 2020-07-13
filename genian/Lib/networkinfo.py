# -*- coding: utf-8 -*-

import re
import socket
import fcntl
import struct
import array
import random

import os
import sys
import site
from errno import *

from debug import set_logging, get_log
import ioctls as ioctl
from hostbyname import HostByName


class NetworkInfo():
    def __init__(self):
        self.netinfo_keys = ["iface", "ip", "mac", "gateway", "netmask"]
        self.str_ip = None
        self.ip_by_host = None
        self.ip_by_network = None
        self.obj_host_name = None
        self.setup_network_info()
        self.log = get_log()

    def setup_network_info(self):
        self.obj_host_name = HostByName()

    def net_if_ip_addr_init(self):
        pass

    def update_ip_address(self):
        pass

    def update_ip_address_by_network(self):
        pass

    def update_ip_address_by_host(self):
        pass

    def get_network_info(self, nType, strConnIP, nPort):
        pass

    def get_local_ip_from_socket(self, str_ip, n_port):
        """
        로컬 네트워크가 구성되었을 경우, 정책 서버와 연결이 된 네트워크 interface정보를 리스트로 리턴 한다.
        :param ip:  정책서버의 주소(string)
        :param port: 정책서버의 포트(int)
        :return: list
            [ip_ntoa, ip_aton, iface name, mac]
        """
        local_ip_info = []

        is_64bits = sys.maxsize > 2 ** 32
        struct_size = 40 if is_64bits else 32
        max_possible = 8

        if self.valid_ip_address_checking(str_ip) is False:
            return None

        """
        소켓 생성.(UDP)
        """
        try:
            ss = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        except AttributeError as e:
            self.log.info('socket.socket() exception AttributeError ERRMSG=' + str(e))
        except socket.error as err:
            self.log.info('socket.socket() exception - No socket information available. ERRMSG=' + str(err))
            return None

        """
        정책 서버로 접속 시도함.
        """
        try:
            ss.connect((str_ip, n_port))
        except AttributeError as e:
            self.log.info('ss.connect() exception AttributeError ERRMSG=' + str(e))
        except socket.error as err:
            self.log.info('ss.connect() exception Failed to get socket information. ERRMSG=' + str(err))
            return None

        """
        로컬 소켓 정보 얻기.
        """
        local_ip = ss.getsockname()[0]
        local_addr = struct.unpack('>L', socket.inet_aton(local_ip))[0]
        local_ip_info.append(local_ip)
        local_ip_info.append(local_addr)

        while True:
            ''''''
            datasize = max_possible * struct_size
            names = array.array('B', '\0' * datasize)

            outbytes = struct.unpack('iL', fcntl.ioctl(
                ss.fileno(),
                ioctl.SIOCGIFCONF,
                struct.pack('iL', datasize, names.buffer_info()[0])))[0]
            if outbytes == datasize:
                max_possible *= 2
            else:
                break

        namestr = names.tostring()

        ipstr = [(namestr[i:i + 16].split('\0', 1)[0], socket.inet_ntoa(namestr[i + 20:i + 24]))
                 for i in range(0, outbytes, struct_size)]

        for ip in ipstr:
            if ip[1] == local_ip:
                local_ip_info.append(ip[0])

                macinfo = fcntl.ioctl(ss.fileno(), ioctl.SIOCGIFHWADDR, struct.pack('256s', ip[0][:15]))
                local_ip_info.append(':'.join(['%02x' % ord(char) for char in macinfo[18:24]]))
                break

        ss.close()
        return local_ip_info

    def valid_ip_address_checking(self, value):
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

    def get_local_ip_from_route(self):
        """
        네트워크 정보를 라우팅 테이블을 참조하여 생성한다.
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

        list_ifname = self._all_interfaces_info()

        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        for ifname in list_ifname:
            result1 = {}

            ipinfo = fcntl.ioctl(s.fileno(), ioctl.SIOCGIFADDR, struct.pack('256s', ifname))[20:24]
            ipaddr = struct.unpack("<L", ipinfo)[0]
            ipaddr = socket.htonl(ipaddr)
            ipaddr = socket.inet_ntoa(ipinfo)

            macinfo = fcntl.ioctl(
                s.fileno(),
                ioctl.SIOCGIFHWADDR,
                struct.pack('256s', ifname[:15]))
            macaddr = ''.join(['%02x:' % ord(char) for char in macinfo[18:24]])[:-1]
            macaddr = macaddr.upper()

            netmask = socket.inet_ntoa(fcntl.ioctl(
                s.fileno(),
                ioctl.SIOCGIFNETMASK,
                struct.pack('256s', ifname[:15]))[20:24])

            result1[self.netinfo_keys[0]] = ifname
            result1[self.netinfo_keys[1]] = ipaddr
            result1[self.netinfo_keys[2]] = macaddr
            result1[self.netinfo_keys[4]] = netmask
            list_netinfo.append(result1)

        with open("/proc/net/route") as fh:
            for line in fh:
                fields = line.strip().split()
                if fields[1] != '00000000' or not int(fields[3], 16) & 2:
                    continue
                for el in list_netinfo:
                    if el['iface'] == fields[0]:
                        el['gateway'] = socket.inet_ntoa(struct.pack("<L", int(fields[2], 16)))
                    else:
                        el['gateway'] = ""

        return list_netinfo

    def read_ip_addr_table(self):
        pass

    def search_local_ip_by_network(self):
        pass

    def search_local_ip_by_host(self):
        pass

    def search_local_ip(self):
        pass

    def network_host_by_name(self):
        self.obj_host_name.network_host_by_name()
        pass

    def _all_interfaces_info(self):
        max_possible = 128
        bytes = max_possible * 32

        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        names = array.array('B', '\0' * bytes)
        outbytes = struct.unpack('iL', fcntl.ioctl(s.fileno(),
                                ioctl.SIOCGIFCONF,
                                 struct.pack('iL', bytes, names.buffer_info()[0])))[0]
        namestr = names.tostring()
        iface_list = []
        for i in range(0, outbytes, 40):
            name = namestr[i:i + 16].split('\0', 1)[0]
            iface_list.append(name)

        return iface_list

    def get_gateway_ip_from_route(self):
        with open("/proc/net/route") as fh:
            for line in fh:
                fields = line.strip().split()
                if fields[1] != '00000000' or not int(fields[3], 16) & 2:
                    continue
                return socket.inet_ntoa(struct.pack("<L", int(fields[2], 16)))
        return ""


