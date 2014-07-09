# Simulations and stuff

- **Author:**        [Vincent Beffara](http://perso.ens-lyon.fr/vincent.beffara/) ([mail](mailto:vbeffara@ens-lyon.fr))
- **Documentation:** <http://perso.ens-lyon.fr/vincent.beffara/simu.html>
- **Development:**   <http://github.com/vbeffara/Simulations>

## Introduction

The directory `libvb/` contains a small (but steadily growing) library providing very easy display and PNG output for 2D simulations, plus a few convenience classes.  Up-to-date documentation on the library is mostly in my head right now but you can have a look at the web page mentioned above.  It depends on a few outside libraries:
- `Cairo` for internally drawing objects
- `FLTK`  for graphical display
- `Boost` for various graph-related algorithms and random number generation

The aim of this library is to provide an easy way for mathematicians (and maybe others) to display 2D graphics for their simulations and to produce PNG files for easy inclusion in a LaTeX document. The main design goal is that usage should be as simple as possible; as such, the library is not particularly efficient (no fancy OpenGL or even hardware acceleration) but the interface is straightforward. Among the useful features:

- vb::Image: the main class provided by the library. It centralizes most of the functionality, can be displayed on the screen by calling vb::Image::show() and transformed into an PNG file.

- Auto-update: the display routines (vb::Image::put() and others) will try to update the screen representation of the image on the screen about 10 times per second. It works best if the computation time is roughly the same for all the points displayed. This is extremely practical. This is wrapped into a class called vb::AutoWindow so that it can be re-used (by vb::Figure for instance).

- vb::Image::tessel(): a quick way to fill an image according to a coloring function (which gives the color as a function of coordinates) by a dyadic decomposition.

- A helper class vb::CL_Parser to access a simple but often sufficient subset of getopt. Supports default values for parameters and a personalized help message. I use it myself even for programs without a display.

- vb::CoarseImage: a simple way to store and display very large images that consist of large black and white zones (typically first-passage percolation clusters).

- vb::PRNG: a pseudo-random number generator (and yet another implementation of the "Mersenne twister algorithm"), very fast and much better than the standard rand(). Implements various standard distributions.

- vb::Figure: a class for building figures from a program, and exporting it as an Asymptote file. Right now it is embryonic (only dots, line segments and circles) but already functional.

- vb::Map: a class to encode planar maps, implementing various algorithms as I need them (circle packing for instance).

- vb::Minimizer: to minimize a function of potentially many variables, using one of a few available algorithms (Fletcher-Reeves, BFGS ... see the documentation for the whole list). 

## Compilation

I am using the very nice [`waf`](http://code.google.com/p/waf/) framework for my simulations, because it has a lot of cool features (such as automatic dependency tracking, and most of all portability). It is very easy to install, if you already have Python, because it is a single file; there is a copy in the root directory, which you can copy somewhere in your `$PATH` for convenience (somewhere where you have write privileges while running it), or use directly by running `./waf`.
