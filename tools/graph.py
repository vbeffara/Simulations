#!/usr/bin/env python3

from glob import glob
from sys import stderr
import re

nodes = {}
clusters = {}


def parent(path):
    m = re.compile("^(?P<p>.*)/[^/]*$").match(path)
    if m:
        return m['p']
    else:
        return ""


def traverse(path):
    print("  subgraph \"cluster_%s\" {" % path)
    print("    label = \"%s\"" % path)
    for f in clusters[path]:
        print('    "%s" [label=\"%s\"]' % (f, nodes[f]["label"]))
    for c in clusters:
        if parent(c) == path:
            traverse(c)
    print("  }")


for f in glob("include/vb/**/*.h", recursive=True):
    m = re.compile("include/(?P<path>.*)/(?P<basename>[^/]*).h").match(f)
    node = {"label": m['basename']+".h", "file": f, "links": []}
    if not m['path'] in clusters:
        clusters[m['path']] = []
    clusters[m['path']] += [f]

    for l in open(f, encoding="utf-8"):
        mm = re.compile("\\#include \\<(?P<link>vb/.*\\.h)\\>$").match(l)
        if mm:
            node["links"] += ["include/" + mm['link']]

    nodes[f] = node

print("digraph {")
print("  rankdir = LR")
traverse("vb")
for k, v in clusters.items():
    print("  subgraph \"cluster_%s\" {" % k)
    print('    label = "%s"' % k)
    for l in v:
        print('    "%s" [label=\"%s\"]' %
              (nodes[l]["file"], nodes[l]["label"]))
    print("  }")
for k, v in nodes.items():
    for l in v["links"]:
        print('    "%s" -> "%s"' % (v["file"], nodes[l]["file"]))
print("}")
