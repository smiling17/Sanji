# -*- coding: utf-8 -*-

import os
import sys

sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))

import logging
from logging.handlers import TimedRotatingFileHandler
import datetime
from include.common import agent_install_path_list


LOG = logging.getLogger("crumbs")


def set_logging():
    LOG.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s [%(levelname).3s|%(filename)-12s: %(lineno)-s| %(process)-d] %(message)s')

    dt = datetime.datetime
    now = dt.now()

    path_info = agent_install_path_list
    dirname = ''.join([path_info["AGENT_INSTALL_PREFIX"], '/', path_info["AGENT_VENDER_NAME"], '/',
                       path_info["AGENT_NAME"], '/', path_info["AGENT_LOG_DIR"]])

    if not os.path.isdir(dirname):
        os.mkdir(dirname)

    rotateHandler = TimedRotatingFileHandler(dirname + '/' + path_info["AGENT_LOG_FILE_NAME"], 'D', 1)
    rotateHandler.setFormatter(formatter)
    LOG.addHandler(rotateHandler)
    return LOG


def set_logging_parameter(argument_list):
    LOG.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s [%(levelname).3s|%(filename)-12s: %(lineno)-s| %(process)-d| %(threadName)s| %(thread)d] %(message)s')

    dt = datetime.datetime
    now = dt.now()

    path_info = agent_install_path_list
    if '-d' in argument_list:
        devopt_path = os.getcwd()
    else:
        devopt_path = ''

    dirname = ''.join([devopt_path, '/',
                       path_info["AGENT_INSTALL_PREFIX"], '/',
                       path_info["AGENT_VENDER_NAME"], '/',
                       path_info["AGENT_NAME"], '/',
                       path_info["AGENT_LOG_DIR"]])

    if not os.path.isdir(dirname):
        """
        mkdir -p 옵션과 같이 하위 디렉토리까지 생성함.
        """
        os.makedirs(dirname)

    rotateHandler = TimedRotatingFileHandler(dirname + '/' + path_info["AGENT_LOG_FILE_NAME"], 'D', 1)
    rotateHandler.setFormatter(formatter)
    LOG.addHandler(rotateHandler)
    return LOG


def get_log():
    return LOG



