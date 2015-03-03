#!/usr/bin/env python
#*******************************************************************************
# Copyright (c) 2013 Tippett Studio. All rights reserved.
# $Id: test.py 39588 2014-03-04 22:08:10Z miker $ 
#*******************************************************************************

import sys
import os
import subprocess


TEST_PODS_DIR = [
                 ("./testpods", ".pod"),
                 ("./testvars", ".tpv"),
                 ("./testmuds", ".mud"),
                 ]
PARSER = "./parser"
LOG_FILE = "./test.log"

results = list()
if os.path.exists(LOG_FILE):
    os.remove(LOG_FILE)

for d, ext in TEST_PODS_DIR:
    for f in filter(lambda f: f.endswith(ext), 
                    os.listdir(d)):
        f = os.path.join(d, f)

        p = subprocess.Popen([PARSER, f], 
                             stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        output = p.communicate()[0]
        results.append( (p.returncode, output) )
        testlog = file(LOG_FILE, 'a')
        testlog.write("-"*80)
        testlog.write("\n")
        testlog.write("Input: %s\n" % f)
        testlog.write("returncode: %d\n" % p.returncode)
        if p.returncode != 0:
            print "[31;1mFAIL[0m: %s" % f
            print "\t", filter(bool, output.splitlines())[-1]
            testlog.write("\n")
            testlog.write(output)
            testlog.write("\n")
            testlog.write("\n")
        else:
            print "[32;1mPASS[0m: %s" % f
            sys.stdout.flush()

print
print "     %d tests passed" % (len([r for r in results if r[0] == 0]))
print "     %d tests failed" % (len([r for r in results if r[0] != 0]))
print
