# -*- coding: utf-8 -*-

import os
import socket
import struct

import ioctls as ioctl
from debug import get_log
import Agent.gnconfigure as gconf

# These constants map to constants in the Linux kernel. This is a crappy
# way to get at them, but it'll do for now.
RTMGRP_LINK = 1

NLMSG_NOOP = 1
NLMSG_ERROR = 2

RTM_NEWLINK = 16
RTM_DELLINK = 17

IFLA_IFNAME = 3


def net_if_change_check(ret_q):
    """
    flags IFF_RUNNING 를 체크하여 인터페이스 다운과 링크 다운를 함께 체크 할 수 있음.
    인터페이스의 상태 변화가 없으면 s.recv()함수에서 blocking 된다.

    감지되는 상태 변화 :
        1. 인터페이스의 UP/DOWN
        2. UTP 케이블의 연결 상태.

    감지되지 않는 경우 :
        1. iface up/down 없이 수행되는 경우는 감지 않됨.
            ex > ifconfig eth0 182.29.99.1
    파라메터 :
        ret_q : 쓰레드용 Queue 객체를 사용함.
                GnNotify 객체가 사용하는 Queue 객체.
    Return 값:
    0 : down
    not 0(양수) : up and running

    참조 코드 URL : https://gist.github.com/Lukasa/6209575d588f1584c374
    """
    LOG = get_log()
    NOTIFYID=gconf.NotifyId()
    LOG.info("Start monitoring network interface status.")
    s = socket.socket(socket.AF_NETLINK, socket.SOCK_RAW, socket.NETLINK_ROUTE)
    s.bind((os.getpid(), RTMGRP_LINK))

    while True:
        data = s.recv(65535)
        msg_len, msg_type, flags, seq, pid = struct.unpack("=LHHLL", data[:16])

        if msg_type == NLMSG_NOOP:
            continue

        elif msg_type == NLMSG_ERROR:
            break

        if msg_type != RTM_NEWLINK:
            continue

        data = data[16:]

        family, _, if_type, index, flags, change = struct.unpack("=BBHiII", data[:16])

        remaining = msg_len - 32
        data = data[16:]

        while remaining:
            rta_len, rta_type = struct.unpack("=HH", data[:4])

            if rta_len < 4:
                break

            rta_data = data[4:rta_len]

            increment = (rta_len + 4 - 1) & ~(4 - 1)
            data = data[increment:]
            remaining -= increment

            if rta_type == IFLA_IFNAME:
                if flags & ioctl.IFF_RUNNING:
                    status = 'Running.'
                else:
                    status = 'Not Running.'

                LOG.info("The status of the network interface has changed. (IF_NAME = %s, IF_RUNNING = %s)" % (rta_data, status))
                send_notify = (NOTIFYID.NOTIFYID_REGISTER_REQ, '0x100')
                try:
                    ret_q.put(send_notify)
                    LOG.info('Successfully sent center reconnection request message.')
                except Exception as e:
                    LOG.info('Failed to send center reconnection request message. (Error = %s)' % (str(e)))
