# General SConstruct file - fit for any project.

import os
import sys

CacheDir('.scons_cache')

# Two "builders" (well, not quite) which keep track of built shared
# libraries, take care of the installing, and do black magic with
# install_name_tool to make everything work.
#
# TODO : add the equivalent on Solaris etc via RPATH. Command-line that
# works on Solaris :
#
# g++ -o Voter -Wl,-R,/home/vbeffara/Boulot/Simulations/libvb Voter.o
# -L../libvb -lvb
#
# (no need to use -lSDL ???)

class _Null:
    pass
_null = _Null

def MyLibrary (env, target=None, source=_null, **kw):
    if source is _null:
        source = target
        target = None

    l   = env.SharedLibrary (target, source, **kw)
    l_i = env.Install ("$prefix/lib", l, **kw)

    env.Append ( BUILT_SHLIBS = [[ l[0].abspath, l_i[0].abspath ]] )

    if sys.platform == 'darwin':
      env.AddPostAction (l, "install_name_tool -id %s %s"
          % (l[0].abspath, l[0].path))
      env.AddPostAction (l_i, "install_name_tool -id %s %s"
          % (l_i[0].abspath, l_i[0].path))

    return l

def MyProgram (env, target=None, source=_null, **kw):
    if source is _null:
        source = target
        target = None

    p   = env.Program (target,source,**kw)
    p_i = env.Install ("$prefix/bin", p,**kw)

    if sys.platform == 'darwin':
      try:
        for l in env['BUILT_SHLIBS']:
          env.AddPostAction (p_i, "install_name_tool -change %s %s %s"
              % ( l[0], l[1], p_i[0].path ))
      except KeyError:
        pass

    return p

# Just remains to install them into the default environment, and we can
# use them.

from SCons.Script.SConscript import SConsEnvironment
SConsEnvironment.MyLibrary = MyLibrary
SConsEnvironment.MyProgram = MyProgram

# Base environment.

env = Environment(
    ENV = os.environ,
    CC = os.environ.get ('CC', 'gcc'),
    CXX = os.environ.get ('CXX', 'g++'),
    CCFLAGS = Split(os.environ.get ('CFLAGS', "-O2")),
    CXXFLAGS = Split(os.environ.get ('CXXFLAGS', "-O2")),
    LINKFLAGS = Split(os.environ.get ('LDFLAGS', "")),
    )

env.Decider ('MD5-timestamp')

# Get the installation prefix and stuff.

opts = Options('simulations.conf')
opts.Add(PathOption("prefix", "installation prefix", "/usr/local", PathOption.PathAccept))
opts.Add('GUI','The GUI to use (fltk or none)','fltk')
opts.Update(env)
opts.Save('simulations.conf', env)

Help(opts.GenerateHelpText(env))

# The install target

env.Alias ('install',"$prefix")

# Now for the specific stuff.

SConscript("SConscript", build_dir='build', exports='env', duplicate=0)
