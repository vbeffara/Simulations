
def set_options (opt) :
    opt.tool_options ('compiler_cc compiler_cxx')
    opt.add_option ('--gui', default='fltk', help='Build to use the specified GUI')

def configure (conf) :
    conf.check_tool ('compiler_cc compiler_cxx')

    conf.check_cfg (package='cairomm-1.0', args='--cflags --libs', uselib_store='cairo', mandatory=1)
    conf.check_cfg (package='fftw3',       args='--cflags --libs', uselib_store='fftw3')
    conf.check_cfg (package='x11',         args='--cflags --libs', uselib_store='x11')

    from Options import options
    if options.gui == 'fltk': conf.check_tool ('fluid')
    
    conf.check_cxx (header_name='boost/version.hpp', define_name='HAVE_BOOST')

    conf.write_config_header ('libvb/vb/config.h')

def build (bld) :
    bld.add_subdirs ('libvb tests 1D 2D 3D trucs geom xtoys')
    bld.install_files ('${PREFIX}/include/vb', 'libvb/vb/config.h')

# vim: ft=python
