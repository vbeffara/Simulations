
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
    bld.add_subdirs ('libvb tests 1D 2D 3D xtoys')
    bld.install_files ('${PREFIX}/include/vb', 'libvb/vb/config.h')

def check (bld):
    from os import system;

    system ('./build/default/tests/sample')
    system ('cat sample.png | md5')
    print 'e900ff49de69b10d8293aa192992fb81'

    system ('./build/default/tests/test_figure')
    system ('cat figtest.png | md5')
    print '7086cede99730506c23719283441ba90'

    system ('./build/default/tests/test_color')
    system ('cat test_color.png | md5')
    print '30c0f0b77ab57a9cf688616b73d0579b'

# vim: ft=python
