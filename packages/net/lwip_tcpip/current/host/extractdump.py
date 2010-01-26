#!/usr/bin/python

import sys
import re
import binascii

def extract_pppdump(input, output):

	fi = open(input, "r")
	fo = open(output, "wb")
	p = re.compile("PPPDUMP:(.*)")
	for line in fi.readlines():
		m = p.match(line)
		if m == None:
			continue
		hex = m.group(1).strip()
		data = binascii.unhexlify(hex)
		fo.write(data)
	fi.close()
	fo.close()

# command line parsing
if len(sys.argv) != 3:
	print "Usage: extractdump.py [input] [output]"
	sys.exit()

extract_pppdump(sys.argv[1], sys.argv[2])
