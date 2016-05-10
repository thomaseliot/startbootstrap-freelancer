#!/bin/bash
#Public Domain License
#Written by Theodore Martin (tdmartin)

#Run all acceptance tests and then put the results in the Portfolio/acceptance_test folder. 
#Also copies the tests themselves over to the Portfolio/acceptance_test folder.

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


cp -f tests/*.pass ../Other\ Files/Portfolio/acceptance_test/
svn add -q ../Other\ Files/Portfolio/acceptance_test/*.pass >> /dev/null
svn commit -q ../Other\ Files/Portfolio/acceptance_test/*.pass -m "New Acceptance Tests"

ls tests/ | awk '/pass/' | while read tests; do
  if [[ "$tests" == "" ]]; then
    #echo "Skipping blank line"
    continue
  fi
  if [[ -f "tests/$tests" ]]; then
    echo "   $tests exists."
    java simulator/framework/Elevator -rt 7000s -fs 5.0 -b 175 -monitor Proj12RuntimeMonitor -head HEADER -pf tests/$tests | grep "RandomSeed\|Delivered\|Stranded\|Total\|delivery_time\|Performance"
  else
    echo "   $tests does not exist."
    echo "Verification failed."
    exit -1
  fi

done
if [[ "$?" -eq "0" ]]; then
  echo "Verification passed"
fi

cp -f elevator*.stats ../Other\ Files/Portfolio/acceptance_test/
svn add -q ../Other\ Files/Portfolio/acceptance_test/*.stats >> /dev/null
svn commit -q ../Other\ Files/Portfolio/acceptance_test/ -m "New Acceptance Test Results"

rm -f *.stats
