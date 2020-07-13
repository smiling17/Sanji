# -*- coding: utf-8 -*-
# 한글 주석 사용 가능하도록 encoding 추가

import os
import sys

sys.path.append(os.path.dirname(os.path.dirname(os.getcwd())))
sys.path.append(''.join([os.path.dirname(os.path.dirname(os.getcwd())) + os.sep + "Lib"]))
sys.path.append(''.join([os.path.dirname(os.path.dirname(os.getcwd())) + os.sep + "Agent"]))
sys.path.append(''.join([os.path.dirname(os.path.dirname(os.getcwd())) + os.sep + "include"]))

import base64

from Agent.gnagent import pluginbase
import include.constants as plugin_const


# Application string
IDS_CPUINFO = "cat /proc/cpuinfo"
IDS_MEMINFO = "free"
IDS_HDDINFO = "udevadm"

IDS_COLUMN_CPU = [
    "NI_MANUFACTURE",
    "NI_MOTHERBOARDNAME",
    "NI_NAME",
    "NI_REVISION",          # int
    "NI_CPUTYPE",           # int
    "NI_CPUFAMILY",         # int
    "NI_CPUMODEL",          # int
    "NI_CPUSTEPPING",       # int
    "NI_CPUCLOCK",          # int
    "NI_BATTERY",           # int
    "NI_USAGECPU",          # int
    "NI_TEMPERATURE"        # int
]
IDS_COLUMN_MEM = [
    "NI_TOTALMEM",          # int
    "NI_USEDMEM",           # int
    "NI_TOTALSWAP",         # int
    "NI_USEDSWAP"           # int
]
IDS_COLUMN_STORAGE = [
    "NI_FILESYSTEM",
    "NI_NAME",
    "NI_TOTAL",
    "NI_TYPE",
    "NI_USED",
    "NI_SERIAL_STR",
    "NI_SERIAL",
    "NI_VOLID",
    "NI_VENDOR",
    "NI_MODEL",
    "NI_RASECTOR"
]


