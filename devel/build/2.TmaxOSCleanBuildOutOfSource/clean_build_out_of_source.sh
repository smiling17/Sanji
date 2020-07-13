#!/bin/bash

CYAN="\e[1;36m"
GREEN="\e[1;32m"
RED="\e[0;31m"
END="\e[0m"

HOME_DIR="$HOME"
BUILD_DEBUG_TOP="${HOME_DIR}/tos_build/binary_debug"
BUILD_DEBUG_TOC_TOP="${HOME_DIR}/toc_build/binary_debug"
BUILD_RELEASE_TOP="${HOME_DIR}/tos_build/binary_release"
BUILD_RELEASE_TOC_TOP="${HOME_DIR}/toc_build/binary_release"
TOS_TOP="${WORK_HOME}/${TOS_HOME}"
TOS_SRC="${TOS_TOP}/src"
TOS_PKG="${TOS_TOP}/pkg"
TOC="${TOS_TOP}/src/toc"
INIT_DEBUG="init_debug.sh"
INIT_RELEASE="init_release.sh"
TOS_PKG_SH="install_linux_pkg.sh"
INIT_DIR="/system /windata /rsmdata"

print_current_dir() {
  echo -e "${GREEN}current Dir : `pwd`"
}

clean_and_refresh_dir() {
  echo -e "${GREEN}make clean을 실행합니다. ${END}"
  sleep 2
  cd ${BUILD_DEBUG_TOP} && make clean-all
  echo -e "${GREEN}기존 디렉토리를 지우고 새로 생성합니다. ${END}"
  sleep 2
  init_tos_dir
}

init_tos_dir() {
	for DIR in ${INIT_DIR}
	do
		rm -rf $DIR
		mkdir ${DIR}
	done
}

make_install_toc() {
  echo -e "${GREEN}TOC 호환 디렉토리 설치전 클린작업을  시작합니다. ${END}"
  cd ${BUILD_DEBUG_TOC_TOP}
  make clean-all
  echo -e "${GREEN}TOC 호환 디렉토리 설치전 init.py를 시작합니다. ${END}"
  sleep 2
  cd $TOC/build
  ./init_debug.sh
  git submodule update
  echo -e "${GREEN}TOC 호환 디렉토리 빌드를  시작합니다. ${END}"
  sleep 2
  cd ${BUILD_DEBUG_TOC_TOP}
  make -j
  echo -e "${GREEN}TOC 호환 디렉토리 인스톨을  시작합니다. ${END}"
  make install -j
}

make_install_tos() {
  echo -e "${GREEN}PKG 설치를 시작합니다. ${END}"
  sleep 2
  cd $TOS_PKG
  ./$TOS_PKG_SH -a
  echo -e "${GREEN}SRC내 빌드를 시작합니다. ${END}"
  sleep 2
  cd $TOS_TOP # change from 18.04.17 
  cp config.cmake.eg config.cmake
  cd ./build
  ./${INIT_DEBUG}
  #cmake . -DCMAKE_BUILD_TYPE=Debug # change from 18.04.17
  cd ${BUILD_DEBUG_TOP}/src/core
  make install -j
  cd ${BUILD_DEBUG_TOP}/src/common
  make install -j
  cd ${BUILD_DEBUG_TOP}/src/lib/tgk
  make install -j
  cd ${BUILD_DEBUG_TOP}/src/lib
  make install -j
  cd ${BUILD_DEBUG_TOP}/src/gk_repo
  make install -j
  echo -e "${GREEN}최상위 인스톨을 시작합니다. ${END}"
  sleep 2
  cd ${BUILD_DEBUG_TOP}
  make install -j
}

case "$1" in
	-d)
		clean_and_refresh_dir
		;;
	-b)
		make_install_tos
		;;
	-a)
		clean_and_refresh_dir
		make_install_tos
		make_install_toc
		#make_install_toc /* 64bit X */
		;;
	-t)
		make_install_toc
		;;
	-z)
		init_tos_dir
		;;
	*)
		echo "Usase: $0 {-d(delete)|-b(tos_build)|-a(all)|-t(toc_build)}"
		exit 2
		;;
esac

exit $?
