
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

def unit_test (prog, file, checksum):
    import os
    from subprocess import call
    from hashlib import md5
    from Utils import pprint

    pprint ('NORMAL', "Running %s :" % prog.ljust(32), sep='')

    if os.path.exists(file): os.remove(file)
    call ('./build/default/' + prog)
    sum = md5 (open(file,'rb').read()).hexdigest()

    if sum == checksum: pprint ('GREEN', "ok")
    else:
        pprint ('RED', "failed")
        pprint ('YELLOW', "  (returned %s, should be %s)" % (sum,checksum))
        exit (1)

def check (bld):
    unit_test ('tests/sample',      'Sample.png',     'e900ff49de69b10d8293aa192992fb81')
    unit_test ('tests/test_figure', 'figtest.pdf',    '03a8b8b072f37ae06dbd9bc6b8f96725')
    unit_test ('tests/test_color',  'test_color.png', '30c0f0b77ab57a9cf688616b73d0579b')

# vim: ft=python
