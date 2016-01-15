cmake_minimum_required (VERSION 3.0)
project (Simulations)

set (CMAKE_CXX_FLAGS "-Wall -Wno-unused-function -std=c++14 -fcilkplus -fdiagnostics-color=always -Ofast -march=native -mtune=native")

link_directories ("/usr/local/lib" "/opt/X11/lib")

include (TestBigEndian)
TEST_BIG_ENDIAN (BIGENDIAN)
if (${BIGENDIAN})
	add_definitions (-DBIGENDIAN)
endif (${BIGENDIAN})

find_package (PkgConfig)
find_package (Boost)
find_package (FLTK)
find_library (GMP gmp)
find_library (FPLLL fplll)

pkg_check_modules (EXT REQUIRED cairo fftw3 libpng)

include_directories ("${PROJECT_SOURCE_DIR}/libvb" ${Boost_INCLUDE_DIRS} ${EXT_INCLUDE_DIRS})

file (GLOB vb_sources libvb/*.cpp)
add_library (vb SHARED ${vb_sources})
target_link_libraries (vb ${EXT_LIBRARIES} ${GMP} ${FLTK_LIBRARIES} ${FPLLL} cilkrts)

foreach (dir tests 1D 2D 3D)
	file (GLOB src ${dir}/*.cpp)
	foreach (f ${src})
		set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${dir})
		get_filename_component (e ${f} NAME_WE)
		add_executable (${e} ${f})
		target_link_libraries (${e} vb)
	endforeach (f)
endforeach (dir)
