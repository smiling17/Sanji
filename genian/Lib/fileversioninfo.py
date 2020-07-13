# -*- coding: utf-8 -*-

import os
import sys
import subprocess
import tempfile
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
import Agent.gnconfigure as g_conf

import include.common as comm
import Lib.crypto as crypto


FILE_PATH, \
FILE_SIZE, \
FILE_VERION, \
FILE_HASH,\
FILE_MAX = range(0, 5)

GNAGENT_FILE_VERSION, \
GNAGENT_FILE_REVISION, \
GNAGENT_FILE_BRANCH, \
GNAGENT_FILE_MAX = range(0, 4)

class FileVersionInfo(object):

    def __init__(self, info):
        """
        파일 버전에 대한 정보를 저장한다.

        :param info: [ 파일 경로, 파일 사이즈, 파일 버전 , 파일 해쉬 값]
                 [/media/sf_f/001_SRC/ALDER/branches/CURRENT/agent_linux/tests/version.enc',
                '112',
                '5.0.14.0611-69160',
                '75ec50c55004ef81df3299e6709d77ec91e851a08722d7545a73e10fa224d6dc]

        :return: self
        """
        p = comm.AgentInstallPathInfo()
        self.file_path = info[FILE_PATH]

        """
        gnagent.py 파일에만 브랜치 정보가 추가되어 있음.
        '5.0.14.0611-69160-C'
        """
        version_info = info[FILE_VERION].split('-')
        if len(version_info) >= 3:
            self.file_version = version_info[GNAGENT_FILE_VERSION]
            self.file_revision = version_info[GNAGENT_FILE_REVISION]
            self.file_branch = version_info[GNAGENT_FILE_BRANCH]
        else:
            self.file_version = version_info[GNAGENT_FILE_VERSION]
            self.file_revision = version_info[GNAGENT_FILE_REVISION]
            self.file_branch = ''

        self.file_size = info[FILE_SIZE]
        self.file_hash = info[FILE_HASH]

    def to_str(self):
        pass


def get_file_version_list_from_param(file_info):
    """
    :param file_info: 파일 버전에 대한 정보(list)
    :return: FileVersionInfo 객체를 리턴함.
    """
    return FileVersionInfo(file_info)


def get_plugin_version_list_from_file():
    """
    Verify.enc 파일에서 플러그인 파일의 버전 정보를 복호화 하여 리턴.
    :return: file list
    """

    p = comm.AgentInstallPathInfo()
    result = crypto.decryption(comm.PATH_VERIFY)

    file_version = []
    for el in result:
        fileinfo = el.replace("%TARGET%/", p.get_agent_path() + "/").split('|')
        file_version.append(get_file_version_list_from_param(fileinfo))

    return file_version


