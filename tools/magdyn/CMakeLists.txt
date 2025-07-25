#
# @author Tobias Weber <tweber@ill.fr>
# @date jan-2022
# @license GPLv3, see 'LICENSE' file
#
# ----------------------------------------------------------------------------
# mag-core (part of the Takin software suite)
# Copyright (C) 2018-2025  Tobias WEBER (Institut Laue-Langevin (ILL),
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
project(magpie)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-std=c++20)
add_compile_options(-Wall -Wextra -Weffc++)


# -----------------------------------------------------------------------------
#options
# -----------------------------------------------------------------------------
option(USE_QT      "use Qt"            TRUE)
option(USE_QT6     "use Qt version 6"  TRUE)
option(USE_HDF     "use HDF"           TRUE)
option(USE_MINUIT  "use Minuit"        TRUE)

if(USE_QT6)
	set(QT_VER 6)
else()
	set(QT_VER 5)
endif()
# -----------------------------------------------------------------------------


list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/../../cmake"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_options(-g -ggdb)
	set(CMAKE_VERBOSE_MAKEFILE TRUE)

	message("Debug building for ${CMAKE_SYSTEM_NAME} systems.")

elseif(CMAKE_BUILD_TYPE STREQUAL "Profile")
	add_compile_options(-g -ggdb3 -pg -O0)
	set(CMAKE_VERBOSE_MAKEFILE TRUE)

	message("Profile building for ${CMAKE_SYSTEM_NAME} systems.")

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_compile_options("-DNDEBUG")
	add_compile_options("-Wno-#pragma-messages")

	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		# see: https://gcc.gnu.org/onlinedocs/gcc/Developer-Options.html
		add_compile_options(-fcompare-debug-second)
	endif()

	message("Release building for ${CMAKE_SYSTEM_NAME} systems.")
endif()


set(MINGW_EXTRALIBS)
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# pass linker --subsystem option
	add_compile_options(-Wl,--subsystem,windows)
	set(MINGW_EXTRALIBS "ws2_32")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	add_compile_options(-mmacosx-version-min=10.15)
endif()


# -----------------------------------------------------------------------------
# packages
# -----------------------------------------------------------------------------
find_package(Threads REQUIRED)

find_package(Boost REQUIRED COMPONENTS program_options)
set(Boost_USE_MULTITHREADED TRUE)
add_definitions(${Boost_CXX_FLAGS})

find_package(Gemmi REQUIRED)

find_package(GFortran)
find_package(Lapacke REQUIRED)
add_definitions(-DUSE_LAPACK=1)
include_directories("${Lapacke_INCLUDE_DIRS}")

find_package(Qhull REQUIRED)
if(Qhull_FOUND)
	add_definitions(-DUSE_QHULL)
	include_directories("${Qhull_INCLUDE_DIRS}")
endif()

if(USE_MINUIT)
	find_package(Minuit2)
endif()

if(Minuit2_FOUND)
	message("Using Minuit 2.")
	include_directories("${Minuit2_INCLUDE_DIRS}")
	add_definitions(-D__TLIBS2_MAGDYN_USE_MINUIT__)
else()
	message("Disabling Minuit support.")
	set(Minuit2_LIBRARIES "")
endif()

if(USE_HDF)
	find_package(HDF5 COMPONENTS CXX)
endif()
if(HDF5_FOUND)
	message("Using HDF5 version ${HDF5_VERSION}.")
	include_directories("${HDF5_CXX_INCLUDE_DIRS}")
	add_definitions(-DUSE_HDF5)
else()
	message("Disabling HDF5 file support.")
	set(HDF5_CXX_LIBRARIES "")
endif()
# -----------------------------------------------------------------------------


include_directories("${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/../..")

include_directories(
	SYSTEM "${Boost_INCLUDE_DIRS}" "${Boost_INCLUDE_DIRS}/.." "../.."
	"${PROJECT_SOURCE_DIR}/ext" "${PROJECT_SOURCE_DIR}/../../ext/qcp"
	gemmi::headers
)


