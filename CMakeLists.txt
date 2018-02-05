cmake_minimum_required (VERSION 3.0)
project (Simulations)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fdiagnostics-color=always -pedantic")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-local-typedefs -Wno-unused-function -Wno-sign-compare")
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-mismatched-tags -Wno-undefined-var-template")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-int-in-bool-context")
endif()

include (FindOpenMP)
if (${OPENMP_CXX_FOUND})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif ()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-fcoroutines-ts COROUTINES_TS)
if (${COROUTINES_TS})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts")
endif ()

if (${COV})
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 --coverage -fprofile-arcs -ftest-coverage")
	set (CMAKE_EXE_LINKER_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native")
endif ()

if (${DEBUG})
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
endif ()

link_directories ("/usr/local/lib" "/opt/X11/lib")

include (TestBigEndian)
test_big_endian (BIGENDIAN)

include(CheckIncludeFileCXX)
check_include_file_cxx (png++/png.hpp PNGPP "-include string")

find_library (GMP gmp)
find_library (MPFR mpfr)
find_package (PkgConfig)
find_package (FLTK)
pkg_check_modules (EXT REQUIRED cairo fftw3 yaml-cpp cln gsl eigen3 fplll spdlog)
pkg_check_modules (CMAES libcmaes)

include(CheckCXXSymbolExists)
check_cxx_symbol_exists (spdlog::stdout_color_mt spdlog/spdlog.h SPDLOG_COLOR_MT)

if (${CMAES_FOUND})
    link_directories (${CMAES_LIBRARY_DIRS})
    include_directories (${CMAES_INCLUDE_DIRS})
    set (CMAES 1)
endif ()

file (GLOB vb_sources ${PROJECT_SOURCE_DIR}/libvb/*.cpp)
add_library (vb SHARED ${vb_sources})
target_link_libraries (vb ${EXT_LIBRARIES} ${GMP} ${FLTK_LIBRARIES} ${MPFR} ${CMAES_LIBRARIES} ${CONAN_LIBS})

execute_process(COMMAND "git" describe --always --abbrev=40 WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE GIT_SHA1 OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file ("${PROJECT_SOURCE_DIR}/libvb/vb/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/libvb/vb/config.h")
include_directories ("${CMAKE_CURRENT_BINARY_DIR}/libvb")

include_directories ("${PROJECT_SOURCE_DIR}/libvb" ${EXT_INCLUDE_DIRS} ${CMAES_INCLUDE_DIRS})

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
