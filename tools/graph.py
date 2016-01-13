#!/usr/bin/python

from glob import glob
from sys import argv

full = 0
if len(argv) >= 2 and argv[1] == "-f":
	full = 1

vbh = sorted (glob ("libvb/vb/*.h"))

print "digraph {"
print "  rankdir = LR"

for f in vbh:
	print '"%s"[style="filled",fillcolor="lightblue"];' % f[6:]

for f in vbh:
	for l in open(f):
		if l.startswith("#include"):
			h = l[10:-2]
			if h.startswith("vb/") or full:
				print '  "%s" -> "%s"' % (f[6:],h)

print "}"
