#!/bin/bash

CYAN="\e[1;36m"
GREEN="\e[1;32m"
ORANGE='\e[0;33m'
RED="\e[0;31m"
END="\e[0m"

usage(){
	echo -e "\n${GREEN} 이 스크립트는 SK2-1 팀을 위한 스크립트입니다. ${END}"
	echo -e "${CYAN} -i flag에  자신의 순서를 입력하세요. (build.sh -i 2)${END}"
	echo -e "${CYAN} -d flag를 주시면 TOC_LIB macro가 적용된 상태로 빌드됩니다.${END}"
	echo -e "${CYAN} -c flag를 주시면 c로 빌드합니다.${END}"
}

# 초기화작업
file_count=0
index=0
opt_i=0
opt_d=0
opt_c=0

while getopts "i:dch" opt
do
	case $opt in
		i)
			index=${OPTARG}
			opt_i=1
			;;
		d)
			opt_d=1
			;;
		c)
			opt_c=1
			;;
		h)
			usage
			exit 0
			;;
		\?)
			usage
			exit 0
	esac
done

# 명령어처리부분

if [ ${opt_i} -eq 0 ]; then
	usage
	exit 0
fi

if [ ${opt_c} -eq 1 ]; then
	build_cmd="clang -I ../"
else
	build_cmd="clang++ --std=c++11 -I ../"
fi

if [ ${opt_d} -eq 1 ]; then
	build_cmd="${build_cmd} -D TOC_LIB"
fi

file_count=`ls ./ | grep -v '.gch\|build' | wc -l`

start_n=$((34 * $(( index - 1 )) + 1))
start_file=`ls ./ | grep -v '.gch\|build' | tail -n $((${file_count}-$((${start_n}-1)))) | head -n 1`

echo -e "\nheader의 개수는 ${ORANGE}${file_count}${END}입니다."
echo -e "${start_n}번째 파일인 ${ORANGE} ${start_file} ${END} 부터 빌드를 시작합니다.\n"

sleep 3

files=`ls ./ | grep -v '.gch\|build' | tail -n $((${file_count}-$((${start_n}-1)))) | head -n 34`

for file in ${files}
do
	cmd="${build_cmd} ${file}"
	echo -e "\n"
	echo -e "${CYAN}/**************BUILD START *******************/${END}"
	echo -e "${CYAN}Build Command : ${cmd}${END}"
	echo -e "${CYAN}/****************************************/${END}"
	echo -e "\n"
	echo -e `${cmd}`
	echo -e "\n"
	echo -e "${CYAN}/************* BUILD END ***********${END}"
	echo -e "\n\n"

done


