#
# file: FreeMODBUS-sources.cmake
#
# author: Copyright (C) 2019 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
#
# This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
# distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

add_library(FreeMODBUS-integration STATIC
		${CMAKE_CURRENT_LIST_DIR}/errorCodeToFreemodbusError.cpp
		${CMAKE_CURRENT_LIST_DIR}/freemodbusErrorToErrorCode.cpp
		${CMAKE_CURRENT_LIST_DIR}/freemodbusEvents.cpp
		${CMAKE_CURRENT_LIST_DIR}/freemodbusTcp.cpp
		${CMAKE_CURRENT_LIST_DIR}/freemodbusTimers.cpp
		${CMAKE_CURRENT_LIST_DIR}/ListenSocket.cpp)
target_include_directories(FreeMODBUS-integration PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/include
		$<TARGET_PROPERTY:FreeMODBUS,INTERFACE_INCLUDE_DIRECTORIES>)
target_link_libraries(FreeMODBUS-integration PUBLIC
		lwIP::core)

target_link_libraries(FreeMODBUS PUBLIC
		FreeMODBUS-integration)
add_library(FreeMODBUS::FreeMODBUS ALIAS FreeMODBUS)
