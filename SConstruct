# General SConstruct file - fit for any project.

from os import environ

CacheDir('.scons_cache')

# Two "builders" (well, not quite) which keep track of built shared
# libraries, take care of the installing, and do black magic with
# install_name_tool to make everything work.

from sys import platform

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

    if platform == 'darwin':
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

    if platform == 'darwin':
      for l in env['BUILT_SHLIBS']:
        env.AddPostAction (p_i, "install_name_tool -change %s %s %s"
            % ( l[0], l[1], p_i[0].path ))

    return p

# Just remains to install them into the default environment, and we can
# use them.

from SCons.Script.SConscript import SConsEnvironment
SConsEnvironment.MyLibrary = MyLibrary
SConsEnvironment.MyProgram = MyProgram

# Base environment.

env = Environment(
    ENV = environ,
    CC = environ.get ('CC', 'gcc'),
    CXX = environ.get ('CXX', 'g++'),
    CCFLAGS = Split(environ.get ('CFLAGS', "-O2")),
    CXXFLAGS = Split(environ.get ('CXXFLAGS', "-O2")),
    LINKFLAGS = Split(environ.get ('LDFLAGS', "")),
    BUILT_SHLIBS = [],
    )

# Get the installation prefix

opts = Options()
opts.Add(PathOption("prefix", "installation prefix", "/usr/local", PathOption.PathAccept))
opts.Update(env)

Help(opts.GenerateHelpText(env))

# The install target

env.Alias ('install',"$prefix")

# Now for the specific stuff.
#
# CAREFUL ! Putting a build_dir here makes the glob("*.cpp") shortcuts
# fail whenever the build_dir already exists ...

Export('env')
SConscript("SConscript")
