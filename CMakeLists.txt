cmake_minimum_required (VERSION 3.1)

project(protorecord)

set(PROTORECORD_MAJOR_VERSION 0)
set(PROTORECORD_MINOR_VERSION 1)
set(PROTORECORD_PATCH_VERSION 0)
set(PROTORECORD_VERSION
	${PROTORECORD_MAJOR_VERSION},${PROTORECORD_MINOR_VERSION},${PROTORECORD_PATCH_VERSION})

# compiler options
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add some modules to the CMAKE_MODULE_PATH
set(PROTORECORD_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
list(APPEND CMAKE_MODULE_PATH "${PROTORECORD_MODULE_PATH}")

# Adds come custome CMake macros
include("${CMAKE_SOURCE_DIR}/cmake/macros/ProtobufMacros.cmake")

# use cppunit for testing
find_package(cppunit)

# if Google Protobuf is installed, include it's headers
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

# enable testing if root project is us
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
	set(BUILD_TESTING "ON")
endif()
