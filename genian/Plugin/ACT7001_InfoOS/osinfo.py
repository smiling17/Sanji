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

import Lib.encoding as encoding
import Lib.platform_profile as platform_profile
import Lib.networkinfo as netinfo


"""
    아래의 ID_NI_xxx 는 정책서버에서도 동일하게 사용하므로 주의.
    ID_NI_xxx 는 정책서버에서 컬럼 이름으로 사용함.
"""

ID_NI_OSNAME = 0
ID_NI_VERSION = 1
ID_NI_LANGUAGE = 2
ID_NI_OWNER = 3
ID_NI_OSID = 4

"""
    ID_NI_LOCALE 이름만 보면 locale 정보를 요구하는 것처럼 보이지만,  
    locale 정보중 로컬 언어 정보(en, ko etc)를 가져와서 정책서버에서
    사용하는 Language ID(1042, 1033, 1041)로 변환한 값.
    
    참고 : Lib/encoding.py 
        dict_locale = {
            "ko": ["1042", "Korean"],
            "en": ["1033", "English"],
            "ja": ["1041", "Japanese"],
            "all": "0"
        }
"""

ID_NI_LOCALE = 5

ID_NI_GATEWAY = 6
ID_NI_DNSSERVER = 7


class osinfo(pluginbase):

    def create_dicts(self):
        pluginbase.create_dicts(self)

        self.list_column = [
            "NI_OSNAME",
            "NI_VERSION",
            "NI_LANGUAGE",
            "NI_OWNER",
            "NI_OSID",
            "NI_OSLOCALE",
            "NI_GATEWAY",
            "NI_DNSSERVER",
        ]

        self.list_data = [
        ]

        self.dict_locale = {
            "ko": ["1042", "Korean"],
            "en": ["1033", "English"],
            "ja": ["1041", "Japanese"],
            "all": "0"
        }

    def run(self):
        list_os = [
            "",  # OSNAME
            "",  # VERSION
            "",  # LANGUAGE
            "",  # OWNER
            "",  # OSID
            "",  # LOCALE
            "",  # GATEWAY
            "",  # DNSSERVER
        ]

        """
        로컬 시스템의 운영체제 이름, 버전 수집
        OSID 리스트에서 해당 운영체제에 대한 ID 정보를 수집.
        """

        list_os[ID_NI_OSNAME] = platform_profile.get_os_name()
        list_os[ID_NI_VERSION] = platform_profile.get_os_version()
        list_os[ID_NI_OSID] = platform_profile.get_system_osid()

        try:
            """
            로컬 시스템의 언어 코드 정보 수집.
            ["1033", "English"]
            """

            lang_info = encoding.get_system_lang()
            list_os[ID_NI_LOCALE] = lang_info[0]
            list_os[ID_NI_LANGUAGE] = lang_info[1]

            """
            로컬 시스템의 로그인되어 있는 사용자 정보 수집.
            os.getlogin() : 환경 변수 LOGNAME 의 정보와 동일함.
            """

            try:
                list_os[ID_NI_OWNER] = os.getlogin()
            except Exception as e:
                print("We were unable to verify your account information. ERRMSG=" + str(e))

            """
            로컬 시스템의 Gateway 정보 수집.
            """

            net = netinfo.NetworkInfo()
            list_os[ID_NI_GATEWAY] = net.get_gateway_ip_from_route()

            self.list_data = [
                [
                    "OS",
                    self.list_column,
                    [list_os]
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
