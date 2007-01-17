
nodist_nobase_include_HEADERS = vb/config.h
nobase_include_HEADERS = vb.h vb/Image.h vb/CoarseImage.h vb/CL_Parser.h vb/PRNG.h

lib_LTLIBRARIES = libvb.la
libvb_la_SOURCES = Image.cpp CoarseImage.cpp PRNG.cpp CL_Parser.cpp \
		   cruft.cpp

AM_CFLAGS =
AM_LDFLAGS = $(XCCLINKER)
AM_CXXFLAGS =

if DISPLAY_SDL

libvb_la_SOURCES += display-sdl.cpp
AM_CXXFLAGS += @SDL_CFLAGS@
AM_LDFLAGS += @SDL_LIBS@

else
if !DISPLAY_NOX

libvb_la_SOURCES += display-x11.cpp
AM_CFLAGS += @X_CFLAGS@
AM_LDFLAGS += @X_LIBS@ -lX11

else

libvb_la_SOURCES += display-dummy.cpp

endif
endif

noinst_PROGRAMS = sample
sample_SOURCES = sample.cpp
sample_LDADD = libvb.la

bin_SCRIPTS = libvb-config
EXTRA_DIST = libvb-config.in vb/config.h.in
