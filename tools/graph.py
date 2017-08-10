#!/usr/bin/env python3

from glob import glob
from sys import argv,stderr,exit
import networkx as nx

G = nx.DiGraph()

full = "-f" in argv

for f in glob ("libvb/vb/*.h"):
	f = f[6:]
	G.add_node(f, fillcolor="lightblue")

if "-l" in argv:
	for f in glob ("libvb/*.cpp"):
		G.add_node(f, fillcolor="yellow")
	for f in glob ("libvb/vb/impl/*.hxx"):
		f = f[6:]
		G.add_node(f, fillcolor="yellow")

if "-x" in argv:
	for f in glob ("[123]*/*.cpp") + glob("tests/*.cpp") + glob("playground/*.cpp"):
		G.add_node(f, fillcolor="pink")

for f in G.nodes():
	ff = f;
	if ff[:3] == "vb/":
		ff = "libvb/" + f;
	for l in open(ff):
		if l.startswith("#include"):
			h = l[10:-2]
			if h.startswith("vb/") or full:
				if not h in G.nodes():
					G.add_node(h)
				G.add_edge(f,h)

print ("digraph {")
print ("  rankdir = LR")
for x in G.nodes():
	print ('  "%s" [style="filled", fillcolor="%s"]' % (x, G.node[x].get('fillcolor',"white")))
	for f in G.neighbors(x):
		print ('    "%s" -> "%s"' % (x,f))
print ("}")

bad = 0
for f in G.nodes():
	for k in set(G.neighbors(f)):
		for l in set(G.neighbors(f)):
			if l in G.nodes() and k in nx.descendants(G,l):
				print ("Redundant include: %s -> %s (implied by %s)" % (f,k,l), file=stderr)
				bad = 1
exit(bad)
