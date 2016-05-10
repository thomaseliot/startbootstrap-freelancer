#!/bin/bash
#Public Domain License
#Written by Theodore Martin (tdmartin)

#Copy java files from code/simulator to the portfolio and commit to repository.

#This script expects to be run in the code folder. 
#Also, be sure to change the paths to the files to match 
#your file structure.

set -u
set -e

cp -f simulator/elevatorcontrol/*.java ../Other\ Files/Portfolio/elevatorcontrol/
svn add -q ../Other\ Files/Portfolio/elevatorcontrol/*.java
svn commit ../Other\ Files/Portfolio/elevatorcontrol/*.java -m "Copied over elevatorcontrol"






