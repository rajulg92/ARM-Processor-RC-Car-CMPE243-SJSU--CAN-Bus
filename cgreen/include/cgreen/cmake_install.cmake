# Install script for directory: C:/SJSU_Dev/cgreen/include/cgreen

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cgreen" TYPE FILE FILES
    "C:/SJSU_Dev/cgreen/include/cgreen/assertions.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/boxed_double.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/breadcrumb.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/cdash_reporter.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/cgreen.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/constraint.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/constraint_syntax_helpers.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/cpp_assertions.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/cpp_constraint.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/cute_reporter.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/legacy.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/mocks.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/string_comparison.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/reporter.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/runner.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/suite.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/text_reporter.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/unit.h"
    "C:/SJSU_Dev/cgreen/include/cgreen/vector.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/SJSU_Dev/cgreen/include/cgreen/internal/cmake_install.cmake")

endif()

