#
# @author Tobias Weber <tweber@ill.fr>
# @date dec-2018
# @license GPLv3, see 'LICENSE' file
#
# ----------------------------------------------------------------------------
# mag-core (part of the Takin software suite)
# Copyright (C) 2018-2021  Tobias WEBER (Institut Laue-Langevin (ILL),
#                          Grenoble, France).
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ----------------------------------------------------------------------------
#

cmake_minimum_required(VERSION 3.5)
project(structfact)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-std=c++20)
add_compile_options(-Wall -Wextra)

list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/../cmake"
)

set(CMAKE_VERBOSE_MAKEFILE TRUE)


# options
option(USE_QT6 "use Qt version 6" TRUE)

if(USE_QT6)
	set(QT_VER 6)
else()
	set(QT_VER 5)
endif()


# system specific settings
message("Building for ${CMAKE_SYSTEM_NAME} systems.")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# pass linker --subsystem option
	add_compile_options(-Wl,--subsystem,windows)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	add_compile_options(-mmacosx-version-min=10.15)
endif()

find_package(Boost REQUIRED)
add_compile_options(${Boost_CXX_FLAGS})

find_package(Gemmi REQUIRED)

message("Trying to select Qt version ${QT_VER}.")
if(QT_VER EQUAL 6)
	find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets)
	include_directories("${Qt6Core_INCLUDE_DIRS}/..")
	list(APPEND QtLibraries Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGLWidgets)
elseif(QT_VER EQUAL 5)
	find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
	include_directories("${Qt5Core_INCLUDE_DIRS}/..")
	list(APPEND QtLibraries Qt5::Core Qt5::Gui Qt5::Widgets)
else()
	message(FATAL_ERROR "Tried to select a non-supported Qt version ${QT_VER}.")
endif()

set(CMAKE_AUTOUIC TRUE)
set(CMAKE_AUTOMOC TRUE)

include_directories(
	"${PROJECT_SOURCE_DIR}"
	"${Boost_INCLUDE_DIRS}" "${Boost_INCLUDE_DIRS}/.." "../.."
	gemmi::headers
)


add_executable(takin_structfact
	structfact.cpp structfact_file.cpp
	structfact_plot.cpp structfact_main.cpp
	structfact_findsg.cpp structfact_nuclei.cpp
	structfact.h
	../../tlibs2/libs/qt/gl.cpp ../../tlibs2/libs/qt/gl.h
	../../tlibs2/libs/qt/glplot.cpp ../../tlibs2/libs/qt/glplot.h)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	target_link_options(takin_structfact
		# create an __info_plist section in the binary
		PRIVATE LINKER:-sectcreate,__TEXT,__info_plist,${PROJECT_SOURCE_DIR}/../../cmake/plists/InfoStructFact.plist
	)

	add_custom_command(TARGET takin_structfact POST_BUILD
		COMMAND install_name_tool -add_rpath /usr/local/lib $<TARGET_FILE:takin_structfact> || true
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
endif()

target_link_libraries(takin_structfact
	${Boost_LIBRARIES}
	${QtLibraries}
	gemmi::gemmi_cpp
)


add_executable(takin_magstructfact
	magstructfact.cpp magstructfact_file.cpp
	magstructfact_plot.cpp magstructfact_main.cpp
	magstructfact_nuclei.cpp magstructfact.h
	../../tlibs2/libs/qt/gl.cpp ../../tlibs2/libs/qt/gl.h
	../../tlibs2/libs/qt/glplot.cpp ../../tlibs2/libs/qt/glplot.h)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	target_link_options(takin_magstructfact
		# create an __info_plist section in the binary
		PRIVATE LINKER:-sectcreate,__TEXT,__info_plist,${PROJECT_SOURCE_DIR}/../../cmake/plists/InfoMagStructFact.plist
	)

	add_custom_command(TARGET takin_magstructfact POST_BUILD
		COMMAND install_name_tool -add_rpath /usr/local/lib $<TARGET_FILE:takin_magstructfact> || true
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
endif()

target_link_libraries(takin_magstructfact
	${Boost_LIBRARIES}
	${QtLibraries}
	gemmi::gemmi_cpp
)
