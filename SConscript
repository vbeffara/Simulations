# SConscript for my simulations.

Import('env')

# Get the installation prefix and stuff.

opts = Variables('simulations.conf')
opts.Add (PathVariable ("prefix", "installation prefix", "/usr/local", PathVariable.PathAccept))
opts.Add ("GUI", "The GUI to use (fltk or none)", "fltk")
opts.Update(env)
opts.Save('simulations.conf', env)

env.Alias ('install',"$prefix")
Help(opts.GenerateHelpText(env))

# Check the configuration

conf = Configure(env, config_h='#build/libvb/vb/config.h')

if conf.CheckCHeader ("fftw3.h"):
    conf.Define ('HAVE_FFTW3')
    env.Append (LIBS = ["fftw3"])

if conf.CheckCHeader ("png.h"):
    conf.Define ('HAVE_PNG')
    env.Append (LIBS = ["png"])

if env['GUI'] == 'fltk' and conf.CheckCXXHeader ("FL/Fl.H"):
    conf.Define ('HAVE_FLTK')
    env.ParseConfig ("fltk-config --cxxflags --ldflags")

if conf.CheckCXXHeader ("boost/graph/graph_utility.hpp"):
    conf.Define ('HAVE_BOOST')

conf.Finish()

# Build libvb and the associated environment.

env.Append ( CPPPATH = ["#build/libvb"] )

libvb = SConscript ("libvb/SConscript", exports="env")

vb = env.Clone()
vb.Append ( LIBPATH = [ libvb[0].dir ], LIBS = ["vb"] )

# Now build the programs.

for i in ['tests', '1D', '2D', 'trucs', 'geom']:
  SConscript ("%s/SConscript" % i, exports="vb")

SConscript ("xtoys/SConscript", exports="env")

# Install the headers

env.Install ("$prefix/include", "#libvb/vb.h")
env.Install ("$prefix/include/vb", env.Glob("#build/libvb/vb/*.h"))
