#!/usr/bin/python

from glob import glob
from sys import argv,stderr,exit

full = "-f" in argv
files = {}

print "digraph {"
print "  rankdir = LR"

for f in glob ("libvb/vb/*.h"):
	f = f[6:]
	print '"%s"[style="filled",fillcolor="lightblue"];' % f
	files[f] = set()

if "-l" in argv:
	for f in glob ("libvb/*.cpp"):
		print '"%s"[style="filled",fillcolor="yellow"];' % f
		files[f] = set()
	for f in glob ("libvb/vb/impl/*.hxx"):
		f = f[6:]
		print '"%s"[style="filled",fillcolor="yellow"];' % f
		files[f] = set()

if "-x" in argv:
	for f in glob ("[123]*/*.cpp") + glob("tests/*.cpp"):
		print '"%s"[style="filled",fillcolor="pink"];' % f
		files[f] = set()

for f in files:
	ff = f;
	if ff[:3] == "vb/":
		ff = "libvb/" + f;
	for l in open(ff):
		if l.startswith("#include"):
			h = l[10:-2]
			if h.startswith("vb/") or full:
				print '  "%s" -> "%s"' % (f,h)
				files[f].add(h)
print "}"

def descendents (f) :
	out = files[f].copy();
	for k in files[f]:
		if k in files:
			out |= descendents(k)
	return out

bad = 0
for f in files:
	for k in files[f]:
		for l in files[f]:
			if l in files and k in descendents(l):
				print >> stderr, "Redundant include: %s -> %s (implied by %s)" % (f,k,l)
				bad = 1
exit(bad)
