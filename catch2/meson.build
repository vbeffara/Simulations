catch2 = dependency ('catch2')

cxx = meson.get_compiler('cpp')
catch2main = cxx.find_library('Catch2Main')

catch2_src = ['Array.cpp', 'Bitmap.cpp', 'CL_Parser.cpp', 'Color.cpp', 'Coloring.cpp',
  'Constellation0.cpp', 'Constellation1.cpp', 'Cube.cpp', 'Figure.cpp', 'HLib.cpp',
  'LinearAlgebra.cpp', 'Minimizer.cpp', 'Permutation.cpp', 'ProgressBar.cpp',
  'TriMatrix.cpp', 'coo.cpp', 'math.cpp']

catch2_bin = executable ('catch2', catch2_src, dependencies : [libvb, catch2, catch2main])

test('Global test', catch2_bin)