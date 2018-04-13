#!/bin/bash

CYAN="\e[1;36m"
GREEN="\e[1;32m"
RED="\e[0;31m"
END="\e[0m"

TOS_SRC="/home/lsj/work/trunk/src"
TOC="/home/lsj/work/trunk/src/toc"
TOS_PKG="/home/lsj/work/trunk/pkg"
TOS_PKG_SH="install_linux_pkg.sh"

clean_and_refresh_dir() {
  echo -e "${GREEN} make clean을 실행합니다. ${END}"
  sleep 2
  cd $TOS_SRC
  make clean-all
  echo -e "${GREEN} 기존 디렉토리를 지우고 새로 생성합니다. ${END}"
  sleep 2
  rm -rf /system
  rm -rf /windata
  rm -rf /rsmdata
  # create new directory
  mkdir /system
  mkdir /windata
  mkdir /rsmdata
}

make_install_toc() {
  echo -e "${GREEN} TOC 호환 디렉토리 설치전 클린작업을  시작합니다. ${END}"
  sleep 2
  cd $TOC
  cd ./build/debug
  ninja clean
  echo -e "${GREEN} TOC 호환 디렉토리 설치전 init.py를 시작합니다. ${END}"
  sleep 2
  cd ..
  ./init.py
  echo -e "${GREEN} TOC 호환 디렉토리 빌드를  시작합니다. ${END}"
  sleep 2
  cd ./debug
  ninja
  echo -e "${GREEN} TOC 호환 디렉토리 인스톨을  시작합니다. ${END}"
  ninja install
  cd $TOS_SRC
}

make_install_tos() {
  echo -e "${GREEN} PKG 설치를 시작합니다. ${END}"
  sleep 2
  cd $TOS_PKG
  ./$TOS_PKG_SH -a
  echo -e "${GREEN} SRC내 빌드를 시작합니다. ${END}"
  sleep 2
  cd $TOS_SRC
  #cp config.cmake.eg config.cmake
  cmake .
  make -j10
  echo -e "${GREEN} SRC내 인스톨을 시작합니다. ${END}"
  sleep 2
  make install -j10
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
    ;;
  -t)
	make_install_toc
	;;
  *)
  echo "Usase: $0 {-d(delete)|-b(build)|-a(all)|-t(toc_build)}"
    exit 2
  ;;
esac

exit $?
