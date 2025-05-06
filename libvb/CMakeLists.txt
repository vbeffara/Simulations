add_library(vb SHARED
  Auto.cpp
  CLP.cpp
  CL_Parser.cpp
  CoarseImage.cpp
  Color.cpp
  Coloring.cpp
  Constellation.cpp
  Constellation0.cpp
  Constellation1.cpp
  Cube.cpp
  Elliptic.cpp
  Figure.cpp
  Hub.cpp
  Hypermap.cpp
  Hypermap_lib.cpp
  LinearAlgebra.cpp
  Map.cpp
  Minimizer.cpp
  PRNG.cpp
  Pairings.cpp
  Path.cpp
  Permutation.cpp
  Picture.cpp
  Pov.cpp
  ProgressBar.cpp
  QuadTree.cpp
  Shape.cpp
  Sphere.cpp
  Spheroidal.cpp
  Stream.cpp
  Toroidal.cpp
  misc.cpp
  mp.cpp
)

target_include_directories(vb PUBLIC ${PROJECT_SOURCE_DIR}/include
  ${fplll_INCLUDE_DIRS} ${mpfr_INCLUDE_DIRS})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET(SCK "-framework ScreenCaptureKit")
endif()

target_link_libraries(vb PUBLIC
  spdlog::spdlog Boost::chrono Eigen3::Eigen fplll PNG::PNG gmp::gmp mpfr::mpfr mpc
  cairo::cairo OpenGL::GL fltk::fltk yaml-cpp onetbb::onetbb Microsoft.GSL::GSL ${SCK})
