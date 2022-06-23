catch2 = dependency ('catch2-with-main')

catch2_src = ['Array.cpp', 'Bitmap.cpp', 'CL_Parser.cpp', 'Color.cpp', 'Coloring.cpp',
  'Constellation0.cpp', 'Constellation1.cpp', 'Cube.cpp', 'Figure.cpp', 'HLib.cpp',
  'LinearAlgebra.cpp', 'Minimizer.cpp', 'Permutation.cpp', 'ProgressBar.cpp',
  'TriMatrix.cpp', 'coo.cpp', 'math.cpp']

catch2_bin = executable ('catch2', catch2_src, dependencies : [libvb, catch2])

test('Global test', catch2_bin)