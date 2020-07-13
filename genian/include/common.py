#!/usr/bin/python
# -*- coding: utf-8 -*-


import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from Lib.singleton import Singleton

"""
    에이전트 설치 경로를 최대한 토큰 단위로 나누어서 설정함.
"""
agent_install_path_list = {
    "AGENT_VENDER_NAME": "Geni",

    "AGENT_NAME": "Genian",

    "AGENT_DIR_NAME": "Agent",

    "AGENT_INSTALL_PREFIX": "/usr/local",

    "AGENT_LOG_DIR": "log",

    "AGENT_LOG_FILE_NAME": "agent.log",

    "AGENT_VERIFY_NAME": "Verify.gnc",

    "AGENT_VERIFY_TRN_NAME": "Verify.trn",

    "AGENT_UNITTEST_NAME": "UnitTest",

    "AGENT_GNSTART_DAEMON_NAME": "gnstart.py",

    "AGENT_GNAGENT_DAEMON_NAME": "gnagent.py",

    "AGENT_TEMP_DIR": "tmp",

    "AGENT_DATABASE_NAME": "service.dat",

    "AGENT_PLUGIN_DIR": "Plugin",

    "AGENT_POLICY_SERVER_FILE_NAME": "soapserver",

    "AGENT_VERSION_ENC_NAME": "version.enc",

    "AGENT_VERSION_TRN_NAME": "version.trn",

    "AGENT_USER_ENC_NAME": "user.enc",
}


path = agent_install_path_list

""" /usr/local/Geni/Genian/Verify.gnc """
PATH_VERIFY = ''.join([path['AGENT_INSTALL_PREFIX'], '/',
                        path['AGENT_VENDER_NAME'], '/',
                        path['AGENT_NAME'], '/',
                        path['AGENT_VERIFY_NAME']])

""" /usr/local/Geni/Genian/Verify.trn """
PATH_VERIFY_TRUNC = ''.join([path['AGENT_INSTALL_PREFIX'], '/',
                                path['AGENT_VENDER_NAME'], '/',
                                path['AGENT_NAME'], '/',
                                path['AGENT_VERIFY_TRN_NAME']])

plugin_name = [
    ("7001", "InfoOS"),
    ("7002", "InfoHW"),
    ("7003", "InfoSW"),
    ("7004", "InfoNet"),
    ("7005", "ScriptCtrl"),
]

class AgentInstallPathInfo:
    __metaclass__ = Singleton

    def __init__(self):
        self.p = agent_install_path_list
        self.devopt = None

    def set_devopt(self, param):
        """
        에이전트의 개발/상용 모드 구분을 위해서 설정한다.
        개발 모드 : unittest
        상용 모드 : None
        """
        self.devopt = str(param)
        if "unittest" in param:
            self.p["AGENT_INSTALL_PREFIX"] = ''.join([os.getcwd(), "/usr/local"])
        else:
            self.p["AGENT_INSTALL_PREFIX"] = "/usr/local"

    def get_database_path(self):
        """ /usr/local/Geni/Genian/service.dat """
        p = self.p
        path_str = ''.join([p["AGENT_INSTALL_PREFIX"], '/',
                            p["AGENT_VENDER_NAME"], '/',
                            p["AGENT_NAME"], '/',
                            p["AGENT_DATABASE_NAME"]])
        return path_str

    def get_agent_path(self):
        """ /usr/local/Geni/Genian """
        p = self.p
        path_str = ''.join([p["AGENT_INSTALL_PREFIX"], '/',
                            p["AGENT_VENDER_NAME"], '/',
                            p["AGENT_NAME"]])
        return path_str

    def get_policy_server_address_file_path(self):
        """ /usr/local/Geni/Genian/soapserver """
        p = self.p
        path_str = ''.join([p["AGENT_INSTALL_PREFIX"], '/',
                            p["AGENT_VENDER_NAME"], '/',
                            p["AGENT_NAME"], '/',
                            p["AGENT_POLICY_SERVER_FILE_NAME"]])
        return path_str

    def get_agent_process_name(self):
        """ gnagent.py """
        p = self.p
        path_str = ''.join([p["AGENT_GNAGENT_DAEMON_NAME"]])
        return path_str

    def get_log_dir_path(self):
        """ /usr/local/Geni/Genian/log """
        p = self.p
        path_str = ''.join([p["AGENT_INSTALL_PREFIX"], '/',
                            p["AGENT_VENDER_NAME"], '/',
                            p["AGENT_NAME"], '/',
                            p["AGENT_LOG_DIR"]])
        return path_str

    def get_plugin_dir_name(self):
        """ Plugin """
        p = self.p
        path_str = ''.join([p["AGENT_PLUGIN_DIR"]])
        return path_str

    def get_version_file_path(self):
        """ /usr/local/Geni/Genian/version.enc """
        p = self.p
        path_str = ''.join([self.get_agent_path(), '/', p["AGENT_VERSION_ENC_NAME"]])
        return path_str

    def get_user_enc_file_path(self):
        """ /usr/local/Geni/Genian/version.enc """
        p = self.p
        path_str = ''.join([self.get_agent_path(), '/', p["AGENT_USER_ENC_NAME"]])
        return path_str

    def get_gnagent_dir(self):
        """ /usr/local/Geni/Genian/Agent """
        p = self.p
        path_str = ''.join([p["AGENT_INSTALL_PREFIX"], os.sep,
                            p["AGENT_VENDER_NAME"], os.sep,
                            p["AGENT_NAME"], os.sep,
                            p["AGENT_DIR_NAME"]])
        return path_str


