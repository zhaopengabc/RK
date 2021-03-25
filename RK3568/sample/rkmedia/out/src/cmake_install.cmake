# Install script for directory: /home/zhaopeng/project/RK3568/rk356x/external/rkmedia/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libeasymedia.so.1.0.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libeasymedia.so.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libeasymedia.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/libeasymedia.so.1.0.1"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/libeasymedia.so.1"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/libeasymedia.so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libeasymedia.so.1.0.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libeasymedia.so.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libeasymedia.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/easymedia" TYPE FILE FILES
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/buffer.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/codec.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/control.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/decoder.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/demuxer.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/encoder.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/filter.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/flow.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/image.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/key_string.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/link_config.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/lock.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/media_config.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/media_reflector.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/media_type.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/message.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/message_type.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/muxer.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/reflector.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/rga_filter.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/rknn_user.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/rknn_utils.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/sound.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/stream.h"
    "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/include/easymedia/utils.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/libeasymedia.pc")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/flow/cmake_install.cmake")
  include("/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/stream/cmake_install.cmake")
  include("/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/rkrga/cmake_install.cmake")
  include("/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/guard/cmake_install.cmake")
  include("/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/filter/cmake_install.cmake")
  include("/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/rknn/cmake_install.cmake")
  include("/home/zhaopeng/project/RK3568/rk356x/external/rkmedia/out/src/c_api/cmake_install.cmake")

endif()

