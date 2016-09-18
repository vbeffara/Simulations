#!/usr/bin/python

import subprocess

cmd = "./build/1D/Diploid -x .1 -K 3000 -Z .1"

for i in range(1,100):
	print "s = %d :" % i,
	vcmd = cmd.split() + ["-s", str(i), "-t", str(0)]
	(o,e) = subprocess.Popen(vcmd, stderr=subprocess.PIPE).communicate()
	ee = e.split()
	print ee
	if (ee[1] != '0') and (ee[6] != '0'):
		break
