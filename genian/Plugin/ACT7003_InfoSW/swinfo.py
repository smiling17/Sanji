# -*- coding: utf-8 -*-
# 한글 주석 사용 가능하도록 encoding 추가

import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.getcwd())))
sys.path.append(''.join([os.path.dirname(os.path.dirname(os.getcwd())) + os.sep + 'Agent']))
sys.path.append(''.join([os.path.dirname(os.path.dirname(os.getcwd())) + os.sep + 'Lib']))
sys.path.append(''.join([os.path.dirname(os.path.dirname(os.getcwd())) + os.sep + 'include']))

import datetime
import threading
import base64

from Agent.gnagent import pluginbase
from Lib.debug import set_logging, get_log, set_logging_parameter

import include.constants as plugin_const
import Lib.fileversioninfo as versionInfo

# Application string
IDS_SW_DPKG = "dpkg"
IDS_SW_RPM = "rpm"
IDS_SW_PACMAN = "pacman"
IDS_SW_PROFILER = "system_profiler"


class swinfo(pluginbase):
    '''
    swinfo
    '''

    def create_dicts(self):
        '''
        create_dicts
        '''
        pluginbase.create_dicts(self)

        """
        IDS_SW_DPKG: "dpkg --get-selections | awk '{print $1}' | sed -e /^lib/d",
        """
        self.dict_app = {
            IDS_SW_DPKG: "dpkg -l | awk '/^(ii|pi)/ {print $2 "'"@"'" $3}'",
            IDS_SW_RPM: "rpm -qa --queryformat '%{NAME}:%{VERSION}:%{INSTALLTIME}:%{FILENAMES}\n' | sed -e /^lib/d",
            IDS_SW_PACMAN: "pacman -Qqen",
            IDS_SW_PROFILER: "system_profiler SPApplicationsDataType"
        }

        self.list_column = [
            "NI_NAME",
            "NI_DATE",
            "NI_TYPE",
            "NI_VERSION",
            "NI_PATH"
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
                if self.check_tool(cmdid) == False:
                    continue

                list_result = self.execute(command)
                if list_result[plugin_const.ID_RESULT] == plugin_const.ID_RESULT_FAIL:
                    continue

                if self.check_complete() == True:
                    break

                list_sw = []
                if cmdid == IDS_SW_DPKG:
                    ''''''
                    list_raw_sw = list_result[plugin_const.ID_RESULT_STR].split("\n")

                    list_sw_name = []
                    for raw_sw in list_raw_sw:
                        ''''''
                        swdetail = raw_sw.split("@")
                        sw = swdetail[0]
                        if len(swdetail) > 1:
                            version = swdetail[1]

                        # which 명령어로 실제 설치위치를 찾음
                        cmdresult = self.execute("which " + sw)
                        installpath = cmdresult[plugin_const.ID_RESULT_STR].split("\n")[0]
                        if installpath == "":
                            continue

                        # 파일 생성 일자 조회
                        cdate = os.path.getmtime("/var/lib/dpkg/info/" + sw + ".list")
                        ctime = datetime.datetime.fromtimestamp(cdate).strftime('%Y-%m-%d %H:%M:%S')

                        if (sw not in list_sw_name):
                            ''''''
                            sw_info = [
                                sw,
                                ctime,  # install date
                                "1",
                                version,  # version
                                installpath  # install path
                            ]

                            list_sw_name.append(sw)
                            list_sw.append(sw_info)
                elif cmdid == IDS_SW_RPM:
                    ''''''
                    list_raw_sw = list_result[plugin_const.ID_RESULT_STR].split("\n")
                    list_sw_name = []
                    for raw_sw in list_raw_sw:
                        ''''''
                        if len(raw_sw) == 0:
                            continue
                        sw = raw_sw.split(":")
                        if sw[0] not in list_sw_name:
                            ''''''
                            ctime = datetime.datetime.fromtimestamp(float(sw[2])).strftime('%Y-%m-%d %H:%M:%S')

                            sw_info = [
                                sw[0],
                                ctime,  # install date
                                "1",
                                sw[1],  # version
                                sw[3]  # install path
                            ]

                            list_sw_name.append(sw[0])
                            list_sw.append(sw_info)
                elif cmdid == IDS_SW_PACMAN:
                    pass
                elif cmdid == IDS_SW_PROFILER:
                    pass

                if not len(list_sw):
                    continue

                """
                Genian Agent(for Linux)의 버전 정보 추가.
                """
                agent_file_lists = versionInfo.get_plugin_version_list_from_file()

                for el in agent_file_lists:

                    dir, file = os.path.split(el.file_path)

                    create_time = os.path.getmtime(dir)
                    create_time_str = datetime.datetime.fromtimestamp(create_time).strftime('%Y-%m-%d %H:%M:%S')

                    """
                    Genian Agent(for Linux)의 버전 정보 추가.
                    """
                    if "Agent" in dir:
                        sw = ['Genian Agent',
                              ''.join([el.file_version + '-' + el.file_revision]),
                              create_time_str,
                              dir
                            ]
                    elif "ACT" in dir:
                        """
                        Genian Agent Plugin의 버전 정보 추가.
                        """
                        plugin_path = dir.split(os.sep)
                        sw = [''.join(['Genian Agent Plugin - ' + plugin_path[6]]),
                              ''.join([el.file_version + '-' + el.file_revision]),
                              create_time_str,
                              dir
                            ]
                    else:
                        """
                        /usr/local/Geni/Genian/Agent 폴더와 /usr/local/Geni/Genian/Plugin/ACT* 폴더 이외에는 무시함.
                        """
                        continue

                    """
                    동일한 소프트웨어 이름이 이미 추가되어 있으면, 추가하지 않도록 함.
                    동일한 폴더내에 여러 파일이 있는 경우, 중복 추가될 수 있으므로, 중복 체크함.
                    """
                    if len(list_sw_name):
                        if sw[0] in list_sw_name:
                            continue
                        else:
                            sw_info = [
                                sw[0],
                                create_time_str,  # install date
                                "2",
                                sw[1],  # version
                                sw[3]  # install path
                            ]

                            list_sw_name.append(sw[0])
                            list_sw.append(sw_info)

                self.list_data = [
                    [
                        "SW",
                        self.list_column,
                        list_sw
                    ]
                ]

                self.set_profileresult(plugin_const.GN_PROFILE_WORKTYPE_ACTION, "1", "")
        except Exception as e:
            self.set_profileresult(plugin_const.GN_PROFILE_WORKTYPE_ACTION, "0", str(e))
