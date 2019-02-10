#!/usr/bin/env python3

import os
import re
import glob
import sys

nodes = {}
clusters = {}


def traverse(path):
    print("  subgraph \"cluster_%s\" {" % path)
    print("    label = \"%s\"" % path)
    for f in clusters[path]:
        print('    "%s" [label=\"%s\", style=filled, fillcolor=red]' %
              (f, nodes[f]["label"]))
    for c in clusters:
        if os.path.dirname(c) == path:
            traverse(c)
    print("  }")


def explore(id):
    if nodes[id]["deps"] == []:
        for i in nodes[id]["links"]:
            if i in nodes:
                nodes[id]["deps"] += nodes[i]["links"]
                nodes[id]["deps"] += explore(i)
    return nodes[id]["deps"]


for f in glob.glob("include/vb/**/*.h", recursive=True):
    name, (path, filename) = f[8:], os.path.split(f[8:])
    if not path in clusters:
        clusters[path] = []
    clusters[path] += [name]

    node = {"label": filename, "links": [], "deps": []}
    for l in open(f, encoding="utf-8"):
        mm = re.compile('#include \\<(.*)\\>$').match(l)
        if mm:
            node["links"] += [mm[1]]
    nodes[name] = node

print("digraph {")
print("  rankdir = LR")
traverse("vb")
bad = False
for k, v in nodes.items():
    explore(k)
    for l in v["links"]:
        print('    "%s" -> "%s"' % (k, l))
        if l in v["deps"]:
            print("File %s includes unneeded <%s>" % (k, l), file=sys.stderr)
            bad = True
print("}")

if bad == True:
    exit(1)
