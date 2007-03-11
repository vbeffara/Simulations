# SConstruct file for everything in libvb, 1D, 2D, xtoys.

import sys
import os
from glob import glob

CacheDir('.scons_cache')

# Base environment.

env = Environment(
    ENV = os.environ,
    CC = os.environ.get ('CC', 'gcc'),
    CXX = os.environ.get ('CXX', 'g++'),
    CCFLAGS = Split(os.environ.get ('CFLAGS', "-O2")),
    LINKFLAGS = Split(os.environ.get ('LDFLAGS', "")),
    )

# Get the installation prefix - with a hack to make installation in a
# new directory possible ...

if "install" in sys.argv:
  os.makedirs(ARGUMENTS.get('prefix',"/usr/local"))

opts = Options()
opts.Add(PathOption("prefix", "installation prefix", "/usr/local"))
opts.Update(env)

Help(opts.GenerateHelpText(env))

# Environment with SDL.

sdl = env.Copy()
sdl.ParseConfig('sdl-config --cflags --libs')
sdl.Append ( CPPPATH = ['./libvb'] )

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
    Split('libvb/CL_Parser.cpp libvb/CoarseImage.cpp libvb/Image.cpp libvb/PRNG.cpp libvb/cruft.cpp libvb/display-sdl.cpp')
    )
sdl.Install("$prefix/lib",libvb)

sample = vb.Program('libvb/sample.cpp')
vb.Install ("$prefix/bin",sample)

for i in glob('2D/*.cpp'):
  j = vb.Program (i)
  vb.Install ("$prefix/bin",j)

# The rest.

for i in glob('1D/*.c'):
  j = env.Program (i)
  vb.Install ("$prefix/bin",j)

for i in glob('xtoys/*.c'):
  j = x11.Program (i)
  x11.Install ("$prefix/bin",j)

# Installation target

env.Alias ('install',"$prefix")
