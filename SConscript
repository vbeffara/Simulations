# SConscript for my simulations.

Import('env')

# The libvb stuff

fltk = env.Clone()
try:
    fltk.ParseConfig('fltk3-config --cxxflags --ldflags')
except OSError:
    print "FLTK2 not found, building without display support."
    fltk.Append (CXXFLAGS = ["-DVB_NO_GUI"])

fltk.Append ( CPPPATH = [Dir('#libvb')] )

libvb = SConscript ("libvb/SConscript", exports="fltk")

vb = fltk.Clone()
vb.Append ( LIBS = [libvb] )

SConscript ("2D/SConscript", exports="vb")
SConscript ("tests/SConscript", exports="vb")

# The rest

for i in ['1D', 'trucs', 'xtoys']:
  SConscript ("%s/SConscript" % i, exports="env")

# Install the headers

env.Install ("$prefix/include", "libvb/vb.h")
for i in env.MyGlob("libvb/vb/*.h"):
    env.Install ("$prefix/include/vb", i)
