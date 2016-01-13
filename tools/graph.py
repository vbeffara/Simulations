#!/usr/bin/python

from glob import glob

vbh = sorted (glob ("libvb/vb/*.h"))

print "digraph {"
print "  rankdir = LR"

for f in vbh:
	print '"%s"[style="filled",fillcolor="lightblue"];' % f[6:]

for f in vbh:
	for l in open(f):
		if l.startswith("#include"):
			h = l[10:-2]
			if h.startswith("vb/"):
				print '  "%s" -> "%s"' % (f[6:],h)

print "}"
