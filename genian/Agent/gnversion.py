# -*- coding: utf-8 -*-
#!/usr/bin/python

import sys
import os
import string

"""
/usr/local/Geni/Genian 경로를 PYTHONPATH에 추가하여야 라이브러리 참조가 가능함.
"""
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))

import sqlite3
import subprocess

import Lib.fileversioninfo as versionInfo
from include.common import PATH_VERIFY, PATH_VERIFY_TRUNC, AgentInstallPathInfo


ID_RESULT_FAIL = 0
ID_RESULT_SUCCESS = 1

ID_RESULT = 0
ID_RESULT_STR = 1

KEYSTR_PROFILE_VERSION = 'profile.version'


class Shell:
    @staticmethod
    def rm(name, is_dir=False):
        if (is_dir == True):
            command = ''.join(["rm -rf ", name, "/*"])
        else:
            command = ''.join(["rm -f ", name])
        Shell.execute(command)

    @staticmethod
    def execute(command):
        popen = subprocess.Popen(
            command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        (stdoutdata, stderrdata) = popen.communicate()

        if (len(stderrdata) == 0):
            return [ID_RESULT_SUCCESS, stdoutdata]
        else:
            return [ID_RESULT_FAIL, stderrdata]


class Dataservice:
    conn = None

    def __init__(self):
        self.p = AgentInstallPathInfo()
        self.conn = sqlite3.connect(self.p.get_database_path(), isolation_level=None, check_same_thread=False)
        query = 'CREATE TABLE IF NOT EXISTS INFODATA (KEYPATH varchar(1024), VALUE Text, Primary Key(KEYPATH))'
        cur = self.conn.cursor()
        cur.execute(query)

    def close(self):
        self.conn.close()

    def get_data(self, keypath, default=''):
        data = [keypath]
        cur = self.conn.cursor()
        query = 'SELECT VALUE FROM INFODATA WHERE KEYPATH = ?'
        cur.execute(query, data)
        rows = cur.fetchall()
        for row in rows:
            if not row[0]:
                return default
            else:
                return row[0]
        return default

    def set_data(self, keypath, value):
        data = (keypath, value)
        cur = self.conn.cursor()
        query = 'INSERT OR REPLACE INTO INFODATA (KEYPATH, VALUE) VALUES (?, ?)'
        cur.execute(query, data)

    def del_data(self, keypath):
        data = [keypath]
        cur = self.conn.cursor()
        query = 'DELETE FROM INFODATA WHERE KEYPATH = ?'
        cur.execute(query, data)


def verify_agent():
    """
    verify_agent
    agent 무결성 검증
    Verify.enc 파일의 작성된 파일목록의 hash값을 비교하여 변경여부를 확인한다.
    """
    p = AgentInstallPathInfo()
    try:
        # Verify.enc 파일 존재여부 확인
        fp = open(PATH_VERIFY, 'r')
        fp.close()

        # 앞쪽에 28바이트로 작성된 signature 제거한 임시 파일 생성
        command = ''.join(["dd if='", PATH_VERIFY, "' bs=28 skip=1 of='", PATH_VERIFY_TRUNC, "' 2> /dev/null"])
        result = Shell.execute(command)

        enc = Crypte()
        crypte_key = "".join([enc.RANDOM_GEN_DATA[idx] for idx in enc.CRYPT_KEY_IDX])
        # 임시 파일에서 복호화
        command = ''.join(["openssl enc ", enc.CRYPT_ENCODING,
                        " -d -K ", crypte_key,
                        " -iv ", enc.CRYPT_VECTOR,
                        " -in ", PATH_VERIFY_TRUNC,
                        " 2> /dev/null"])
        result = Shell.execute(command)
        fileinfolist = result[1].replace("\x00", '')

        del crypte_key
        del enc

        # 임시파일 삭제
        command = ''.join(["rm -f ", PATH_VERIFY_TRUNC])
        Shell.execute(command)

        fileinfolist = fileinfolist.replace("%TARGET%/", p.get_agent_path() + "/")
        result = fileinfolist.splitlines()

        for fileinfos in result:
            ''''''
            fileinfo = fileinfos.split('|')
            if "gnagent.py" in fileinfo[0]:
                current_ver2 = fileinfo[2].split('-')
                dataservice.set_data("node.version", current_ver2[0])

            try:
                ''''''
                cf = open(fileinfo[0], 'r')
                cf.close()
            except Exception as e:
                continue

            # file hash check
            command = ''.join(["openssl dgst -sha256 ", fileinfo[0], " | awk '{print $2}'"])
            Shell.execute(command)
        return 1

    except Exception as e:
        return 0


def show_version():
    """
        리눅스 에이전트 패키지의 버전 정보를 stdout으로 출력한다.
        return 값이 None인 경우, 호출자 쪽에서 print()로 출력할 경우, 마지막에 None이 출력되어 Return  값을 "" (공백)으로 처리함.
        ex>
        cd /usr/local/Geni/Genian
        python -c 'import gnstart; print(gnstart.show_version())'
    """

    productname = dataservice.get_data("custominfo.productname")
    productcopyright = dataservice.get_data("custominfo.productcopyright")

    p = AgentInstallPathInfo()
    if len(productname) and len(productcopyright):
        print("\t{0}".format(productname))
        print("\t{0}\n".format(productcopyright))
    else:
        print("\tGenian NAC v5.0")
        print("\tGENIANS, INC. All rights reserved.\n")


    """ 
    현재 로컬 장비에 설치된 리눅스 에이전트의 버전 문자열(NAC-AGENT-R-68488-5.0.13.0509). 
    """
    version_str = dataservice.get_data("node.version")
    version_full_str = ''
    info = versionInfo.get_plugin_version_list_from_file()
    for el in info:
        file_name_extend = os.path.split(el.file_path)

        if p.get_agent_path() == file_name_extend[0] and len(file_name_extend[1]) == 0:
            version_full_str = ''.join(['NAC-LNXAGENT', '-', el.file_branch, '-', el.file_revision, '-', el.file_version])

    print("\tVersion \t: {0} ({1})".format(version_str, version_full_str))
    print("\tDescription \t: Genian Agent for Linux\n")
    print("\t=========================================================")
    print("\t|\t{0}\t|\t     {1}       \t|".format("Plugin Name", "Version"))
    print("\t---------------------------------------------------------")

    for el in info:
        """
        '/usr/local/Geni/Genian/Plugin/ACT7005_ScriptCtrl', 'scriptctrl.py' 
        """
        file_name_extend = os.path.split(el.file_path)

        """
        'scriptctrl'
        """
        file_name = file_name_extend[1].split('.')
        if "__init__" in file_name:
            continue

        if p.get_plugin_dir_name() in el.file_path:
            print("\t|\t{0}     \t|\t{1}-{2} \t|".format(os.path.basename(el.file_path).split('.')[0], el.file_version, el.file_revision))

    print("\t=========================================================")
    return ""


def main(*args):

    global dataservice
    dataservice = Dataservice()

    """
        실행시 "-v" 옵션이 있는 경우.
    """
    if '-v' in args:
        show_version()

    """
        아래와 같이 실행시 추가 옵션이 없거나 "-h"가 있는 경우.

        >python ./Agent/gnversion.py
            usage: python gnversion.py [-h] [-v]
        >python ./Agent/gnversion.py -h
            usage: python gnversion.py [-h] [-v]
    """
    if '-h' in args or len(args) == 1:
        print("usage: python {0} [-h] [-v]".format(os.path.basename(sys.argv[0])))

    dataservice.close()
    sys.exit()


if __name__ == '__main__':
    main(*sys.argv)

