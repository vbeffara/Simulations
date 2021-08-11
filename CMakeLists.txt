cmake_minimum_required (VERSION 3.10)
project (Simulations CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)
set (CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT}/")

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release)
endif()

include(${PROJECT_SOURCE_DIR}/conan.cmake)
set(CONAN_SYSTEM_INCLUDES ON)
conan_cmake_run(REQUIRES
				docopt.cpp/0.6.3
				ms-gsl/3.1.0
				mpc/1.2.0
				mpfr/4.1.0
				pkgconf/1.7.4
                range-v3/0.11.0
				spdlog/1.8.5
                BASIC_SETUP NO_OUTPUT_DIRS KEEP_RPATHS BUILD missing)

add_compile_options(-fdiagnostics-color=always -pedantic -Wall -Wextra -Wshadow
                    -Wpedantic -Wold-style-cast -Wunused -Wconversion
                    -Wsign-conversion -Wnull-dereference)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("-- Debug mode")
  add_compile_options(-g)
else()
  message("-- Release mode")
  add_compile_options(-O3 -DNDEBUG)
endif()

find_program(CCACHE ccache)
if(CCACHE)
  message(STATUS "Using ccache")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG(-fcoroutines-ts COROUTINES_TS)
if (${COROUTINES_TS})
	add_compile_options(-fcoroutines-ts)
endif ()

include(FindBoost)
find_package(Boost REQUIRED COMPONENTS chrono)

include(FindPkgConfig)
pkg_search_module(CAIRO REQUIRED cairo)
pkg_search_module(CATCH2 REQUIRED catch2)
pkg_search_module(EIGEN REQUIRED eigen3)
pkg_search_module(FFTW3 REQUIRED fftw3)
pkg_search_module(FPLLL REQUIRED fplll)
pkg_search_module(PNG REQUIRED libpng)
pkg_search_module(TBB REQUIRED tbb)
pkg_search_module(YAMLCPP REQUIRED yaml-cpp)
include_directories(SYSTEM ${CAIRO_INCLUDE_DIRS} ${CATCH2_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${FPLLL_INCLUDE_DIRS} ${LIBPNG_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ${YAMLCPP_INCLUDE_DIRS})

include(FindFLTK)
if (${FLTK_FOUND})
    include_directories(SYSTEM ${FLTK_INCLUDE_DIR})
else()
    message(FATAL_ERROR "FLTK not found")
endif()

if (${COV})
	add_compile_options(-g -O0 --coverage -fprofile-arcs -ftest-coverage)
	add_link_options(--coverage -fprofile-arcs -ftest-coverage)
else ()
	add_compile_options(-O3 -DNDEBUG)
endif ()
link_directories ("/usr/local/lib")
include_directories ("${PROJECT_SOURCE_DIR}/include")

file (GLOB vb_sources CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/libvb/*.cpp)
add_library (vb SHARED ${vb_sources})
target_link_libraries (vb ${CONAN_LIBS} ${Boost_LIBRARIES} ${CAIRO_LIBRARIES} ${FFTW3_LIBRARIES} ${FLTK_LIBRARIES} ${FPLLL_LIBRARIES} ${PNG_LIBRARIES} ${TBB_LIBRARIES} ${YAMLCPP_LIBRARIES})
link_libraries(vb)

execute_process(COMMAND "git" describe --always --abbrev=40 WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE GIT_SHA1 OUTPUT_STRIP_TRAILING_WHITESPACE)
set_source_files_properties(libvb/version.cpp PROPERTIES COMPILE_FLAGS -DGIT_SHA1='"${GIT_SHA1}"')

set (DIRS tests 1D 2D 3D)
foreach (dir ${DIRS})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${dir})
	file (GLOB src CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/${dir}/*.cpp)
	foreach (f ${src})
		get_filename_component (e ${f} NAME_WE)
		add_executable (${e} ${f})
	endforeach (f)
endforeach (dir)

file (GLOB tests_sources CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/catch2/*.cpp)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable(catch2 ${tests_sources})
