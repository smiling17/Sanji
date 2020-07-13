#! /bin/bash

INSTALL_PATH=`pwd`

#------------------------------------------------------------------------------
# 함수 목록
#------------------------------------------------------------------------------

function print_message() {
	echo -e "$*" >> ${INSTALL_PATH}/log/agent.log
}

function error() {
	print_message ""
	print_message "------------------------------------------------------------"
	print_message "ERROR\t: $1  "
	print_message "------------------------------------------------------------"
	exit 1
}

function error_check() {
	if [ $1 -ne 0 ] ; then
		error $2
	fi
}

function env_set() {
	INSTR=`echo "${1}" | tr '[:lower:]' '[:upper:]'`
	print_message "[ $INSTR    ] ----------------------------------------------"
	export PKGTARGET=${INSTR}
}

function excute() {
	print_message $1
	eval "$1"
	if [ $? -ne 0 ] ; then
		error "$1"
	fi
}

function execute() {
	eval "$1"
	if [ $? -ne 0 ] ; then
		error $1
	fi
}

function valid_ip() {
	local  ip=$1

	if [[ $ip =~ ^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$ ]]; then
		OIFS=$IFS
		IFS='.'
		ip=($ip)
		IFS=$OIFS
		if [[ ${ip[0]} -le 255 && ${ip[1]} -le 255 \
			&& ${ip[2]} -le 255 && ${ip[3]} -le 255 ]]; then
			echo "TRUE"
			return
		fi
	fi
	echo "FALSE"
}

function iptables_rules_save() {
	if [ "x$IPTABLE" = "x" ] ; then
		print_message "iptable 3871 port added"

		if [ "x$OS_DECTION" = "xUbuntu" ] ; then
			excute "sudo iptables -I INPUT 1 -p udp --dport 3871 -j ACCEPT"
			excute "sudo iptables-save > ~/iptables.rules"
		else
			excute "service iptables save"
			excute "/etc/init.d/iptables restart"
		fi
	fi
}

function iptables_rules_add_keepalive_port() {
	OS_DECTION=`cat /etc/issue 2> /dev/null | awk '{print $1}'`
	if [ "x$OS_DECTION" = "xUbuntu" ] ; then
		IPTABLE=`sudo iptables -L | grep 3871`
	else
		IPTABLE=`cat /etc/sysconfig/iptables | grep 3871`
	fi
	export IPTABLE
}

function download_lnxagent_file() {
	excute "wget https://$IPADDR/Agent/GnAgent/lnxagent.tar --no-check-certificate"
#	excute "wget http://$IPADDR/Agent/GnAgent/lnxagent.tar"
	excute "tar -xf lnxagent.tar"

	excute "sed -i 's:pwd:'`pwd`':g' gnstart"
	excute "sudo rm -rf /etc/init.d/gnstart"
	excute "chmod 755 gnagent.py gnstart gnstart.py"
	excute "sudo cp -n gnstart /etc/init.d/gnstart"

	excute "echo $IPADDR > soapserver"
}

function delete_agent_file_all() {
	excute "rm -f lnxagent.tar"
	excute "rm -f Verify.gnc"
	excute "rm -f *.py*"
	excute "rm -rf plugins"
	excute "sudo rm -f /etc/init.d/gnstart"
}

function valid_ip_address() {
	IS_IP=`valid_ip $1`

	if [ "x$IS_IP" = "xFALSE" ] ; then
		error "Exit lnxagent.sh REASON=IP NOT VALID"
	fi
}

#------------------------------------------------------------------------------
#
#------------------------------------------------------------------------------

if [ ! -f "${INSTALL_PATH}/log/agent.log" ] ; then
	mkdir ${INSTALL_PATH}/log
	touch ${INSTALL_PATH}/log/agent.log
fi


if [ "x$1" = "x" ] ; then
	# 최초 설치 시 : lnxagent_172.29.99.93.sh
	THISFILE=`basename "$0"`
	IPADDR=`echo "$THISFILE" | cut -d '_' -f 2 | cut -d '.' -f 1-4`
	OS_DECTION=`cat /etc/issue 2> /dev/null | awk '{print $1}'`

	valid_ip_address $IPADDR
	echo `whoami` > ${INSTALL_PATH}/USERID
	MODE="Initial_installation"
else
	# 에이전트에서 업그레이드 : lnxagent.sh 172.29.99.93
	IPADDR=$1

	valid_ip_address $IPADDR

	MODE="Agent_upgrade"
fi

if [ "x$MODE" = "xAgent_upgrade" ] ; then
	excute "/etc/init.d/gnstart stop"
fi

excute "delete_agent_file_all"

excute "download_lnxagent_file"

excute "iptables_rules_add_keepalive_port"

excute "iptables_rules_save"

excute "/etc/init.d/gnstart start"
