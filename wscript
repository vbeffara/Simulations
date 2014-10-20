#!/usr/bin/env python

def options (ctx) :
	ctx.load ('compiler_c compiler_cxx')

def configure (ctx) :
	ctx.env.CXXFLAGS += [ '-std=c++0x', '-fext-numeric-literals' ]
	ctx.env.LINKFLAGS += [ '-std=c++0x' ]

	ctx.load ('compiler_c compiler_cxx boost')
	ctx.define ('VB_ENDIAN', ctx.check_endianness(), 0)
	ctx.check_cfg	(package='cairo',                        	args='--cflags --libs')
	ctx.check_cfg	(package='fftw3',                        	args='--cflags --libs')
	ctx.check_cfg	(package='libpng',                       	args='--cflags --libs',   	uselib_store='PNG')
	ctx.check_cfg	(package='',                             	args='--cflags --ldflags',	uselib_store='FLTK', path='fltk-config')
	ctx.check_cfg	(package='x11',                          	args='--cflags --libs',   	mandatory=0)
	ctx.check    	(header_name=["string", "png++/png.hpp"],	msg='Checking for PNG++', 	define_name='HAVE_PNGPP')
	ctx.check    	(lib='boost_system')
	ctx.check    	(lib='boost_filesystem')
	ctx.check    	(lib='quadmath')
	ctx.check_boost ()

	ctx.write_config_header ('libvb/vb/config.h')

def build (ctx) :
	ctx.recurse ('libvb tests 1D 2D 3D')
	ctx.install_files ('${PREFIX}/include/vb', 'libvb/vb/config.h')

def unit_test (ctx, prog, ext, checksum):
	import os,subprocess,hashlib
	from waflib.Logs import pprint

	pprint ('NORMAL', "Running %s :" % prog.ljust(32), sep='')

	file = "output/%s.%s" % (prog,ext)
	if os.path.exists(file): os.remove(file)
	subprocess.call ('./build/tests/' + prog)
	sum = hashlib.md5 (open(file).read()).hexdigest()

	if sum == checksum:
		pprint ('GREEN',  "ok")
	else:
		pprint ('RED',    "failed")
		pprint ('YELLOW', "  (returned %s, should be %s)" % (sum,checksum))
		exit (1)

def check (ctx):
	unit_test (ctx, 'sample',     'png', 'e900ff49de69b10d8293aa192992fb81')
	unit_test (ctx, 'test_color', 'png', '30c0f0b77ab57a9cf688616b73d0579b')
