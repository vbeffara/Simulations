/// @file common.h
/// Utility include file importing external resources (plus vb/config.h) 
/// needed by all the others. Exactly which ones is hidden from the 
/// documentation, but you can have a look at the source. You should never 
/// have to include this file directly.

#ifndef __VB_COMMON_H
#define __VB_COMMON_H

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <complex>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

#include <vb/config.h>

#ifdef HAVE_CLN
#define WANT_OBFUSCATING_OPERATORS
#include <cln/cln.h>
#endif

#ifdef HAVE_FLTK
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Enumerations.H>
#endif

#ifdef HAVE_PNG
#include <png.h>
#endif

#endif
