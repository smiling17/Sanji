cmake_minimum_required(VERSION 3.1)

project(toc LANGUAGES C CXX)

################################################## Compiler setting ##########################################
SET(CMAKE_C_COMPILER clang)
SET(CMAKE_CXX_COMPILER clang++)

################################################## set flags ##########################################
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -fPIC -fno-threadsafe-statics -fvisibility-inlines-hidden -Wno-tautological-compare -Wno-return-type -Wno-shift-negative-value -fno-rtti -fcolor-diagnostics -fno-standalone-debug")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fPIC -fno-threadsafe-statics -fvisibility-inlines-hidden -std=c++11 -Wno-tautological-compare -Wno-return-type -Wno-shift-negative-value -fno-rtti -fcolor-diagnostics -fno-standalone-debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

################################################## set definition ##########################################
#add_definitions(
	#  -DOS_TOS # for gfx library
  #  -D_USER32_
  #  -D_GDI32_
  #  -DTOC_DLLS
  #)

################################################## include path ##########################################

set(SYSTEM_INCLUDE     /usr/include
                       /usr/include/libnl3
)

set(FULL_INCLUDE
  ${TOC_DIR}/src/include
  ${TOC_DIR}/src
  ${SYSTEM_INCLUDE}
  ./
)

include_directories(${FULL_INCLUDE})

################################################## include libdir ##########################################
set(SYSTEM_LIBDIR     	  /lib /usr/lib /usr/lib/x86_64-linux-gnu)
set(SYSTEM_TOC_LIBDIR     /system/toc/lib)
set(TOC_LIB_DIR ${TOC_DIR}/output/lib)

link_directories(${SYSTEM_LIBDIR} ${TOC_LIB_DIR} ${SYSTEM_TOC_LIBDIR} ./) 

################################################## build files ##########################################
set(LINKLIBS c nl-route-3 nl-3)
set(TARGET main.cc netlink.cc)
add_executable(a.out ${TARGET})

target_link_libraries(a.out ${LINKLIBS})
