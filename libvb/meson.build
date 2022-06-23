boost = dependency ('boost')
cairo = dependency ('cairo')
eigen = dependency ('eigen3')
fplll = dependency ('fplll')
gsl = dependency ('Microsoft.GSL', modules : ['Microsoft.GSL::GSL'])
libpng = dependency ('libpng')
mpfr = dependency ('mpfr')
range = dependency ('range-v3')
spdlog = dependency ('spdlog')
tbb = dependency ('tbb')
yamlcpp = dependency ('yaml-cpp')

fltk_config = find_program('fltk-config')
fltk_cflags = run_command(fltk_config, '--cflags', check: true).stdout().strip()
fltk_libs = run_command(fltk_config, '--ldflags', '--use-gl', check: true).stdout().strip().split(' ')
fltk = declare_dependency(compile_args: fltk_cflags, link_args: fltk_libs)

cxx = meson.get_compiler('cpp')
boost_chrono = cxx.find_library('boost_chrono')
mpc = cxx.find_library('mpc')

libvb_src = ['Auto.cpp', 'CL_Parser.cpp', 'CLP.cpp', 'CoarseImage.cpp', 'Color.cpp', 'Coloring.cpp',
  'Constellation.cpp', 'Constellation0.cpp', 'Constellation1.cpp', 'Cube.cpp', 'Elliptic.cpp',
  'Figure.cpp', 'Hub.cpp', 'Hypermap_lib.cpp', 'Hypermap.cpp', 'LinearAlgebra.cpp', 'Map.cpp',
  'Minimizer.cpp', 'misc.cpp', 'mp.cpp', 'Pairings.cpp', 'Path.cpp', 'Permutation.cpp',
  'Picture.cpp', 'Pov.cpp', 'PRNG.cpp', 'ProgressBar.cpp', 'QuadTree.cpp', 'Shape.cpp', 'Sphere.cpp',
  'Spheroidal.cpp', 'Stream.cpp', 'Toroidal.cpp' ]

libvb_dep = [boost, cairo, eigen, fltk, fplll, gsl, libpng, mpfr, range, spdlog, tbb, yamlcpp, boost_chrono, mpc]

libvb_lib = shared_library ('vb', libvb_src, dependencies : libvb_dep, include_directories : '../include')

libvb = declare_dependency (
  include_directories : '../include',
  dependencies : libvb_dep,
  link_with : libvb_lib
)