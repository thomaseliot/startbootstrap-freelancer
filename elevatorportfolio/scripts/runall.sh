#!/bin/bash
#Public Domain License
#Written by Theodore Martin (tdmartin)

#Update, make clean, make, then run all test scripts.

#This script expects to be run in the code folder. 
#Also, be sure to change the paths to the files to match 
#your file structure.

set -e
set -u

rm -f *.stats

svn update

make clean >>/dev/null
make >> /dev/null

./acceptance.sh
./units2.sh
./integration.sh

rm -f *.stats
