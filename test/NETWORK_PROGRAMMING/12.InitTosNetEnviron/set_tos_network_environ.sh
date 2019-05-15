#!/bin/bash

CYAN="\e[1;36m"
GREEN="\e[1;32m"
RED="\e[0;31m"
END="\e[0m"

# 목표 
# - Independant on Networkd (TOS), Zone(lxc), Networking(ifupdown)
# - Flexable Network Environ Setting (Run time)
#
# 고려사항
# - Ethernet name : limit length
#
# 동작 
# 1) Check tosnet envriron (/etc/default/tosnet)
# 2) Lxc-net enable / disable
# 3) Set systemd dependancy : networkd -> networking -> lxc-net -> lxc
# 4) Create Bridge, Vethernet, ...

# default value
TOSNET_TYPE="default" # default, nat, none, zone-free, cds ...
SET_TARGET="unknown"  # host, guest
HOST_SET_START=0 
GUEST_NAME="normal"

# path, prefix, ...
TOSNET_TYPE_PATH="/etc/default/tosnet"
TOS_BRIDGE_PREFIX="tosbr_tmp_"
IFACE_INFO_PATH="/sys/class/net/" 					# linux only
HOST_NETWORK_PATH="/etc/network/interfaces_tmp" 	# ifupdown
HOST_USER_NETWORK_PATH="/etc/network/interfaces.d/" # ifupdown
GUEST_CONFIG_NAME="zone.network.conf.tmp"			# TOS zone configuration
LXC_NETWORK_TYPE_PREFIX="lxc.net.0.type" 			# 17.10 lxc
LXC_NETWORK_LINK_PREFIX="lxc.net.0.link" 			# 17.10 lxc
LXC_NETWORK_IPV4_ADDRESS_PREFIX="lxc.net.0.ipv4.address" 			# 17.10 lxc
LXC_NETWORK_IPV4_GATEWAY_PREFIX="lxc.net.0.ipv4.gateway" 			# 17.10 lxc
LXC_NETWORK_NAME_PREFIX="lxc.net.0.name" 			# 17.10 lxc
LXCBR_NORMAL_IPV4="10.0.3.105" 						# 17.10 lxc
LXCBR_SECURE_IPV4="10.0.3.106" 						# 17.10 lxc
VETH_PREFIX="veth_"
VETH_HOST_SUFFIX="_host"
VETH_GUEST_SUFFIX="_guest"

# [1] For Host (Super Zone)
check_tosnet_environ() {
	if [ -f ${TOSNET_TYPE_PATH} ]
	then
		TOSNET_TYPE=`awk -F "=" '/TOSNET_TYPE=/ { print $2 }' ${TOSNET_TYPE_PATH}`
		#TIP : $2 앞뒤로 괄호씌우면 안됨 
	else
		echo -e "check tosnet environ fail"
		${TOSNET_TYPE} = "default"
	fi
	echo -e "${CYAN}TOSNET TYPE : ${TOSNET_TYPE}${END}"
}

check_lxcbr_environ() {
	return 1
}

get_nic_type() {
	local IF=${1} TYPE="unknown"
	test -n "$IF" || return 1
	test -e "${IFACE_INFO_PATH}${IF}" || return 2
	case "`cat ${IFACE_INFO_PATH}${IF}/type`" in
		1)
			TYPE=eth
			if test -d  ${IFACE_INFO_PATH}${IF}/wireless -o \
				-L ${IFACE_INFO_PATH}${IF}/phy80211; then
				TYPE=wlan
			elif test -d ${IFACE_INFO_PATH}${IF}/bridge ; then
				TYPE=bridge
			elif test -f /proc/net/vlan/${IF} ; then
				TYPE=vlan
			elif test -f ${IFACE_INFO_PATH}${IF}/bonding ; then
				TYPE=bond
			elif test -d /sys/devices/virtual/net/${IF} ; then
				TYPE=dummy
			fi
			;;
		*)
			TYPE=unknown
			;;
	esac
	test -n ${TYPE} && echo $TYPE && return 0
	return 3;
}

