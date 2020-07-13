# -*- coding: utf-8 -*-
# 한글 주석 사용 가능하도록 encoding 추가
'''
공통으로 사용하는 상수를 클래스로 정의함.
'''
import Queue
import site
import os
import logging


class NotifyId:
    NOTIFYID_REGISTER_REQ = 11  # 재인증
    NOTIFYID_PMSNOW = 36  # PMS 수행
    NOTIFYID_ACTIONNOW = 37  # 액션 수행
    NOTIFYID_SYSCONSTNOW = 38  # 제약사항 수행
    NOTIFYID_SMSNOW = 39  # SMS 수행
    NOTIFYID_AGENTDONOW = 44  # PMS/액션/제약사항/SMS/라이센스 검사(PDMC) 수행
    NOTIFYID_AGENTRUNSTAT = 45  # PMS/액션/라이센스 검사(PDMC) 작업 상태
    NOTIFYID_NEWNOTICE = 46  # 새로운 공지사항 존재
    NOTIFYID_NEWMESSAGE = 53  # 새로운 알림 메시지
    NOTIFYID_NODEAUTHCHANGED = 55  # 인증 상태 변경
    NOTIFYID_SENDDEBUGFILE = 66  # 지정된 목적지로 디버그 로그 전송
    NOTIFYID_SELECTACTIONNOW = 72  # 지정 액션 수행
    NOTIFYID_AGENTSTART = 73  # 에이전트 서비스 시작
    NOTIFYID_AGENTSTOP = 74  # 에이전트 서비스 중지
    NOTIFYID_AGENTUPDATE = 75  # 에이전트 업데이트
    NOTIFYID_RECOVERY = 76  # 센서 장애복구
    NOTIFYID_PERMMESSAGE = 77  # 고정메시지 업데이트
    NOTIFYID_NODEPOISONSTATUS = 81  # 노드차단상태
    NOTIFYID_LICSWPERMMESSAGE = 85  # 라이센스 고정메시지 업데이트


class NotifyIdRegisterReqFlag:
    """
    노드 등록 이벤트에 대해서 발신자에 따라서 구분한다.
    """
    REGISTER_REQ_FLAGE_POLICY_SERVER = 0x0,
    REGISTER_REQ_FLAGE_NETIF_CHANGE = 0x100,
    REGISTER_REQ_FLAGE_KEEPALIVE_NET_CHECK = 0x101


agent_notify_id_dict = {
    NotifyId.NOTIFYID_REGISTER_REQ: "register_req",
    NotifyId.NOTIFYID_PMSNOW: "pms_now",
    NotifyId.NOTIFYID_ACTIONNOW: "action_now",
    NotifyId.NOTIFYID_SYSCONSTNOW: "sys_cont_now",
    NotifyId.NOTIFYID_SMSNOW: "sms_now",
    NotifyId.NOTIFYID_AGENTDONOW: "agent_do_now",
    NotifyId.NOTIFYID_AGENTRUNSTAT: "agent_run_stat",
    NotifyId.NOTIFYID_NEWNOTICE: "new_notic",
    NotifyId.NOTIFYID_NEWMESSAGE: "new_msg",
    NotifyId.NOTIFYID_NODEAUTHCHANGED: "node_auth_changed",
    NotifyId.NOTIFYID_SENDDEBUGFILE: "send_debug_file",
    NotifyId.NOTIFYID_SELECTACTIONNOW: "select_action_now",
    NotifyId.NOTIFYID_AGENTSTART: "agent_start",
    NotifyId.NOTIFYID_AGENTSTOP: "agent_stop",
    NotifyId.NOTIFYID_AGENTUPDATE: "agent_update",
    NotifyId.NOTIFYID_RECOVERY: "recovery",
    NotifyId.NOTIFYID_PERMMESSAGE: "perm_message",
    NotifyId.NOTIFYID_NODEPOISONSTATUS: "node_poison_status",
    NotifyId.NOTIFYID_LICSWPERMMESSAGE: "lic_sw_perm_msg"
}


