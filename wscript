
def set_options (opt) :
    opt.tool_options ('compiler_cc compiler_cxx')
    opt.add_option ('--gui', default='fltk', help='Build to use the specified GUI')

def configure (conf) :
    conf.check_tool ('compiler_cc compiler_cxx')

    conf.check_cfg (package='libpng', args='--cflags --libs', uselib_store='png')
    conf.check_cfg (package='fftw3',  args='--cflags --libs', uselib_store='fftw3')
    conf.check_cfg (package='x11',    args='--cflags --libs', uselib_store='x11')

    from Options import options
    if options.gui == 'fltk':
        conf.check_cfg (path='fltk-config', args='--cxxflags --ldflags', package='', uselib_store='fltk', msg='Checking for fltk')
    
    conf.check_cxx (header_name='boost/version.hpp', define_name='HAVE_BOOST')

    conf.write_config_header ('libvb/vb/config.h')

def build (bld) :
    bld.add_subdirs ('libvb tests 1D 2D trucs geom xtoys')

# vim: ft=python
