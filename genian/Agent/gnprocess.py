# -*- coding: utf-8 -*-
from subprocess import Popen, PIPE
from re import split
from sys import stdout

class Proc(object):
    """
    :return: ps(process status)명령어 대한 Data Structure.
    """
    def __init__(self, proc_info):
        self.user = proc_info[0]
        self.pid = proc_info[1]
        self.cpu = proc_info[2]
        self.mem = proc_info[3]
        self.vsz = proc_info[4]
        self.rss = proc_info[5]
        self.tty = proc_info[6]
        self.stat = proc_info[7]
        self.start = proc_info[8]
        self.time = proc_info[9]
        self.cmd = proc_info[10:]

    def to_str(self):
        """
        info about the process : user, pid, and coimmand
        """
        return '%s %s %s' % (self.user, self.pid, self.cmd)


def get_proc_list():
    """
    :return: process list를 리턴함.
    """
    proc_list = []
    sub_proc = Popen(['ps', 'aux'], shell=False, stdout=PIPE)
    for line in sub_proc.stdout:
        proc_info = split(" *", line)
        proc_list.append(Proc(proc_info))
    return proc_list

