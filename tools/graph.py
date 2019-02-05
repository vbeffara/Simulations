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
        print('    "%s" [label=\"%s\", style=filled, fillcolor=red]' %
              (f, nodes[f]["label"]))
    for c in clusters:
        if parent(c) == path:
            traverse(c)
    print("  }")


for f in glob("include/vb/**/*.h", recursive=True):
    m = re.compile("include/(?P<path>.*)/(?P<basename>[^/]*).h").match(f)
    node = {"label": m['basename']+".h", "file": f, "links": [],
            "name": m['path'] + "/" + m['basename'] + ".h"}
    if not m['path'] in clusters:
        clusters[m['path']] = []
    clusters[m['path']] += [node['name']]

    for l in open(f, encoding="utf-8"):
        mm = re.compile("\\#include \\<(?P<link>.*)\\>$").match(l)
        if mm:
            node["links"] += [mm['link']]

    nodes[node['name']] = node

print("digraph {")
print("  rankdir = LR")
traverse("vb")
for k, v in nodes.items():
    for l in v["links"]:
        print('    "%s" -> "%s"' % (v["name"], l))
print("}")
