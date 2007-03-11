
from glob import glob
from os import environ

CacheDir('.scons_cache')

# Base environment.

env = Environment(
    ENV = environ,
    CC = environ['CC'],
    CXX = environ['CXX'],
    CCFLAGS = Split(environ['CFLAGS']),
    LINKFLAGS = Split(environ['LDFLAGS']),
    )

# Environment with SDL.

sdl = env.Copy()
sdl.ParseConfig('sdl-config --cflags --libs')
sdl.Append ( CPPPATH = Split('./libvb') )

# Environment with libvb.

vb = sdl.Copy()
vb.Append (
    LIBPATH = Split('.'),
    LIBS = Split('vb'),
    )

# Environment with X11.

x11 = env.Copy()
x11.Append (
    CPPPATH = Split('/usr/include/sys'),
    LIBPATH = Split('/usr/X11R6/lib'),
    LIBS = Split('X11 m'),
    )

# LibVB and the 2D simulations.

sdl.SharedLibrary ('vb', 
    Split('libvb/CL_Parser.cpp libvb/CoarseImage.cpp libvb/Image.cpp libvb/PRNG.cpp libvb/cruft.cpp libvb/display-sdl.cpp')
    )

vb.Program('libvb/sample.cpp')

for i in glob('2D/*.cpp'):
  vb.Program (i)

# The rest.

for i in glob('1D/*.c'):
  env.Program (i)

for i in glob('xtoys/*.c'):
  x11.Program (i)
