
import sys
import os
from glob import glob

CacheDir('.scons_cache')

# Base environment.

env = Environment(
    ENV = os.environ,
    CC = os.environ['CC'],
    CXX = os.environ['CXX'],
    CCFLAGS = Split(os.environ['CFLAGS']),
    LINKFLAGS = Split(os.environ['LDFLAGS']),
    )

# Hack to make installation in a new directory possible ...

if "install" in sys.argv:
  prefix = "/usr/local"
  for i in sys.argv:
    if i[:7] == "prefix=":
      prefix = i[7:]
  os.makedirs(prefix)

opts = Options()
opts.Add(PathOption("prefix", "installation prefix", "/usr/local"))
opts.Update(env)

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
