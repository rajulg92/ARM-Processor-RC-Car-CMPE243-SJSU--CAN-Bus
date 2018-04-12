# Install script for directory: C:/SJSU_Dev/cgreen/include/cgreen/internal

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/cgreen")
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

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cgreen/internal" TYPE FILE FILES
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/unit_implementation.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/mock_table.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/mocks_internal.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/suite_internal.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/assertions_internal.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/c_assertions.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/cpp_assertions.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/cgreen_pipe.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/cgreen_time.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/runner_platform.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/internal/function_macro.h"
    )
endif()

