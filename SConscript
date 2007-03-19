# SConscript for my simulations.

Import('*')

# First, build libvb

sdl = env.Clone()
sdl.ParseConfig('sdl-config --cflags --libs')

Export ('sdl')
SConscript ("libvb/SConscript")

# Then, the 2D stuff

vb = sdl.Clone()
vb.Append ( CPPPATH = ['#libvb'], LIBPATH = ['#libvb'], LIBS = ['vb'] )

Export('vb')
SConscript ("2D/SConscript")

# The rest

Export('env')
for i in ['1D', 'trucs', 'xtoys']:
  SConscript ("%s/SConscript" % i)
