# -*- coding: utf-8 -*-

import sys
import os

sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))

ID_RESULT = 0
ID_RESULT_STR = 1

ID_RESULT_FAIL = 0
ID_RESULT_SUCCESS = 1


"""
프로파일 작업 타입 
"""

""" PMS """
GN_PROFILE_WORKTYPE_PMS = 1

""" 액션 """
GN_PROFILE_WORKTYPE_ACTION = 2

""" 제약사항 (사용안함) """
GN_PROFILE_WORKTYPE_CONST = 3

""" 액션수행결과를 Elasticsearch 로 전송 """
GN_PROFILE_WORKTYPE_ELASTICSEARCH = 4


# 프로파일 결과
GN_PROFILE_RESULT_PFTYPE = "pftype"     # profileresult_t.pftype
GN_PROFILE_RESULT_PFID = "pfid"         # profileresult_t.pfid
GN_PROFILE_RESULT_PFGROUPID = "pfgorupid"       # profileresult_t.result_code
GN_PROFILE_RESULT_PFGROUPOP = "groupop"         # profileresult_t.result_msg
GN_PROFILE_RESULT_RESULTCODE = "result-code"    # profileresult_t.result_code
GN_PROFILE_RESULT_RESULTMSG = "result-msg"      # profileresult_t.result_msg
GN_PROFILE_RESULT_ACTIVE = "active"             # profileresult_t.active

KEYSTR_ACTION_RESULT = 'send.action.result'
KEYSTR_PLUGIN_RESULT = 'send.plugins.data'
KEYSTR_NODEID = 'node.id'
KEYSTR_NODE_NOUPDATE = 'node.aanoupdate'
KEYSTR_PROFILE_INTERVAL = 'profile.policyinterval'      # 정책 수신 주기
KEYSTR_PROFILE_AUTHSTATUS = 'profile.authstatus'
KEYSTR_PROFILE_VERSION = 'profile.version'
KEYSTR_NODE_KEEPALIVE = 'node.keepalive'