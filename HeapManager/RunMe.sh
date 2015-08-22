#!/bin/bash
#David Shagam
#Unix 352
#Run me for getting all outputs

make all
./bestheap352 > bestOutput.txt
./worstheap352 > worstOutput.txt
./quickheap352 > quickOutput.txt