class hwinfo(pluginbase):
    '''
    hwinfo
    '''

    def create_dicts(self):
        '''
        create_dicts
        '''
        pluginbase.create_dicts(self)

        self.dict_app = {
            IDS_CPUINFO: {
                "NI_MANUFACTURE": "grep 'vendor_id' /proc/cpuinfo | tail -1 | awk '{print $3}'",
                "NI_MOTHERBOARDNAME": "cat /sys/devices/virtual/dmi/id/board_vendor",
                "NI_NAME": "grep 'model name' /proc/cpuinfo | tail -1 | awk -F': ' '{print $2}'",
                "NI_CPUFAMILY": "grep 'cpu family' /proc/cpuinfo | tail -1 | awk -F': ' '{print $2}'",
                "NI_CPUSTEPPING": "grep 'stepping' /proc/cpuinfo | tail -1 | awk -F': ' '{print $2}'",
                "NI_CPUCLOCK": "grep 'cpu MHz' /proc/cpuinfo | tail -1 | awk -F': ' '{print $2}'",
            },
            IDS_MEMINFO: {
                "NI_TOTALMEM": "free | grep Mem: | awk '{print $2}'",
                "NI_USEDMEM": "free | grep Mem: | awk '{print $3}'",
                "NI_TOTALSWAP": "free | grep Swap: | awk '{print $2}'",
                "NI_USEDSWAP": "free | grep Swap: | awk '{print $3}'",
            },
            IDS_HDDINFO: {
                # "NI_SERIAL" : "udevadm info --query=all --name=/dev/sda | grep ID_SERIAL= | awk -F'=' '{print $2}'",
                "DFINFO": "df -T | grep '^/dev' | awk -F'[ :]+' '{print $1,$2, $3, $4, $5}'"
            }
        }
        self.list_data = [
        ]

        self.list_column = {
            "CPU": IDS_COLUMN_CPU,
            "MEM": IDS_COLUMN_MEM,
            "STORAGE": IDS_COLUMN_STORAGE
        }

    def run(self):
        '''
        run
        '''
        list_cpu = []
        list_mem = []
        list_storage = []
        try:
            for cmdid, commands in self.dict_app.items():
                ''''''
                if (self.check_tool(cmdid) == False):
                    continue

                if (cmdid == IDS_CPUINFO):
                    ''''''
                    cpuinfo = ["", "", "", "0", "0", "0",
                               "0", "0", "0", "0", "0", "0"]
                    for colname, command in commands.items():
                        ''''''
                        list_result = self.execute(command)
                        if (list_result[plugin_const.ID_RESULT] == plugin_const.ID_RESULT_FAIL):
                            continue

                        idx = IDS_COLUMN_CPU.index(colname)
                        cpuinfo[idx] = list_result[1].rstrip("\n")
                    ''''''
                    list_cpu.append(cpuinfo)

                elif (cmdid == IDS_MEMINFO):
                    ''''''
                    meminfo = ["0", "0", "0", "0"]
                    for colname, command in commands.items():
                        ''''''
                        list_result = self.execute(command)
                        if (list_result[plugin_const.ID_RESULT] == plugin_const.ID_RESULT_FAIL):
                            continue

                        idx = IDS_COLUMN_MEM.index(colname)
                        meminfo[idx] = list_result[1].rstrip("\n")
                    ''''''
                    list_mem.append(meminfo)

                elif (cmdid == IDS_HDDINFO):
                    ''''''
                    for colname, command in commands.items():
                        ''''''
                        list_result = self.execute(command)
                        if (list_result[plugin_const.ID_RESULT] == plugin_const.ID_RESULT_FAIL):
                            continue

                        if (colname == "DFINFO"):
                            ''''''
                            for diskinfos in list_result[1].split("\n"):
                                ''''''
                                if (len(diskinfos) == 0):
                                    continue

                                hddinfo = ["", "", "", "", "",
                                           "", "", "", "", "", ""]
                                diskinfo = diskinfos.split(" ")

                                hddinfo[IDS_COLUMN_STORAGE.index("NI_NAME")] = diskinfo[0]
                                hddinfo[IDS_COLUMN_STORAGE.index("NI_FILESYSTEM")] = diskinfo[1]
                                hddinfo[IDS_COLUMN_STORAGE.index("NI_TOTAL")] = diskinfo[2]
                                hddinfo[IDS_COLUMN_STORAGE.index("NI_USED")] = diskinfo[3]
                                hddinfo[IDS_COLUMN_STORAGE.index("NI_TYPE")] = "0"
                                hddinfo[IDS_COLUMN_STORAGE.index("NI_RASECTOR")] = "0"

                                """
                                /dev/sda 디바이스가 중복될수 있음.
                                디바이스 이름(/dev/sda1, /dev/sda2 etc)으로 중복이 있는지 체크한다.
                                openSuSE 42.x 에서는 /dev/sda2 가 중복적으로 생성이 됨.
                                """
                                if len(list_storage):
                                    dev_list = []
                                    for el in list_storage:
                                        """
                                            /dev/sda1 , /dev/sda2 와 같은 저장 장치 이름으로 구성된 목록을 생성함.
                                        """
                                        dev_list.append(el[IDS_COLUMN_STORAGE.index("NI_NAME")])

                                    if hddinfo[IDS_COLUMN_STORAGE.index("NI_NAME")] in dev_list:
                                        continue
                                    else:
                                        list_storage.append(hddinfo)
                                else:
                                    list_storage.append(hddinfo)
                        else:
                            ''''''
                            idx = IDS_COLUMN_STORAGE.index(colname)
                            hddinfo[idx] = list_result[1].rstrip("\n")
                            list_storage.append(hddinfo)

                self.list_data = [
                    [
                        "CPU",
                        IDS_COLUMN_CPU,
                        list_cpu
                    ],
                    [
                        "MEM",
                        IDS_COLUMN_MEM,
                        list_mem
                    ],
                    [
                        "STORAGE",
                        IDS_COLUMN_STORAGE,
                        list_storage
                    ]
                ]

            self.set_profileresult(plugin_const.GN_PROFILE_WORKTYPE_ACTION, "1", "")
        except Exception as e:
            self.set_profileresult(plugin_const.GN_PROFILE_WORKTYPE_ACTION, "0", str(e))


    def __init__(self):
        '''
        __init__
        '''
        self.create_dicts()
