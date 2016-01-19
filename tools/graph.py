#!/usr/bin/python

from glob import glob
from sys import argv

full = "-f" in argv

print "digraph {"
print "  rankdir = LR"

vbh = sorted (glob ("libvb/vb/*.h"))
for f in vbh:
	print '"%s"[style="filled",fillcolor="lightblue"];' % f[6:]

for f in vbh:
	for l in open(f):
		if l.startswith("#include"):
			h = l[10:-2]
			if h.startswith("vb/") or full:
				print '  "%s" -> "%s"' % (f[6:],h)

if "-x" in argv:
	vbc = sorted (glob ("[123]*/*.cpp"))
	for f in vbc:
		print '"%s"[style="filled",fillcolor="pink"];' % f
		for l in open(f):
			if l.startswith("#include"):
				h = l[10:-2]
				if h.startswith("vb/") or full:
					print '  "%s" -> "%s"' % (f,h)

if "-l" in argv:
	vbc = sorted (glob ("libvb/*.cpp"))
	for f in vbc:
		print '"%s"[style="filled",fillcolor="yellow"];' % f
		for l in open(f):
			if l.startswith("#include"):
				h = l[10:-2]
				if h.startswith("vb/") or full:
					print '  "%s" -> "%s"' % (f,h)

print "}"
