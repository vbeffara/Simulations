#!/bin/sh -x

ACDIRS=""
test -d ${HOME}/local/share/aclocal && ACDIRS="$ACDIRS -I ${HOME}/local/share/aclocal"

aclocal $ACDIRS
libtoolize
automake -a
autoconf
