# SConscript for my simulations.

Import('*')

# The libvb stuff

vb = env.Clone()
vb.ParseConfig('sdl-config --cflags --libs')
vb.Append ( CPPPATH = ['#libvb'] )

Export('vb')
SConscript ("2D/SConscript")

# The rest

Export('env')
for i in ['1D', 'trucs', 'xtoys']:
  SConscript ("%s/SConscript" % i)