class NotifyAgentDoNowFlag:
    NOTYFLAGS_PMSNOW = 0x0100        # 운영체제 업데이트 즉시수행
    NOTYFLAGS_ACTIONNOW = 0x0200        # 에이전트액션 즉시수행
    NOTYFLAGS_SMSNOW = 0x0400        # 노드 자산정보 즉시전송
    NOTYFLAGS_SYSCONSTNOW = 0x0800
    NOTYFLAGS_LICENSE = 0x1000
    NOTYFLAGS_AUTHNOW = 0x2000
    NOTYFLAGS_PROFILE = 0x4000       # 노드 정책 재적용
    NOTYFLAGS_NOTICE = 0x8000


agent_do_now_flag_dict = {
    NotifyAgentDoNowFlag.NOTYFLAGS_PMSNOW: "pms_now",
    NotifyAgentDoNowFlag.NOTYFLAGS_ACTIONNOW: "action_now",
    NotifyAgentDoNowFlag.NOTYFLAGS_SMSNOW: "sms_now",
    NotifyAgentDoNowFlag.NOTYFLAGS_SYSCONSTNOW: "sys_const_now",
    NotifyAgentDoNowFlag.NOTYFLAGS_LICENSE: "license",
    NotifyAgentDoNowFlag.NOTYFLAGS_AUTHNOW: "auth_now",
    NotifyAgentDoNowFlag.NOTYFLAGS_PROFILE: "profile",
    NotifyAgentDoNowFlag.NOTYFLAGS_NOTICE: "notice"
}

class NotifyDataPos:
    EVENT_DATA_TYPE = 1     # KeepAlive와 Notification Event 구분
    # Notification Event인 경우, Notification ID는 데이터 패킷의 9번째 자리에 있음.(0 부터 시작)
    EVENT_DATA_NOTIFYID = 9
    # Notification Event인 경우, Notification ID중 FLAG로 구분할 때 사용하며, 10번째 위치에 있음.
    EVENT_DATA_NOTIFYID_FLAG = 10


class SoapFunctionID:
    ID_SOAP_LOGON = 0
    ID_SOAP_GETMYPROFILE = 1
    ID_SOAP_UPDATEINFO = 2
    ID_SOAP_PROFILERESULT = 3
    ID_SOAP_INSTALLINFO = 4
    ID_SOAP_HELLO2 = 5


class KeyStrDB:
    KEYSTR_ACTION_RESULT = 'send.action.result'
    KEYSTR_PLUGIN_RESULT = 'send.plugins.data'
    KEYSTR_NODEID = 'node.id'
    KEYSTR_NODE_NOUPDATE = 'node.aanoupdate'
    KEYSTR_PROFILE_INTERVAL = 'profile.policyinterval'  # 정책 수신 주기
    KEYSTR_EVENT_PORT = 'system.eventport'      # 3870
    KEYSTR_AGTEVENT_PORT = 'system.agteventport'    # 3871
    KEYSTR_HTTP_PORT = 'system.httpport'        # 80
    KEYSTR_HTTPS_PORT = 'system.httpsport'      # 443


class UnixDomainSocketPath:
    NAMEDPIPE_AGENT = './agent_pipe'
    NAMEDPIPE_START = './start_pipe'
    NAMEDPIPE_FROM_AGENT_TO_START = './agent_to_start'


class Crypte:
    CRYPT_ENCODING = "-aes-128-cbc"
    CRYPT_VECTOR = "31323334353637383930313233343536"
    RANDOM_GEN_DATA = "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"
    CRYPT_KEY_IDX = [34, 275, 10, 76, 10, 8, 10, 95, 10, 35, 10, 8, 76, 110, 34,
                         8, 34, 35, 34, 143, 76, 110, 34, 11, 10, 76, 275, 95, 76, 110, 143, 35]
    CRYPTED_USER_INFO_FILE = "user.enc"


notify_q = Queue.Queue()
