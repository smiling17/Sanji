#!/bin/bash

INTERFACES_FILE="/etc/network/interfaces"
INTERFACES_FILE_BAK="/etc/network/interfaces.bak"
SET_DHCP=false
DEV_DHCP=""
REG_DHCP="iface ([^ ]+) inet dhcp"
REG_PRE_UP="pre-up"
REG_ESSID="wireless-essid"

[ -f ${INTERFACES_FILE} ] && rm $INTERFACES_FILE_BAK

#while IFS= read -r line
while read -r line
do
	if [[ $line =~ $REG_DHCP ]];
	then
		echo $line >> $INTERFACES_FILE_BAK
		SET_DHCP=true
		DEV_DHCP=${BASH_REMATCH[1]}
		continue;
	fi
	if ${SET_DHCP} && ! [[ $line =~ $REG_PRE_UP ]] && ! [[ $line =~ $REG_ESSID ]]
	then
		echo -e "\tpre-up ip link set dev ${DEV_DHCP} up" >> $INTERFACES_FILE_BAK
	fi
	SET_DHCP=false
	DEV_DHCP=""

	if [[ $line =~ "#" ]] || [[ $line =~ "auto" ]] || [[ $line =~ "iface" ]];
	then
		echo -e $line >> $INTERFACES_FILE_BAK
	else
		echo -e "\t$line" >> $INTERFACES_FILE_BAK
	fi

done < $INTERFACES_FILE

#cat ${INTERFACES_FILE_BAK}

cp $INTERFACES_FILE_BAK $INTERFACES_FILE
rm $INTERFACES_FILE_BAK

exit 0
