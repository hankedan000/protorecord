cmake_minimum_required (VERSION 3.1)

project(protorecord)

set(PROTORECORD_VERSION_MAJOR 0)
set(PROTORECORD_VERSION_MINOR 1)
set(PROTORECORD_VERSION_PATCH 0)
set(PROTORECORD_VERSION
	${PROTORECORD_VERSION_MAJOR},${PROTORECORD_VERSION_MINOR},${PROTORECORD_VERSION_PATCH})

# compiler options
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add some modules to the CMAKE_MODULE_PATH
set(PROTORECORD_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
list(APPEND CMAKE_MODULE_PATH "${PROTORECORD_MODULE_PATH}")

# Adds come custome CMake macros
include("${CMAKE_SOURCE_DIR}/cmake/macros/ProtobufMacros.cmake")

find_package(cppunit)# use cppunit for testing
find_package (Python COMPONENTS Interpreter)

# if Google Protobuf is installed, include it's headers
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

# enable testing if root project is us
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(CTest)
	set(BUILD_TESTING "ON")
endif()

# update all submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
	option(GIT_SUBMODULE "Check submodules during build" ON)
	if(GIT_SUBMODULE)
		message(STATUS "Submodule update")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
						RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif()
	endif()
endif()

# protorecord public includes directory
set(PROTORECORD_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/include")

add_subdirectory(include)
add_subdirectory(extern)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(demo)

# install main header file
install(FILES "${PROTORECORD_INCLUDE_DIR}/protorecord.h"
	DESTINATION "include")

# install the exported targets file
install(
	EXPORT protorecord-targets
	DESTINATION "lib/cmake/protorecord"
)