if(USE_QT)
	message("Trying to select Qt version ${QT_VER}.")

	if(QT_VER EQUAL 6)
		find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets PrintSupport Svg)
		include_directories("${Qt6Core_INCLUDE_DIRS}/..")
		list(APPEND QtLibraries Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGLWidgets Qt6::PrintSupport Qt6::Svg)
	elseif(QT_VER EQUAL 5)
		find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Svg)
		include_directories("${Qt5Core_INCLUDE_DIRS}/..")
		list(APPEND QtLibraries Qt5::Core Qt5::Gui Qt5::Widgets Qt5::PrintSupport Qt5::Svg)
	else()
		message(FATAL_ERROR "Tried to select a non-supported Qt version ${QT_VER}.")
	endif()

	set(CMAKE_AUTOUIC TRUE)
	set(CMAKE_AUTOMOC TRUE)

	find_package(QCP)
	include_directories("${QCP_INCLUDE_DIRS}")

	add_executable(magpie
		# main program
		main.cpp main_gui.cpp
		magdyn.cpp magdyn.h
		dialogs.cpp panels.cpp
		structure.cpp tables.cpp
		config.cpp file.cpp
		export.cpp export_sqe.cpp
		kernel.cpp dispersion.cpp
		graph.cpp graph.h
		defs.cpp defs.h
		gui_defs.h

		# bz calculator
		../bz/bz_lib.h ../bz/plot_cut.h
		../bz/plot.cpp ../bz/plot.h

		# dialogs
		dialogs/struct_plot.cpp dialogs/struct_plot.h
		dialogs/ground_state.cpp dialogs/ground_state.h
		dialogs/topology.cpp dialogs/topology.h
		dialogs/dispersion3d.cpp dialogs/dispersion3d.h
		dialogs/table_import.cpp dialogs/table_import.h
		dialogs/trafos.cpp dialogs/trafos.h
		dialogs/pol.cpp dialogs/pol.h
		dialogs/notes.cpp dialogs/notes.h
		dialogs/infos.cpp dialogs/infos.h
		dialogs/settings.h

		# library
		../../tlibs2/libs/magdyn.h
		../../tlibs2/libs/qt/gl.cpp ../../tlibs2/libs/qt/gl.h
		../../tlibs2/libs/qt/glplot.cpp ../../tlibs2/libs/qt/glplot.h
		../../tlibs2/libs/qt/recent.cpp ../../tlibs2/libs/qt/recent.h

#		../../ext/qcp/qcustomplot.cpp
	)

	target_link_libraries(magpie
		Threads::Threads
		${QCP_LIBRARIES}
		${QtLibraries}
		${Boost_LIBRARIES}
		${Lapacke_LIBRARIES} ${Lapack_LIBRARIES} ${BLAS_LIBRARIES} ${GFortran_LIBRARIES}
		${HDF5_CXX_LIBRARIES}
		${Minuit2_LIBRARIES}
		${MINGW_EXTRALIBS}
		${Qhull_LIBRARIES}
		gemmi::gemmi_cpp

#		/usr/local/lib/liblapack.a /usr/local/lib/libblas.a libgfortran.so    # for static lapack(e)
	)
else()
	message("Compiling without Qt.")
	add_definitions(-DDONT_USE_QT)

	add_executable(magpie
		main.cpp
		defs.cpp defs.h
		../../tlibs2/libs/magdyn.h
	)

	target_link_libraries(magpie
		Threads::Threads
		${Boost_LIBRARIES}
		${Lapacke_LIBRARIES} ${Lapack_LIBRARIES} ${BLAS_LIBRARIES} ${GFortran_LIBRARIES}
		${HDF5_CXX_LIBRARIES}
		${Minuit2_LIBRARIES}
		${MINGW_EXTRALIBS}
		gemmi::gemmi_cpp
	)
endif()


# -----------------------------------------------------------------------------
# rpath
# -----------------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	target_link_options(magpie
		# create an __info_plist section in the binary
		PRIVATE LINKER:-sectcreate,__TEXT,__info_plist,${PROJECT_SOURCE_DIR}/../../cmake/plists/InfoMagDyn.plist
	)

	add_custom_command(TARGET magpie POST_BUILD
		COMMAND install_name_tool -add_rpath /usr/local/lib $<TARGET_FILE:magpie> || true
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)
endif()
# -----------------------------------------------------------------------------
