#!/bin/bash
#Public Domain License
#Written by Theodore Martin (tdmartin)

#Run all unit tests. To be run in checking your submission. 

#This script expects to be run in the code folder. 
#Also, be sure to change the paths to the files to match 
#your file structure.

#HEADER is the file that must be on the top of every .stats file.
#-fs 5.0 is for when fast speed is increased. 

#These are dumbly hard coded. You probably will want to use someone else's script for these. 

set -e
set -u

rm -f *.stats

java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/HallButtonControl.cf -mf tests/HallButtonControl_01.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/HallButtonControl.cf -mf tests/HallButtonControl_02.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/HallButtonControl.cf -mf tests/HallButtonControl_03.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/CarButtonControl.cf -mf tests/CarButtonControl_01.mf| grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/drivecontrol.cf -mf tests/drivecontrol.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/drivecontrol.cf -mf tests/drivecontrol_02.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/DoorControl_01.cf -mf tests/DoorControl_01.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/DoorControl_01.cf -mf tests/DoorControl_02.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/DoorControl_01.cf -mf tests/DoorControl_03.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/DoorControl_01.cf -mf tests/DoorControl_04.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/DoorControl_01.cf -mf tests/DoorControl_05.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/CarPositionControl.cf -mf tests/CarPositionControl_01.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/LanternControl.cf -mf tests/LanternControl_01.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_01.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_02.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_03.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_04.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_05.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_06.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_08.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_09.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_10.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_11.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_12.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_13.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_14.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_15.mf | grep "Message"
java simulator/framework/Elevator -rt 0s -fs 5.0 -head HEADER -cf tests/Dispatcher.cf -mf tests/Dispatcher_16.mf | grep "Message"

grep "Failed" *.stats

rm -f *.stats
