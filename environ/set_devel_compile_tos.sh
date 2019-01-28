#!/bin/bash

update-alternatives --set cc /usr/bin/clang
update-alternatives --set c++ /usr/bin/clang++

ln -s /usr/bin/gmake /usr/bin/make

apt update
apt install cmake
