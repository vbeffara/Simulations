cmake_minimum_required (VERSION 3.0)
project (Simulations)

set (CMAKE_CXX_FLAGS "-g -Wall -Wextra -Wno-deprecated-declarations -std=c++14 -fcilkplus -fdiagnostics-color=always")

if (${COV})
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 --coverage -fprofile-arcs -ftest-coverage -Dcilk_spawn= -Dcilk_sync= -Dcilk_for=for")
	set (CMAKE_EXE_LINKER_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -mtune=native")
endif ()

link_directories ("/usr/local/lib" "/opt/X11/lib")

include (TestBigEndian)
test_big_endian (BIGENDIAN)
if (${BIGENDIAN})
	add_definitions (-DBIGENDIAN)
endif (${BIGENDIAN})

find_package (PkgConfig)
find_package (Boost COMPONENTS unit_test_framework REQUIRED)
find_package (FLTK)
find_library (GMP gmp)
find_library (FPLLL fplll)
find_library (MPFR mpfr)

pkg_check_modules (EXT REQUIRED cairo fftw3 libpng yaml-cpp)

execute_process (COMMAND fltk-config --version OUTPUT_VARIABLE FLTK_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
if (FLTK_VERSION VERSION_LESS "1.3.4")
	add_definitions (-DNO_RETINA)
endif (FLTK_VERSION VERSION_LESS "1.3.4")

include_directories ("${PROJECT_SOURCE_DIR}/libvb" ${Boost_INCLUDE_DIRS} ${EXT_INCLUDE_DIRS})

file (GLOB vb_sources libvb/*.cpp)
add_library (vb SHARED ${vb_sources})
target_link_libraries (vb ${EXT_LIBRARIES} ${GMP} ${FLTK_LIBRARIES} ${FPLLL} ${MPFR} cilkrts)

foreach (dir tests 1D 2D 3D)
	file (GLOB src ${dir}/*.cpp)
	foreach (f ${src})
		set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${dir})
		get_filename_component (e ${f} NAME_WE)
		add_executable (${e} ${f})
		target_link_libraries (${e} vb)
	endforeach (f)
endforeach (dir)

target_link_libraries (tests ${Boost_LIBRARIES})
