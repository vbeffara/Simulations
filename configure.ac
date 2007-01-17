
AC_INIT(simulations, 2006-12-27)
AC_CONFIG_SRCDIR(libvb/Image.cpp)
AC_CONFIG_HEADERS(libvb/vb/config.h)
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

AC_CHECK_LIB(m,exp)
AC_PATH_XTRA

AC_ARG_WITH([sdl],
            [AS_HELP_STRING([--without-sdl],
                            [disable SDL usage])],
            [],
	    [with_sdl=yes])

AS_IF([test "x$with_sdl" != xno],
      [AM_PATH_SDL(1.2.0,
                   AC_DEFINE(HAVE_SDL,1,[Do we have SDL?]),
                   with_sdl=no)])

AM_CONDITIONAL(DISPLAY_SDL, test "x$with_sdl" = xyes)
AM_CONDITIONAL(DISPLAY_NOX, test "x$no_x" = xyes)

AS_IF([test "x$with_sdl" = xyes], 
         [AC_SUBST(VB_CFLAGS, $SDL_CFLAGS)
          AC_SUBST(VB_LIBS, $SDL_LIBS)],
      [test "x$no_x" != xyes],
         [AC_SUBST(VB_CFLAGS, $X_CFLAGS)
          AC_SUBST(VB_LIBS, "$X_LIBS -lX11")],
      [AC_SUBST(VB_CFLAGS, "")
       AC_SUBST(VB_LIBS, "")])

AC_OUTPUT([Makefile libvb/libvb-config \
		    trucs/Makefile xtoys/Makefile \
		    windows/Makefile 1D/Makefile libvb/Makefile \
		    2D/Makefile])
