# -*- coding: utf-8 -*-

import os
import sys
import locale

dict_locale = {
    "ko": ["1042", "Korean"],
    "en": ["1033", "English"],
    "ja": ["1041", "Japanese"],
    "all": "0"
}


def get_system_encoding():
    return "utf-8"


def get_system_lang():
    """
    정책서버에서 사용하는 언어 코드를 듀플 값으로 리턴.
    :return:
        ["1033", "English"]
    """
    try:
        locale.setlocale(locale.LC_ALL, '')
    except locale.Error as msg:
        """
        locale.Error: unsupported locale setting
        여기서 locale를 설정하면 시스템 전체에 설정이 적용되므로, 에러 로그만 출력함.
        """
        print("locale setting Exception MSG=" + str(msg))

    try:
        var_locale = locale.getdefaultlocale()
        result = var_locale[0].split('_')[0]
        locale_info = dict_locale[result]
    except Exception as e:
        print("Exception as MSG=", e)
        locale_info = ["1033", "English"]

    return locale_info


def get_system_locale():
    """

    :return:
        ('en', 'US')
    """
    try:
        locale.setlocale(locale.LC_ALL, '')
    except locale.Error as msg:
        """
        locale.Error: unsupported locale setting
        여기서 locale를 설정하면 시스템 전체에 설정이 적용되므로, 에러 로그만 출력함.
        """
        print("locale setting Exception MSG=" + str(msg))

    return tuple(locale.getdefaultlocale()[0].split('_'))