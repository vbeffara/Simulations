find_package(Catch2 3 REQUIRED)

add_executable(catch2 Array.cpp Bitmap.cpp CL_Parser.cpp Color.cpp Coloring.cpp Constellation0.cpp
  Constellation1.cpp Cube.cpp Figure.cpp HLib.cpp LinearAlgebra.cpp Minimizer.cpp Permutation.cpp
  ProgressBar.cpp TriMatrix.cpp coo.cpp math.cpp)
target_link_libraries(catch2 PRIVATE Catch2::Catch2WithMain)