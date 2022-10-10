# TODO: code coverage

project ('simulations', 'cpp', default_options : ['cpp_std=c++20'])

if meson.get_compiler('cpp').get_id() == 'clang'
  add_global_arguments('-fexperimental-library', language : 'cpp')
endif

fftw3 = dependency ('fftw3')
docopt = dependency ('docopt')

subdir ('libvb')
subdir ('catch2')
subdir ('tests')
subdir ('1D')
subdir ('2D')
subdir ('3D')