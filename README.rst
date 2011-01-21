=====================
Simulations and stuff
=====================

:Author:        `Vincent Beffara`_
:Documentation:  http://vbeffara.github.com/Simulations/
:Development:    http://github.com/vbeffara/Simulations

Compilation
-----------

I am using the very nice waf_ framework for my simulations, because it 
has a lot of cool features (such as automatic dependency tracking, and 
most of all portability). It is very easy to install, if you already 
have Python, because it is a single file; there is a copy in the root 
directory, which you can copy somewhere in your ``$PATH`` for 
convenience (somewhere where you have write privileges while running 
it), or use directly as ``./waf``.

Some notes about the ``libvb`` library
--------------------------------------

The directory ``libvb/`` contains a small (but steadily growing) library 
providing very easy display and PNG output for 2D simulations, plus a 
few convenience classes.  Up-to-date documentation on the library is at 
the above-mentioned web page.  It depends on a few outside libraries:

* ``Cairo`` for internally drawing objects (through the ``CairoMM`` wrapper)
* ``FLTK`` for graphical display

About the ``xtoys`` directory
-----------------------------

These programs are not mine at all, they were written by `Michael 
Creutz`_. I include them because they are somehow related to the others, 
and because I had to rewrite the Makefile to compile them on Mac OS X, 
now they should compile everywhere (as long as the X11 include files are 
present, of course).  If there is any (legal or other) problem about 
their presence here, I will remove them at once.

.. _Vincent Beffara: mailto:vbeffara@ens-lyon.fr
.. _Michael Creutz:  http://thy.phy.bnl.gov/www/xtoys/xtoys.html
.. _waf:             http://code.google.com/p/waf/
