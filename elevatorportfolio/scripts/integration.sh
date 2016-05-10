#!/bin/bash
#Public Domain License
#Written by Theodore Martin (tdmartin)

#Run all integration tests and then put the results in the Portfolio/integration_test folder. 
#Also copies the tests themselves over to the Portfolio/integration_test folder.

#This script expects to be run in the code folder. 
#Also, be sure to change the paths to the files to match 
#your file structure.

#HEADER is the file that must be on the top of every .stats file.
#-fs 5.0 is for when fast speed is increased. 

#These are dumbly hard coded. You probably will want to use someone else's script for these. 

set -e
set -u

rm -f *.stats

cp -f tests/sd*.mf ../Other\ Files/Portfolio/integration_test/
cp -f tests/sd*.cf ../Other\ Files/Portfolio/integration_test/
svn add -q ../Other\ Files/Portfolio/integration_test/sd*.mf >> /dev/null
svn add -q ../Other\ Files/Portfolio/integration_test/sd*.cf >> /dev/null
svn commit -q ../Other\ Files/Portfolio/integration_test/sd* -m "New Integration Tests"


java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd1a1.cf -mf tests/sd1a1.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd1b.cf -mf tests/sd1b.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd1c.cf -mf tests/sd1c.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd2a.cf -mf tests/sd2a.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd2b.cf -mf tests/sd2b.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd3a.cf -mf tests/sd3a.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd4a.cf -mf tests/sd4a.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd5a.cf -mf tests/sd5a.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd5b.cf -mf tests/sd5b.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd6.cf -mf tests/sd6.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd7a.cf -mf tests/sd7a.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd7b.cf -mf tests/sd7b.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd7c.cf -mf tests/sd7c.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd8.cf -mf tests/sd8.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd9a.cf -mf tests/sd9a.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd10a.cf -mf tests/sd10a.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -head HEADER -fs 5.0 -cf tests/sd10b.cf -mf tests/sd10b.mf | grep "Message"

grep "Failed" *.stats

cp -f *.mf-0.stats ../Other\ Files/Portfolio/integration_test/
svn add -q ../Other\ Files/Portfolio/integration_test/*.stats >> /dev/null
svn commit -q ../Other\ Files/Portfolio/integration_test/*.stats -m "New Integration Test Results"
