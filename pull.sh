#!/usr/bin/python

import subprocess
import sys

cmds = { \
        'ladd06' : \
        ['git', 'pull', 'dragon@ladd06.triumf.ca:/home/dragon/gc/dragon/analyzer', 'dev'], \
        'jabberwock' : \
        ['git', 'pull', 'gchristian@jabberwock.triumf.ca:/Users/gchristian/soft/develop/dragon/analyzer', 'dev'] }

if len(sys.argv) == 1:
    print 'usage: pull.sh <remote machine>'
    exit(1)

try:
    cmd = cmds[sys.argv[1]]
    subprocess.call(cmd)
except KeyError as e:
    print 'Invalid argument: ' + sys.argv[1] + '\nValid arguments:'
    for key in cmds.keys():
        print '\t' + key

    

