
# This is a simple wrapper around waf for convenience
#
# Change the lines below (or use the command line) accordingly : PREFIX 
# is the installation prefix, GUI is either 'fltk' (try to use fltk if 
# present) or 'none' (don't even try).

PREFIX = /usr/local
GUI = fltk

all:
	python tools/waf --gui $(GUI) --prefix $(PREFIX)

clean:
	if [ -d build ]; then python tools/waf clean; fi

distclean:
	if [ -d build ]; then python tools/waf distclean; fi
