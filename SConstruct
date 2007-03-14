# SConstruct file for everything in libvb, 1D, 2D, xtoys.

from os import environ
from glob import glob

CacheDir('.scons_cache')

# Base environment.

env = Environment(
    ENV = environ,
    CC = environ.get ('CC', 'gcc'),
    CXX = environ.get ('CXX', 'g++'),
    CCFLAGS = Split(environ.get ('CFLAGS', "-O2")),
    LINKFLAGS = Split(environ.get ('LDFLAGS', "")),
    )

# Get the installation prefix

opts = Options()
opts.Add(PathOption("prefix", "installation prefix", "/usr/local", PathOption.PathAccept))
opts.Update(env)

Help(opts.GenerateHelpText(env))

# Environment with SDL.

sdl = env.Copy()
sdl.ParseConfig('sdl-config --cflags --libs')
sdl.Append ( CPPPATH = ['libvb'] )

# Environment with libvb.

vb = sdl.Copy()
vb.Append ( LIBPATH = ['.'], LIBS = ['vb'] )

# Environment with X11.

x11 = env.Copy()
x11.Append (
    CPPPATH = ['/usr/include/sys'],
    LIBPATH = ['/usr/X11R6/lib'],
    LIBS = ['X11','m'],
    )

# LibVB and the 2D simulations.

libvb = sdl.SharedLibrary ('vb',
    Split('libvb/CL_Parser.cpp libvb/CoarseImage.cpp libvb/Image.cpp libvb/PRNG.cpp libvb/cruft.cpp')
    )
sdl.Install("$prefix/lib",libvb)

vb.Install ("$prefix/bin",vb.Program('libvb/sample.cpp'))

for i in glob('2D/*.cpp'):
  vb.Install ("$prefix/bin",vb.Program (i))

# The rest.

for i in glob('1D/*.c'):
  env.Install ("$prefix/bin",env.Program (i))

for i in glob('xtoys/*.c'):
  x11.Install ("$prefix/bin",x11.Program (i))

# Installation target

env.Alias ('install',"$prefix")
