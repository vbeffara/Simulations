
def set_options (opt) :
    opt.tool_options ('compiler_cxx')
    opt.add_option('--gui', default='fltk', help='Build to use the specified GUI')

def configure (conf) :
    from Options import options

    conf.check_tool ('compiler_cxx')

    conf.check_cfg (package='libpng', args='--cflags --libs', uselib_store='png')
    conf.check_cfg (package='fftw3', args='--cflags --libs')

    if options.gui == 'fltk':
        conf.check_cfg (path='fltk-config', args='--cxxflags --ldflags', package='', uselib_store='fltk', msg='Checking for fltk')
    
    conf.check_cxx (header_name='boost/version.hpp', define_name='HAVE_BOOST')

    conf.write_config_header ('libvb/vb/config.h')

def build (bld) :
    bld.add_subdirs ('libvb')

# vim: ft=python
