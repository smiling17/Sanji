#!/bin/bash

CYAN="\e[1;36m"
GREEN="\e[1;32m"
RED="\e[0;31m"
END="\e[0m"

HOME_DIR="/root"
BUILD_DEBUG_TOP="${HOME_DIR}/tos_build/binary_debug"
BUILD_RELEASE_TOP="${HOME_DIR}/tos_build/binary_debug"
TOS_TOP="/root/work/trunk"
TOS_SRC="${TOS_TOP}/src"
TOS_PKG="${TOS_TOP}/pkg"
TOC="${TOS_TOP}/src/toc"
INIT_DEBUG="init_debug.sh"
INIT_RELEASE="init_release.sh"
TOS_PKG_SH="install_linux_pkg.sh"
INIT_DIR="/system /windata /rsmdata"

print_current_dir() {
  echo -e "${GREEN}Current Dir : `pwd`"
}

clean_and_refresh_dir() {
  echo -e "${GREEN}make clean을 실행합니다. ${END}"
  sleep 2
  cd ${BUILD_DEBUG_TOP} && make clean-all
  # git clean -fdx
  echo -e "${GREEN}기존 디렉토리를 지우고 새로 생성합니다. ${END}"
  sleep 5
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
  sleep 2
  cd $TOC/build/debug
  ninja clean
  echo -e "${GREEN}TOC 호환 디렉토리 설치전 init.py를 시작합니다. ${END}"
  sleep 2
  cd $TOC/build
  ./init.py
  git submodule update
  echo -e "${GREEN}TOC 호환 디렉토리 빌드를  시작합니다. ${END}"
  sleep 2
  cd $TOC/build/debug
  ninja
  echo -e "${GREEN}TOC 호환 디렉토리 인스톨을  시작합니다. ${END}"
  ninja install
}

make_install_tos() {
  echo -e "${GREEN}PKG 설치를 시작합니다. ${END}"
  sleep 2
  cd $TOS_PKG
  print_currnet_dir
  ./$TOS_PKG_SH -a
  echo -e "${GREEN}SRC내 빌드를 시작합니다. ${END}"
  sleep 2
  cd $TOS_TOP # change from 18.04.17 
  print_current_dir
  cp config.cmake.eg config.cmake
  cd ./build
  ./${INIT_DEBUG}
  print_current_dir
  #cmake . -DCMAKE_BUILD_TYPE=Debug # change from 18.04.17
  cd ${BUILD_DEBUG_TOP}
  make -j
  echo -e "${GREEN}SRC내 인스톨을 시작합니다. ${END}"
  sleep 2
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