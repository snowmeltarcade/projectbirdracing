set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.15)
project("SnowMeltArcade.ProjectBirdRacing" VERSION 0.0.13)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CLIENT_PROJECT_NAME "${PROJECT_NAME}.Client")
set(SERVER_PROJECT_NAME "${PROJECT_NAME}.Server")
set(SHARED_PROJECT_NAME "${PROJECT_NAME}.Shared")

set(LIBRARIES_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/libraries")

set(INSTALL_DIRECTORY "${CMAKE_BINARY_DIR}/install/bin")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake/)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(IS_DEBUG true)
elseif (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	set(IS_RELEASE_WITH_DEBUG_INFO true)
endif()

if (IS_DEBUG)
	add_compile_definitions(DEBUG)
elseif (IS_RELEASE_WITH_DEBUG_INFO)
	add_compile_definitions(RELEASE_WITH_DEBUG_INFO)
else()
	add_compile_definitions(RELEASE)
endif()

if (UNIX AND NOT APPLE)
	set(LINUX TRUE)
elseif(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "iOS")
	set(IOS TRUE)
endif()

if (WIN32)
	# as we use clang on Windows, make sure this is properly defined
	# SDL needs __WIN32__
	add_definitions(-DWIN32 -D__WIN32__)

	# SDL defines its own main, so ensure we choose the correct main
	add_link_options(-Wl,-subsystem:console)
elseif (IOS)
	# for SDL
	add_definitions(-D__IPHONEOS__)
endif()

configure_file("${CMAKE_CURRENT_LIST_DIR}/version.h.in" "${CMAKE_CURRENT_LIST_DIR}/version.h")

include("${CMAKE_CURRENT_LIST_DIR}/cmake/functions.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/libraries.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/cmake/buildsettings.cmake")

include_directories("${CMAKE_CURRENT_LIST_DIR}")

add_subdirectory("shared")
add_subdirectory("client")
add_subdirectory("server")

configure_data_files()
