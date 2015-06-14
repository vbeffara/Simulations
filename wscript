#!/usr/bin/env python

def options (ctx) :
	ctx.load ('compiler_c compiler_cxx')

def configure (ctx) :
	ctx.env.CXXFLAGS += [ '-std=c++14', '-fext-numeric-literals' ]
	ctx.env.LINKFLAGS += [ '-std=c++14' ]

	ctx.load ('compiler_c compiler_cxx boost')
	ctx.define ('VB_ENDIAN', ctx.check_endianness(), 0)
	ctx.check_cfg	(package='cairo',                        	args='--cflags --libs')
	ctx.check_cfg	(package='fftw3',                        	args='--cflags --libs')
	ctx.check_cfg	(package='libpng',                       	args='--cflags --libs',   	uselib_store='PNG')
	ctx.check_cfg	(package='',                             	args='--cflags --ldflags',	uselib_store='FLTK', path='fltk-config')
	ctx.check    	(header_name=["string", "png++/png.hpp"],	msg='Checking for PNG++', 	define_name='HAVE_PNGPP')
	ctx.check    	(lib='gmp',                              	                          	define_name='HAVE_GMP')
	ctx.check    	(lib='mpfr',                             	                          	define_name='HAVE_MPFR')
	ctx.check    	(lib='fplll',                            	                          	define_name='HAVE_FPLLL')
	ctx.check_boost ()
	ctx.check_cxx	(msg="Checking complex<mpf_float> validity", use=['GMP'], errmsg='no', mandatory=0, define_name='HAVE_COMPLEX_GMP',
	             	code="""#include <boost/multiprecision/gmp.hpp>\nint main () { exp(std::complex<boost::multiprecision::mpf_float_100>()); }""")

	ctx.write_config_header ('libvb/vb/config.h')

def build (ctx) :
	ctx.recurse ('libvb tests 1D 2D 3D')
	ctx.install_files ('${PREFIX}/include/vb', 'libvb/vb/config.h')

def unit_test (ctx, prog, ext, checksum):
	import os,subprocess,hashlib
	from waflib.Logs import pprint

	pprint ('NORMAL', "Running %s :" % prog.ljust(32), sep='')

	file = "output/%s/%s.%s" % (prog,prog,ext)
	if os.path.exists(file): os.remove(file)
	subprocess.call ('./build/tests/' + prog)
	if not os.path.exists(file):
		pprint ('RED',    "failed")
		pprint ('YELLOW', "  (%s not created)" % file)
		exit (1)

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
