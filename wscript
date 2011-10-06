# -*- mode: python; -*-

def options (ctx) :
    ctx.load ('compiler_c compiler_cxx boost')
    ctx.add_option ('--gui', default='fltk', help='Build to use the specified GUI')

def configure (ctx) :
    ctx.load ('compiler_c compiler_cxx boost')
    ctx.check_boost ()
    ctx.check_cfg (package='cairomm-1.0', args='--cflags --libs', uselib_store='cairo')
    ctx.check_cfg (package='fftw3',       args='--cflags --libs', mandatory=0)
    ctx.check_cfg (package='x11',         args='--cflags --libs', mandatory=0)

    if ctx.options.gui == 'fltk':
        ctx.check_cfg (path='fltk-config', package='', args='--cflags --ldflags', uselib_store='FLTK')

    ctx.write_config_header ('libvb/vb/config.h')

def build (ctx) :
    ctx.recurse ('libvb tests 1D 2D 3D xtoys')
    ctx.install_files ('${PREFIX}/include/vb', 'libvb/vb/config.h')

def unit_test (ctx, prog, ext, checksum):
    import os,subprocess,hashlib,Logs

    Logs.pprint ('NORMAL', "Running %s :" % prog.ljust(32), sep='')

    file = "output/%s.%s" % (prog,ext)
    if os.path.exists(file): os.remove(file)
    subprocess.call ('./build/tests/' + prog)
    sum = hashlib.md5 (open(file).read()).hexdigest()

    if sum == checksum:
        Logs.pprint ('GREEN',  "ok")
    else:
        Logs.pprint ('RED',    "failed")
        Logs.pprint ('YELLOW', "  (returned %s, should be %s)" % (sum,checksum))
        exit (1)

def check (ctx):
    unit_test (ctx, 'sample',     'png', 'e900ff49de69b10d8293aa192992fb81')
    unit_test (ctx, 'test_color', 'png', '30c0f0b77ab57a9cf688616b73d0579b')
