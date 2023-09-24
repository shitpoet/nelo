#!/usr/bin/python 
import re
import subprocess
print "PERFT TEST"

rxp = re.compile("(.*?);.*")
f = open("perft.txt")
f2 = open("perft.sh", "w+")
for l in f.readlines():
    m = rxp.match(l)
    if m:
        f2.write("./obj/chessika \"" + m.group(1) + "x 1 3 0\"\n")
f.close()
f2.close()

