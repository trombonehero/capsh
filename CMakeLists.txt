cmake_minimum_required(VERSION 2.6)
project(capsh)

set(CMAKE_C_FLAGS "-Wall -Werror")
set(CMAKE_CXX_FLAGS "-Wall -Werror")

add_subdirectory(cwrap)

add_executable(capsh
		builtins.cc
		capsh.cc
		cmdproc.cc
		exception.cc
		exec.cc
		file.cc
		path.cc
	)

find_path(TECLA_INCLUDE_DIR libtecla.h)
include_directories(${TECLA_INCLUDE_DIR})

include(CheckIncludeFiles)
check_include_files(libtecla.h HAVE_TECLA_H)

find_library(TECLA_LIB tecla)
target_link_libraries(capsh ${TECLA_LIB})

include(CheckLibraryExists)
check_library_exists(tecla new_GetLine "" HAVE_TECLA_LIB)