get_nic_slot_type() {
	local IF=${1} TYPE="unknown"
	test -n "$IF" || return 1
	test -e "${IFACE_INFO_PATH}${IF}" || return 2
	#echo "${GREEN} [get_nic_slot_type] 을 수행해보자 ${END}"
	TYPE=`udevadm info ${IFACE_INFO_PATH}${IF} | awk -F "=" '/ID_BUS/ { print $2 }'`
	if [ ${#TYPE} -eq 0 ]; then
		TYPE="unknown"
	fi
	echo ${TYPE} && return 0
}

set_host_network() {
	sleep 2
	if [ -d ${IFACE_INFO_PATH} ]
	then
		for NIC in $(ls ${IFACE_INFO_PATH})
		do
			NIC_TYPE=`get_nic_type ${NIC}`
			SLOT_TYPE=`get_nic_slot_type ${NIC}`
			# echo -e "${CYAN}NIC TYPE : ${NIC_TYPE} / SLOT_TYPE : ${SLOT_TYPE}${END}"
			if [[ ${NIC_TYPE} == "eth" && ${SLOT_TYPE} == "pci" ]]
			then
				#create_host_bridge_with_ifupdown ${NIC}
				#create_host_bridge_with_iproute ${NIC}
				ls ${IFACE_INFO_PATH}${NIC} | grep upper \
					|| (create_host_bridge_with_iproute ${NIC})
			else
				echo -e "${CYAN}${NIC} is not a wired device${END}"
			fi
		done
	fi
}

create_host_bridge_with_iproute() {
	echo -e "${CYAN}함수왔다!!!${END}"
	local WIRED_DEV=${1} 
	local BRIDGE_NAME=${TOS_BRIDGE_PREFIX}${WIRED_DEV}
	echo -e "${CYAN}wired dev : ${WIRED_DEV}, bridge : ${BRIDGE_NAME}${END}"
	brctl addbr ${BRIDGE_NAME}
	brctl addif ${BRIDGE_NAME} ${WIRED_DEV}
	ip link set dev ${BRIDGE_NAME} up
	# TODO : ip가 필요할 경우에 대한 설정 (for 개발자)
}

create_host_birdge_with_ifupdown() {
	local WIRED_DEV=${1} 
	local BRIDGE_NAME="${TOS_BRIDGE_PREFIX}${WIRED_DEV}"
	test ${HOST_SET_START} -eq 0 && HOST_SET_START=1 && \
	echo -e "#TOS Network Configuration\n\nauto lo\n\tiface lo inet loopback" > \
	$HOST_NETWORK_PATH
	if [ ${TOSNET_TYPE} != "default" ]; then
		return 1
	fi
	if [ -f ${HOST_USER_NETWORK_PATH}${WIRED_DEV} ]; then
		echo -e "\nsource ${HOST_USER_NETWORK_PATH}${WIRED_DEV}" >> \
		${HOST_NETWORK_PATH}
	else
		echo -e "\nauto ${BRIDGE_NAME}\n\tiface ${BRIDGE_NAME} inet manual" >> \
			${HOST_NETWORK_PATH} 
		echo -e "\tbridge_ports ${WIRED_DEV}" >> ${HOST_NETWORK_PATH} 
		echo -e "\tbridge_maxwait 5" >> ${HOST_NETWORK_PATH} 
	fi
	return 0
}

create_guest_network_conf() {
	# create zone.network.conf
	# case : none / empty
	echo -e "#TOS Network Configuration for Zone Separation\n" > \
		/zone/${GUEST_NAME}/${GUEST_CONFIG_NAME}
	if [ ${TOSNET_TYPE} == "none" ]; then
		echo -e "${LXC_NETWORK_TYPE_PREFIX} = none" >> \
		/zone/${GUEST_NAME}/${GUEST_CONFIG_NAME}
	elif [ ${TOSNET_TYPE} == "nat" ]; then
		# LXC에 있는걸 쓸거냐 아니면 직접 해줄거냐
		# 있는걸 쓰자
		echo -e "${LXC_NETWORK_TYPE_PREFIX} = veth" >> \
		/zone/${GUEST_NAME}/${GUEST_CONFIG_NAME}
		echo -e "lxc.net.0.link = lxcbr0" >> \
		/zone/${GUEST_NAME}/${GUEST_CONFIG_NAME}
		echo -e "lxc.net.0.name = veth0" >> \
		/zone/${GUEST_NAME}/${GUEST_CONFIG_NAME}
		# FIXME
		if [ ${GUEST_NAME} == "normal" ]; then
			echo -e "lxc.net.0.ipv4.address = ${LXCBR_NORMAL_IPV4}" >> \
				/zone/${GUEST_NAME}/${GUEST_CONFIG_NAME}
		elif [ ${GUEST_NAME} == "secure" ]; then
			echo -e "lxc.net.0.ipv4.address = ${LXCBR_SECURE_IPV4}" >> \
				/zone/${GUEST_NAME}/${GUEST_CONFIG_NAME}
		fi
		echo -e "lxc.net.0.ipv4.gateway = 10.0.3.1" >> \
		/zone/${GUEST_NAME}/${GUEST_CONFIG_NAME}
	else
		echo -e "${LXC_NETWORK_TYPE_PREFIX} = empty" >> \
		/zone/${GUEST_NAME}/${GUEST_CONFIG_NAME}
	fi
}

enable_host_network() {
	# TODO
	return 1	
}

# [2] For Guest (Normal/Secure Zone)
# work : veth create / wireless or usb device set
set_guest_network() {
	sleep 2
	# 1) wired && pci nic : veth setting
	# 2) wireless || usb nic : migration to guest
	if [ ${TOSNET_TYPE} != "default" ]; then
		return 1
	fi
	if [ -d ${IFACE_INFO_PATH} ]
	then
		for NIC in $(ls ${IFACE_INFO_PATH})
		do
			NIC_TYPE=`get_nic_type ${NIC}`
			SLOT_TYPE=`get_nic_slot_type ${NIC}`
			if [[ ${NIC_TYPE} == "eth" && ${SLOT_TYPE} == "pci" ]]; then
				echo -e "${GREEN}eth pci device check"
				create_veth_from_bridge ${NIC}
			elif [[ ${NIC_TYPE} == "wlan" || ${SLOT_TYPE} == "usb" ]]; then
				if [ ${GUEST_NAME} == "normal" ]; then
					lxc-device -n ${GUEST_NAME} add ${NIC} ${NIC}
				fi
			fi
		done
	fi
	return 0
}

create_veth_from_bridge() {
	local IF=$1
	local BRIDGE=${TOS_BRIDGE_PREFIX}${IF}
	test -n "$IF" || return 1
	echo -e "${GREEN}create_veth_from_bridge 시작1${END}"
	#test ${TOSNET_TYPE} != "default" || return 1
	echo -e "${GREEN}create_veth_from_bridge 시작2${END}"
	ls ${IFACE_INFO_PATH} | grep ${BRIDGE} || return 2
	echo -e "${GREEN}create_veth_from_bridge 시작3${END}"
	ip link add name ${VETH_PREFIX}${IF}${VETH_HOST_SUFFIX} type veth peer name ${VETH_PREFIX}${IF}${VETH_GUEST_SUFFIX}
	echo -e "${GREEN}bridge : ${BRIDGE} / host_veth : ${VETH_PREFIX}${IF}${VETH_HOST_SUFFIX}${END}"
	brctl addif ${BRIDGE} ${VETH_PREFIX}${IF}${VETH_HOST_SUFFIX}
	GUEST_PID=`lxc-info -pHn ${GUEST_NAME}`
	echo -e "${GREEN}guest(${GUEST_NAME}) PID : ${GUEST_PID}${END}"
	ip link set dev ${VETH_PREFIX}${IF}${VETH_GUEST_SUFFIX} netns ${GUEST_PID} name ${IF}
	return 0
}

enable_guest_network() {
	# TODO
	return 1
}

# [3] main
check_tosnet_environ
case "$1" in
	host)
		set_host_network
		create_guest_network_conf
		;;
	guest)
		if [ -z $2 ]; then
			echo -e "Invalid guest name : Please input normal(secure)." && exit 1
		else
			if [[ $2 != "normal" || $2 != "secure" ]]; then
				echo -e "Invalid guest name : Only normal/secure is available"
				exit 1
			fi
			GUEST_NAME=${2}
			set_guest_network ${GUEST_NAME}
		fi
		;;
	*)
		echo -e "Usage: $0 {host|guest} {normal|secure}" && exit 2
		;;
esac
exit $?

# [4] Test Code
echo -e "${GREEN}tosnet environ을 확인합니다 ${END}"
check_tosnet_environ
echo -e "${GREEN}host_network을 설정합니다 ${END}"
set_host_network
echo -e "${GREEN}gueust network 환경설정을 시작합니다 ${END}"
create_guest_network_conf
echo -e "${GREEN}guest_network를 설정합니다 ${END}"
set_guest_network
echo -e "${GREEN}설정이 끝났습니다. ${END}"
exit 0
