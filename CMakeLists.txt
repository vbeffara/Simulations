cmake_minimum_required (VERSION 3.0)
project (Simulations CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)
set (CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT}/")

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release)
endif()

include(${PROJECT_SOURCE_DIR}/conan.cmake)
conan_add_remote(NAME vb URL https://api.bintray.com/conan/vbeffara/misc)
set(CONAN_SYSTEM_INCLUDES ON)
conan_cmake_run(REQUIRES
				boost/1.75.0
				cairo/1.17.4
				catch2/2.13.6
				docopt.cpp/0.6.3
				eigen/3.3.9
				fftw/3.3.9
				ms-gsl/3.1.0
				mpc/1.2.0
				mpfr/4.1.0
				pkgconf/1.7.3
				spdlog/1.8.5
				tbb/2020.3
				yaml-cpp/0.6.3

				fltk/1.3.5-3@vbeffara/stable
				fplll/5.3.2-1@vbeffara/stable
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

include(CheckIncludeFileCXX)
set (CMAKE_REQUIRED_INCLUDES ${CONAN_INCLUDE_DIRS_TBB}) 
set (CMAKE_REQUIRED_FLAGS "-L${CONAN_LIB_DIRS_TBB}")
set (CMAKE_REQUIRED_LIBRARIES ${CONAN_LIBS_TBB})
cmake_policy(SET CMP0075 NEW)
CHECK_INCLUDE_FILE_CXX(execution EXECUTION)
if (${EXECUTION})
	message("Using native <execution> header")
	add_compile_options(-DEXECUTION)
else()
	message("Using parallel STL from Conan")
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
target_link_libraries (vb ${CONAN_LIBS})
link_libraries(vb)

execute_process(COMMAND "git" describe --always --abbrev=40 WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE GIT_SHA1 OUTPUT_STRIP_TRAILING_WHITESPACE)
set_source_files_properties(libvb/version.cpp PROPERTIES COMPILE_FLAGS -DGIT_SHA1='"${GIT_SHA1}"')

set (DIRS tests 1D 2D 3D)
if (${PLAYGROUND})
	set (DIRS ${DIRS} playground)
endif ()
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
