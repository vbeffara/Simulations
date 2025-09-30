cmake_minimum_required(VERSION 3.31)
project(libvb CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexperimental-library")
endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
   set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

include(FindPkgConfig)
include(FindPNG)
include(FindOpenGL)
include(FindFLTK)

find_package(spdlog REQUIRED)
find_package(Boost COMPONENTS chrono REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)
find_package(FLTK COMPONENTS gl REQUIRED)
find_package(OpenGL COMPONENTS OpenGL REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(TBB REQUIRED)
find_package(FFTW3 REQUIRED)
find_package(docopt COMPONENTS CXX REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(gmp REQUIRED)
find_package(mpfr REQUIRED)
find_package(mpc REQUIRED)
find_package(cairo REQUIRED)
find_package(Catch2 REQUIRED)
find_package(fplll REQUIRED)

if(FLTK_FOUND AND NOT TARGET fltk::fltk)
  add_library(fltk::fltk INTERFACE IMPORTED)
  set_target_properties(
    fltk::fltk
    PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${FLTK_INCLUDE_DIR}"
      INTERFACE_LINK_LIBRARIES "${FLTK_LIBRARIES}"
  )
endif()

include_directories(include ${mpc_INCLUDE_DIRS} ${FLTK_INCLUDE_DIR} ${DOCOPT_INCLUDE_DIRS})

add_subdirectory(libvb)

link_libraries(vb spdlog::spdlog docopt_s)

add_subdirectory(1D)
add_subdirectory(2D)
add_subdirectory(3D)
add_subdirectory(catch2)
add_subdirectory(tests)