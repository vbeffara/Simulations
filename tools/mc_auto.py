#!/usr/bin/env python

import vb

def f (v, x):
    return v[0] + v[1] * (x[0]**(-v[2])) + v[3] * (x[0]**(-v[2]-1.0))

F = vb.mc_auto (f, [.7,.3,.9,.1])
F.loop()
