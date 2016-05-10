#!/bin/bash
#Public Domain License
#Written by Theodore Martin (tdmartin)

#Run all acceptance tests. To be run in checking your submission. 

#This script expects to be run in the code folder. 
#Also, be sure to change the paths to the files to match 
#your file structure.

#HEADER is the file that must be on the top of every .stats file.
#Proj12RuntimeMonitor is written late in the semester. Do not worry about it
#until it is obvious to.
#-fs 5.0 is for when fast speed is increased. 
#-b 175 is for when network bandwidth is limited.
#-rt 7000s limits the runtime to 7000 simulated seconds. If the simulation is 
#	taking longer than that, then something is likely wrong. 

set -u
set -e


ls tests/ | awk '/pass/' | while read tests; do
  if [[ "$tests" == "" ]]; then
    #echo "Skipping blank line"
    continue
  fi
  if [[ -f "tests/$tests" ]]; then
    
    java simulator/framework/Elevator -rt 7000s -fs 5.0 -b 175 -monitor Proj12RuntimeMonitor -head HEADER -pf tests/$tests | grep "Acceptance\|RandomSeed\|Stranded\|Violated"
  else
    echo "   $tests does not exist."
    echo "Verification failed."
    exit -1
  fi

#grep output.txt
done
if [[ "$?" -eq "0" ]]; then
  echo "Verification passed"
fi

rm -f *.stats
