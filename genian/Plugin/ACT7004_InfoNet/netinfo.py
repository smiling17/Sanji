# -*- coding: utf-8 -*-
# 한글 주석 사용 가능하도록 encoding 추가

import sys
import os

sys.path.append(os.path.dirname(os.path.dirname(os.getcwd())))
sys.path.append(''.join([os.path.dirname(os.path.dirname(os.getcwd())) + os.sep + "Lib"]))
sys.path.append(''.join([os.path.dirname(os.path.dirname(os.getcwd())) + os.sep + "Agent"]))
sys.path.append(''.join([os.path.dirname(os.path.dirname(os.getcwd())) + os.sep + "include"]))

import socket
import fcntl
import struct
import array
import base64
import threading

from Agent.gnagent import pluginbase
from Lib.debug import set_logging_parameter

from include.common import (
    AgentInstallPathInfo,
)

import include.constants as plugin_const


# Application string
IDS_NET_LS = "ls"

ID_NI_DEVICE = 0
ID_NI_NAME = 1
ID_NI_NETMASK = 2
ID_NI_MAC = 3
ID_NI_GATEWAY = 4
ID_NI_ADDR = 5
ID_NI_TYPE = 6
ID_NI_MEDIATYPE = 7
ID_NI_SSID = 8
ID_NI_BSSID = 9


class netinfo(pluginbase):
    '''
    netinfo
    '''

    def create_dicts(self):
        '''
        create_dicts
        '''
        pluginbase.create_dicts(self)

        self.dict_app = {
            IDS_NET_LS: "ls /sys/class/net/"
        }

        self.list_column = [
            "NI_DEVICE",
            "NI_NAME",
            "NI_NETMASK",
            "NI_MAC",
            "NI_GATEWAY",
            "NI_ADDR",
            "NI_TYPE",
            "NI_MEDIATYPE",
            "NI_SSID",
            "NI_BSSID"
        ]

        self.list_data = [
        ]

    def check_complete(self):
        '''
        check_complete
        '''
        return (len(self.list_data))

    def run(self):
        '''
        run
        '''
        try:
            for cmdid, command in self.dict_app.items():
                ''''''
                if (self.check_tool(cmdid) == False):
                    continue

                list_result = self.execute(command)
                if (list_result[plugin_const.ID_RESULT] == plugin_const.ID_RESULT_FAIL):
                    continue

                if (self.check_complete() == True):
                    break

                list_netinfo = []
                if (cmdid == IDS_NET_LS):
                    ''''''
                    gateways = {}
                    with open("/proc/net/route") as fh:
                        ''''''
                        for line in fh:
                            ''''''
                            fields = line.strip().split()
                            if fields[1] != '00000000' or not int(fields[3], 16) & 2:
                                continue
                            gateways[fields[0]] = socket.inet_ntoa(
                                struct.pack("<L", int(fields[2], 16)))

                    list_ifname = self.all_interfaces()

                    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

                    for ifname in list_ifname:
                        ''''''
                        list_net = [
                            "",  # NI_DEVICE
                            "",  # NI_NAME
                            "",  # NI_NETMASK
                            "",  # NI_MAC
                            "",  # NI_GATEWAY
                            "",  # NI_ADDR
                            "",  # NI_TYPE
                            "",  # NI_MEDIATYPE
                            "",  # NI_SSID
                            "",  # NI_BSSID
                        ]

                        ipinfo = fcntl.ioctl(s.fileno(),
                                             0x8915,  # SIOCGIFADDR
                                             struct.pack('256s', ifname[:15])
                                             )[20:24]

                        ipaddr = struct.unpack("<L", ipinfo)[0]
                        ipaddr = socket.htonl(ipaddr)
                        ipaddr = socket.inet_ntoa(ipinfo)
                        macinfo = fcntl.ioctl(
                            s.fileno(),
                            0x8927,
                            struct.pack('256s', ifname[:15]))
                        macaddr = ''.join(['%02x:' % ord(char)
                                           for char in macinfo[18:24]])[:-1]
                        macaddr = macaddr.upper()

                        list_net[ID_NI_NAME] = ifname
                        list_net[ID_NI_ADDR] = ipaddr
                        list_net[ID_NI_MAC] = macaddr

                        if gateways.has_key(ifname):
                            list_net[ID_NI_GATEWAY] = gateways[ifname]

                        netmask = socket.inet_ntoa(fcntl.ioctl(
                            s.fileno(),
                            0x891b,  # SIOCGIFADDR
                            struct.pack('256s', ifname[:15])
                        )[20:24])

                        list_net[ID_NI_NETMASK] = netmask

                        list_netinfo.append(list_net)

                self.list_data = [
                    [
                        "NETIF",
                        self.list_column,
                        list_netinfo
                    ]
                ]

                self.set_profileresult(plugin_const.GN_PROFILE_WORKTYPE_ACTION, "1", "")
        except Exception as e:
            self.set_profileresult(plugin_const.GN_PROFILE_WORKTYPE_ACTION, "0", str(e))

    def all_interfaces(self):
        max_possible = 128  # arbitrary. raise if needed.
        bytes = max_possible * 32
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        names = array.array('B', '\0' * bytes)
        outbytes = struct.unpack('iL', fcntl.ioctl(
            s.fileno(),
            0x8912,  # SIOCGIFCONF
            struct.pack('iL', bytes, names.buffer_info()[0])))[0]
        namestr = names.tostring()
        lst = []
        for i in range(0, outbytes, 40):
            name = namestr[i:i + 16].split('\0', 1)[0]
            ip = namestr[i + 20:i + 24]
            # lst.append((name, ip))
            lst.append(name)
        return lst

    def format_ip(addr):
        return str(ord(addr[0])) + '.' + \
            str(ord(addr[1])) + '.' + \
            str(ord(addr[2])) + '.' + \
            str(ord(addr[3]))

    def __init__(self):
        '''
        __init__
        '''
        self.create_dicts()
