#!/usr/bin/python

import lldb
import re
import subprocess
import sys
import time

p = subprocess.Popen (sys.argv[1:])
stacks = []

while p.poll() == None:
	ll = subprocess.Popen (["lldb", "-p", str(p.pid)], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	(i,j) = ll.communicate("thread backtrace\nquit")
	stacks.append (i)
	time.sleep (.1)

stats = {}

for s in stacks:
	while re.search ("<[^<>]*>", s): s = re.sub ("<[^<>]*>", "", s)
	while re.search ("\([^()]*\)", s): s = re.sub ("\([^()]*\)", "", s)
	path = ""
	for line in s.splitlines():
		if not re.search("frame",line): continue
		line = re.sub ("^[ \*]*frame #[0-9]*: 0x[0-9a-f]* ", "", line)
		line = re.sub (" \+ [0-9]*( at [^ ]*:[0-9]*)?$", "", line)
		if re.match ("libdyld.dylib", line): continue
		line = re.sub ("^.*`", "", line)
		if line == "main": continue
		path = "/" + line + path
	if path == "": continue
	if path not in stats: stats[path] = 0
	stats[path] += 1

syn = []
for (k,v) in stats.iteritems():
	syn.append ("| %4d | %s" % (v,k))

print
print "+------+-----------------------------------"
for l in sorted(syn, reverse=True): print l
print "+------+-----------------------------------"
