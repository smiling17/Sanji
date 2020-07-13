# -*- coding: utf-8 -*-
# 한글 주석 사용 가능하도록 encoding 추가

import sys
import base64
import gnagent
import threading

from Agent.gnagent import Shell
from Agent.gnagent import pluginbase
from Lib.debug import set_logging_parameter

from include.common import (
    AgentInstallPathInfo,
)

import include.constants as plugin_const

class scriptctrl(pluginbase):
    '''
    scriptctrl
    '''

    def create_dicts(self):
        '''
        create_dicts
        '''
        pass
    
    def run(self):
        '''
        run
        '''
        try:
            # print self.pluginconfig
            # parsing config
            command = self.pluginconfig["ACT7005-COMMAND"]
            sendoption = self.pluginconfig["ACT7005-ALWAYSSEND"]
            
            param = ''
            option = ''
            if command == 'ps':
                param = self.pluginconfig["ACT7005-PSCMD_OPTION"]
                
            elif command == 'netstat':
                param = self.pluginconfig["ACT7005-NETSTAT_OPTION"]

            if param is not None:
                idx = param.find(';')
            
                if idx < 0:
                    option = param
                elif param:
                    option = param[0:idx]
            
            if sendoption == '1':
                self.always_profileresult_send = True
            
            result = Shell.execute(command + ' ' + option)

            if result[0]:
                self.set_profileresult(plugin_const.GN_PROFILE_WORKTYPE_ELASTICSEARCH, "1", "")
            else:
                self.set_profileresult(plugin_const.GN_PROFILE_WORKTYPE_ELASTICSEARCH, "0", str(result[1]))
        except Exception as e:
            self.set_profileresult(plugin_const.GN_PROFILE_WORKTYPE_ELASTICSEARCH, "0", str(e))

        # action result 만 존재
        self.list_data = []