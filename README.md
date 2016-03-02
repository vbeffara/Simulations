[![Build Status](https://travis-ci.org/vbeffara/Simulations.svg?branch=master)](https://travis-ci.org/vbeffara/Simulations)
[![Coverage Status](https://coveralls.io/repos/github/vbeffara/Simulations/badge.svg?branch=master)](https://coveralls.io/github/vbeffara/Simulations?branch=master)

- **Author:**        [Vincent Beffara](http://vbeffara.perso.math.cnrs.fr/) ([mail](mailto:vbeffara@gmail.com))
- **Documentation:** <http://vbeffara.perso.math.cnrs.fr/simulations.html>
- **Development:**   <http://github.com/vbeffara/Simulations>

# Simulations and stuff

[![Join the chat at https://gitter.im/vbeffara/Simulations](https://badges.gitter.im/vbeffara/Simulations.svg)](https://gitter.im/vbeffara/Simulations?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

The directory `libvb/` contains a small (but steadily growing) library providing very easy display and PNG output for 2D simulations, plus a few convenience classes.  Up-to-date documentation on the library is mostly in my head right now but you can have a look at the web page mentioned above, and at the examples.  It depends on a few outside libraries:
- `Cairo` for internally drawing objects
- `FLTK`  for graphical display
- `Boost` for various graph-related algorithms and random number generation

The aim of this library is to provide an easy way for mathematicians (and maybe others) to display 2D graphics for their simulations and to produce PNG files for easy inclusion in a LaTeX document. The main design goal is that usage should be as simple as possible; as such, the library is not particularly efficient (no fancy OpenGL or even hardware acceleration) but the interface is straightforward. Among the useful features:

- vb::Image: the main class provided by the library. It centralizes most of the functionality, can be displayed on the screen by calling vb::Image::show() and transformed into an PNG file.

- Auto-update: the display routines (vb::Image::put() and others) will try to update the screen representation of the image on the screen about 10 times per second. It works best if the computation time is roughly the same for all the points displayed. This is extremely practical. This is wrapped into a class called vb::AutoWindow so that it can be re-used (by vb::Figure for instance).

- vb::Coloring: a quick way to fill an image according to a coloring function (which gives the color as a function of coordinates) by a dyadic decomposition.

- A helper class vb::CL_Parser to access a simple but often sufficient subset of getopt. Supports default values for parameters and a personalized help message. I use it myself even for programs without a display.

- vb::CoarseImage: a simple way to store and display very large images that consist of large black and white zones (typically first-passage percolation clusters).

- vb::PRNG: a pseudo-random number generator (and yet another implementation of the "Mersenne twister algorithm"), very fast and much better than the standard rand(). Implements various standard distributions.

- vb::Figure: a class for building figures from a program, and exporting it as an Asymptote file. Right now it is embryonic (only dots, line segments and circles) but already functional.

- vb::Map: a class to encode planar maps, implementing various algorithms as I need them (circle packing for instance).

- vb::Minimizer: to minimize a function of potentially many variables, using one of a few available algorithms (Fletcher-Reeves, BFGS ... see the documentation for the whole list). 
