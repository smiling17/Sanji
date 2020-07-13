#!/bin/bash

# Project common
NS_1="tos"
NS_2="resource"
PROJECT_SRC_TOP="../"

# Edit these variables (case by case)
FILE_DIR="utils"
FILE_NAME="timer.h"
GUARD_NAME="TOS_RESOURCE_TIMER_H"


print_usage() (
	echo -e "Usage : $0 {FILE_DIR} {FILE_NAME} {GUARD_NAME}"
)

if [ -z $1 ] || [ -z $2 ] || [ -z $3 ]
then
	print_usage
	exit 2
else
	FILE_DIR=$1
	FILE_NAME=$2
	GUARD_NAME=$3
fi

# Auto generation
DESTINATION="${PROJECT_SRC_TOP}${FILE_DIR}"
FILE_PATH="${DESTINATION}/${FILE_NAME}"

echo -e "FILE_PATH : ${FILE_PATH} \nFILE_NAME : ${FILE_NAME}"
touch ${FILE_PATH}

echo -e "#ifndef ${GUARD_NAME}" >> ${FILE_PATH}
echo -e "#define ${GUARD_NAME}" >> ${FILE_PATH}
echo -e "namespace ${NS_1} {" >> ${FILE_PATH}
echo -e "namespace ${NS_2} {" >> ${FILE_PATH}
echo -e "\n" >> ./${FILE_PATH}
echo -e "} /* namespace ${NS_2} */" >> ${FILE_PATH}
echo -e "} /* namespace ${NS_1} */" >> ${FILE_PATH}
echo -e "#endif /* #ifndef ${GUARD_NAME} */" >> ${FILE_PATH}
