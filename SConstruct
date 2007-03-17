# General SConstruct file - fit for any project.

from os import environ

CacheDir('.scons_cache')

# Base environment.

env = Environment(
    ENV = environ,
    CC = environ.get ('CC', 'gcc'),
    CXX = environ.get ('CXX', 'g++'),
    CCFLAGS = Split(environ.get ('CFLAGS', "-O2")),
    CXXFLAGS = Split(environ.get ('CXXFLAGS', "-O2")),
    LINKFLAGS = Split(environ.get ('LDFLAGS', "")),
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
