# SConscript for my simulations.

Import('env')

# The libvb stuff

fltk = env.Clone()

if fltk['GUI'] == 'fltk':
    try:
        fltk.ParseConfig('fltk2-config --cxxflags --ldflags')
        fltk.Append (CXXFLAGS = ["-DLIBVB_FLTK"])
    except OSError:
        print "FLTK2 not found, building without display support."
        fltk['GUI'] = None

fltk.Append ( CPPPATH = ["#libvb"] )

libvb = SConscript ("libvb/SConscript", exports="fltk")

vb = fltk.Clone()
vb.Append ( LIBPATH = [ libvb[0].dir ], LIBS = ["vb"] )

SConscript ("2D/SConscript", exports="vb")
SConscript ("tests/SConscript", exports="vb")

# The rest

for i in ['1D', 'trucs', 'xtoys']:
  SConscript ("%s/SConscript" % i, exports="env")

# Install the headers

env.Install ("$prefix/include", "libvb/vb.h")
for i in env.MyGlob("libvb/vb/*.h"):
    env.Install ("$prefix/include/vb", i)
