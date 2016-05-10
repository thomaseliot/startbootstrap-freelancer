#!/bin/bash
#Public Domain License 
#Written by Theodore Martin (tdmartin)

#Check that the files that have been submitted will compile in a clean copy of 
#the simulator. Also runs simple tests to make sure that the tests still pass.

#This script expects to be run in the folder which contains code and doc. 
#Also, be sure to change the paths to the files to match your file structure.
#HEADER is the file that must be on the top of every .stats file.

set -u
set -e


mkdir clean_copy
cp simulator-3.*.tar.gz clean_copy/simulator.tar.gz
cd clean_copy/

tar xzvf simulator.tar.gz

cp ../Other\ Files/Portfolio/elevatorcontrol/*.java ./code/simulator/elevatorcontrol/

cp ../Other\ Files/Portfolio/integration_test/*.cf ./code/tests/
cp ../Other\ Files/Portfolio/integration_test/*.mf ./code/tests/
cp ../Other\ Files/Portfolio/unit_test/*.cf ./code/tests/
cp ../Other\ Files/Portfolio/unit_test/*.mf ./code/tests/
cp ../Other\ Files/Portfolio/acceptance_test/*.pass ./code/tests/
cp ../code/*.sh ./code/

cp ../code/HEADER ./code/

cd code

make

./trialAccept.sh >> ../../acceptResults.txt
./trialInte.sh >> ../../integratResults.txt
./trialUnits.sh >> ../../unitResults.txt

cd ../../

rm -r clean_copy

