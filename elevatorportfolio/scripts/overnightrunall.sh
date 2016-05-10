#!/bin/bash
#Public Domain License
#Written by Theodore Martin (tdmartin)

#Update, make clean, make, then run all test scripts.
#Run the scripts 1000 times. Prints out to keep informed. 

#This script expects to be run in the code folder. 
#Also, be sure to change the paths to the files to match 
#your file structure.

set -e
set -u

rm -f *.stats

svn update

make clean >>/dev/null
make >> /dev/null

for ((c=1; c<=1000; c++))
do
  ./overnightunits.sh >> unitResults.txt
  echo "Unit Done"
  ./overnightintegration.sh >> integrationResults.txt
  echo "Integration Done"
  ./overnightacceptance.sh >> acceptanceResults.txt
  echo "Acceptance Done"
done

rm -f *.stats



