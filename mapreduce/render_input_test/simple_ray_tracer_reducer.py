#!/usr/bin/env python

import sys

can_output = True

# takes input stdin
for line in sys.stdin:

    if(can_output):
        print(line)

    if(line == 'COLOR LIST START'):
        can_output = True

    if(line == 'COLOR LIST END'):
        can_output = False
