# SConscript for my simulations.

Import('env')

# The libvb stuff

vb = env.Clone()
vb.ParseConfig('sdl-config --cflags --libs')
vb.Append ( CPPPATH = ['#libvb'] )

SConscript ("2D/SConscript", exports="vb")
SConscript ("tests/SConscript", exports="vb")

# The rest

for i in ['1D', 'trucs', 'xtoys']:
  SConscript ("%s/SConscript" % i, exports="env")

# Install the headers

env.Install ("$prefix/include", "libvb/vb.h")
for i in env.MyGlob("libvb/vb/*.h"):
    env.Install ("$prefix/include/vb", i)
