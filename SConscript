# SConscript for my simulations.

from glob import glob
Import('*')

# First, build libvb

sdl = env.Copy()
sdl.ParseConfig('sdl-config --cflags --libs')

Export ('sdl')
(libvb, libvb_i) = SConscript ("libvb/SConscript")

# Then, the 2D stuff

vb = sdl.Copy()
vb.Append ( CPPPATH = ['#libvb'], LIBS = [libvb] )

Export('vb libvb libvb_i')
SConscript ("2D/SConscript")

# The rest

Export('env')
for i in ['1D', 'trucs', 'xtoys']:
  SConscript ("%s/SConscript" % i)
