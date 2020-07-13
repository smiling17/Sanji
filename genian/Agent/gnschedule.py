# -*- coding: utf-8 -*-

import threading

from gnconfigure import (
    agent_do_now_flag_dict
)

from Lib.debug import get_log

class GnScheduleThread(threading.Thread):
    def __init__(self, name, sch_queue, notify_queue):
        threading.Thread.__init__(self, name=name)
        self.sch_q = sch_queue
        self.notify_q = notify_queue
        self.case_name = ""
        self.case = None
        self.log = get_log()

    def switch(self, arg):
        """
        :param arg: NotifyAgentDoNowFlag ID 값.
        :return: NotifyAgentDoNowFlag ID 값에 맵핑되는 함수의 주소를 리턴함.
        """
        self.case_name = agent_do_now_flag_dict.get(arg)
        self.case = getattr(self, self.case_name, self.case_default)
        return self.case()

    def pms_now(self):
        self.log.info("pms_now")
        pass

    def action_now(self):
        self.log.info("action_now")
        pass

    def sms_now(self):
        self.log.info("sms_now")
        pass

    def sys_const_now(self):
        self.log.info("sys_const_now")
        pass

    def license(self):
        self.log.info("license")
        pass

    def auth_now(self):
        self.log.info("auth_now")
        send_notify = (44, '0x400')
        self.put(self.notify_q, send_notify)

    def profile(self):
        self.log.info("profile")

    def notice(self):
        self.log.info("notice")

    def default(self):
        self.log.info("default")

    def run(self):
        agent_do_now_flag_list = []
        for el in agent_do_now_flag_dict:
            agent_do_now_flag_list.append(el)

        while True:
            job = self.sch_q.get()
            self.sch_q.task_done()
            if job in agent_do_now_flag_list:
                self.switch(job)

    def put(self, queue, event_id):
        queue.put(event_id)
