#!/usr/bin/python

import lldb
import subprocess
import sys
import time

p = subprocess.Popen (sys.argv[1:])
stacks = []

while p.poll() == None:
	ll = subprocess.Popen (["lldb", "-p", str(p.pid)], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
	(i,j) = ll.communicate("thread backtrace all\nquit")
	stacks.append (i)
	time.sleep (1)

print stacks
