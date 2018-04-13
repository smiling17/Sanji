#!/bin/bash

CYAN="\e[1;36m"
GREEN="\e[1;32m"
RED="\e[0;31m"
END="\e[0m"

DEVEL_DIR_SET="/etc/network/interfaces.d"
DEVEL_DIR_SET_TEST="/etc/network/interface-test"
DEVEL_DIR_SET_FALSE="/etc/network/interface.e"

check_devel_network_set() {
	echo -e "${CYAN}${DEVEL_DIR_SET}내 네트워크 설정 정보 유무를 확인합니다. ${END}"
	if [ -d ${DEVEL_DIR_SET} ]
	then
		echo -e "${GREEN}${DEVEL_DIR_SET}가 존재합니다. ${END}"
	else
		echo -e "${RED}${DEVEL_DIR_SET}가 존재하지 않습니다. ${END}"
	fi
	if [ -d ${DEVEL_DIR_SET_FALSE} ]
	then
		echo -e "${GREEN}${DEVEL_DIR_SET_FALSE}가 존재합니다. ${END}"
	else
		echo -e "${RED}${DEVEL_DIR_SET_FALSE}가 존재하지 않습니다. ${END}"
	fi
	if [ -d ${DEVEL_DIR_SET_TEST} ]
	then
		if [ -n "$(ls -A ${DEVEL_DIR_SET_TEST} 2>/dev/null)" ]
		then
			echo -e "${GREEN}${DEVEL_DIR_SET_TEST}가 존재하고 파일도 있습니다. ${END}"
		else
			echo -e "${RED}${DEVEL_DIR_SET_TEST}가 존재하나 파일이 없습니다. ${END}"
		fi	
	else
		echo -e "${RED}${DEVEL_DIR_SET_TEST}가 존재하지 않습니다. ${END}"
	fi
}

check_devel_network_set
