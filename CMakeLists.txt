cmake_minimum_required (VERSION 3.0)
project (Simulations CXX)

set(CONAN_SYSTEM_INCLUDES ON)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)

include (FindOpenMP)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -fdiagnostics-color=always -pedantic ${OpenMP_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-fcoroutines-ts COROUTINES_TS)
if (${COROUTINES_TS})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts")
endif ()

if (${COV})
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 --coverage -fprofile-arcs -ftest-coverage")
	set (CMAKE_EXE_LINKER_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()

if (${DEBUG})
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
endif ()

link_directories ("/usr/local/lib" "/opt/X11/lib")

file (GLOB vb_sources ${PROJECT_SOURCE_DIR}/libvb/*.cpp)
add_library (vb SHARED ${vb_sources})
target_link_libraries (vb ${CONAN_LIBS})

execute_process(COMMAND "git" describe --always --abbrev=40 WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE GIT_SHA1 OUTPUT_STRIP_TRAILING_WHITESPACE)
set_source_files_properties(libvb/Hub.cpp PROPERTIES COMPILE_FLAGS -DGIT_SHA1='"${GIT_SHA1}"')

include_directories ("${PROJECT_SOURCE_DIR}/libvb")

set (DIRS tests 1D 2D 3D)
if (${PLAYGROUND})
	set (DIRS ${DIRS} playground)
endif ()
foreach (dir ${DIRS})
	file (GLOB src ${PROJECT_SOURCE_DIR}/${dir}/*.cpp)
	foreach (f ${src})
		set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${dir})
		get_filename_component (e ${f} NAME_WE)
		add_executable (${e} ${f})
		target_link_libraries (${e} vb)
	endforeach (f)
endforeach (dir)
