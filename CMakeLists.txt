cmake_minimum_required(VERSION 4.0)
project(libvb)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexperimental-library")
# set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10")

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
find_package(fftw3 REQUIRED)
find_package(docopt REQUIRED)

find_library(mpc mpc REQUIRED)

pkg_search_module(cairo REQUIRED cairo)
pkg_search_module(fplll REQUIRED fplll)
pkg_search_module(gmp REQUIRED gmp)
pkg_search_module(mpfr REQUIRED mpfr)

include_directories(include)

add_subdirectory(libvb)

link_libraries(vb spdlog::spdlog docopt)

add_subdirectory(1D)
add_subdirectory(2D)
add_subdirectory(3D)
add_subdirectory(catch2)
add_subdirectory(tests)