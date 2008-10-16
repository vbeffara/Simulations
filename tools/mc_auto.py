#!/usr/bin/env python

import sys, vb

if len(sys.argv) > 1:
    file = sys.argv[1]
else:
    file = "amc.data"

vb.mc_auto(file).loop()
