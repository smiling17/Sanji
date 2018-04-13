#!/bin/bash
#lxc

LXC_DEFAULT="./lxc"
LXC_NET_DEFAULT="./lxc-net"

LXC_SERVICE="./lxc.service"
LXC_NET_SERVICE="./lxc-net.service"
NETWORKING_SERVICE="./networking.service"
TOS_NETWORKD_SERVICE="./networkd.service"

#sed -i 's/abc/sangjun_lee/' ./test.txt
#cat ./test.txt

use_lxc_bridge() {

	line_num=`cat ${2} | grep -n "USE_LXC_BRIDGE=" | awk -F : '{print $1}'`

	if [ -z ${line_num} ]; then
		echo "No lxc infomation"
		exit 0
	fi

	sed -e "${line_num}c\
		USE_LXC_BRIDGE=\"$1\"" ./lxc
}

# (1) system-service
# lxc.service
# lxc-net.service
# networking.service
set_lxc_service() {
	line_num_after=`cat ${2} | grep -n "After=" | awk -F : '{print $1}'`
	line_num_befure=`cat ${2} | grep -n "Before=" | awk -F : '{print $1}'`
	if [ -z ${line_num_after} ]; then
		echo "No lxc infomation"
		exit 0
	fi

	#분기문을 태우자 
	sed -e "${line_num_after}c\
		After=\"$1\"" ./lxc

	if [ -z ${line_num_before} ]; then
		echo "No lxc infomation"
		exit 0
	fi

	sed -e "${line_num_before}c\
		Before=\"$1\"" ./lxc
}

set_networking_service() {
	line_num_after=`cat ${2} | grep -n "After=" | awk -F : '{print $1}'`
	line_num_befure=`cat ${2} | grep -n "Before=" | awk -F : '{print $1}'`
	return 0
}

# (2) TOS systemd
# networkd.service
set_tos_networkd_service() {
	line_num_after=`cat ${2} | grep -n "After=" | awk -F : '{print $1}'`
	line_num_befure=`cat ${2} | grep -n "Before=" | awk -F : '{print $1}'`
	return 0
}

# (3) networkd
use_lxc_bridge true ./lxc
use_lxc_bridge true ./lxc-net
