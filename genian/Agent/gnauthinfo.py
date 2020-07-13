# -*- coding: utf-8 -*-
# 한글 주석 사용 가능하도록 encoding 추가

import webbrowser


class AuthInfo(object):

    def __init__(self, url):
        self.url = url

    def read_profile(self):
        raise NotImplementedError('subclasses of AuthInfo must provide a read_profile() method')

    def write_profile(self, bprofile):
        raise NotImplementedError('subclasses of AuthInfo must provide a write_profile() method')

    def profile_update(self, profile):
        raise NotImplementedError('subclasses of AuthInfo must provide a profile_update() method')

    def work_receive_message(self, n_job_id, str_job):
        raise NotImplementedError('subclasses of AuthInfo must provide an work_receive_message() method')

    def get_shape_ie_string(self, ntype):
        raise NotImplementedError('subclasses of AuthInfo must provide an get_shape_ie_string() method')

    def get_size_ie_string(self, ntype):
        raise NotImplementedError('subclasses of AuthInfo must provide an get_size_ie_string() method')

    def get_popup_id_string(self, str_popup_id, nid):
        raise NotImplementedError('subclasses of AuthInfo must provide an get_popup_id_string() method')

    def update_info_ex(self, job):
        raise NotImplementedError('subclasses of AuthInfo must provide an update_info_ex() method')

    def get_center_url(self, str_url):
        raise NotImplementedError('subclasses of AuthInfo must provide an get_center_url() method')

    def show_web_browser_for_ie(self):
        webbrowser.open_new(self.url)
