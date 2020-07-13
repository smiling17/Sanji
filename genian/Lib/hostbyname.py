# -*- coding: utf-8 -*-

import socket
import fcntl
import struct
import array

import os
import sys
from errno import *


def network_host_by_name():
    """
    도메인 이름 -> IP주소 변경.
    :return:
    """
    pass


def network_host_by_addr():
    """
    IP주소 ->  도메인 이름으로 변경.
    :return:
    """
    pass


class HostByName(object):
    pass

    def __init__(self):
        self.strIP = None
        self.ip_by_host = None
        self.ip_by_network = None

    def network_host_by_name(self):
        pass

