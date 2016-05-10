/**
 * 18-649 Fall 2015
 * 
 * Group 12
 * ---------------------------
 * Daniel Gorziglia - dgorzigl
 * Shepard Emerson - semerson
 * Tom Eliot - tke
 * Daniel Haddox - dhaddox
 * 
 */

package simulator.elevatorcontrol;

import java.util.HashMap;
import simulator.elevatormodules.AtFloorCanPayloadTranslator;
import simulator.elevatormodules.DoorClosedCanPayloadTranslator;
import simulator.payloads.CANNetwork;
import simulator.framework.Direction;
import simulator.framework.DoorCommand;
import simulator.framework.Elevator;
import simulator.framework.Hallway;
import simulator.framework.Harness;
import simulator.framework.ReplicationComputer;
import simulator.framework.Side;
import simulator.payloads.CanMailbox;
import simulator.payloads.CanMailbox.ReadableCanMailbox;

/**
 * This class provides some example utility classes that might be useful in more
 * than one spot.  It is okay to create new classes (or modify the ones given
 * below), but you may not use utility classes in such a way that they constitute
 * a communication channel between controllers.
 */
public class Utility {

	/**
	 * Array of door closed messages
	 */
    public static class DoorClosedArray {

    	// Array of translators
        HashMap<Integer, DoorClosedCanPayloadTranslator> translatorArray = new HashMap<Integer, DoorClosedCanPayloadTranslator>();
        // Hallway for this array
        public final Hallway hallway;

        /**
         * Constructor
         * @param hallway hallway for these doors
         * @param conn CAN connection
         */
        public DoorClosedArray(Hallway hallway, CANNetwork.CanConnection conn) {
        	// Set hallway
            this.hallway = hallway;
            // Create doors for each side
            for (Side s : Side.values()) {
                int index = ReplicationComputer.computeReplicationId(hallway, s);
                ReadableCanMailbox m = CanMailbox.getReadableCanMailbox(
                		MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID + index);
                DoorClosedCanPayloadTranslator t = new DoorClosedCanPayloadTranslator(m, hallway, s);
                conn.registerTimeTriggered(m);
                translatorArray.put(index, t);
            }
        }

        /**
         * Helper function
         * @return whether both doors are closed
         */
        public boolean bothClosed() {
            return translatorArray.get(ReplicationComputer.computeReplicationId(
            			hallway, Side.LEFT)).getValue() 
            		&& translatorArray.get(ReplicationComputer.computeReplicationId(
                    		hallway, Side.RIGHT)).getValue();
        }
        
        /**
         * Helper function
         * @return whether any door is open
         */
        public boolean anyOpen() {
            return !(this.bothClosed());
        }
    }
    
    /**
	 * Array of door motor messages
	 */
    public static class DoorMotorArray {

    	// Array of translators
        HashMap<Integer, DoorMotorCanPayloadTranslator> translatorArray = new HashMap<Integer, DoorMotorCanPayloadTranslator>();
        // Hallway for this array
        public final Hallway hallway;

        /**
         * Constructor
         * @param hallway hallway for these door motors
         * @param conn CAN connection
         */
        public DoorMotorArray(Hallway hallway, CANNetwork.CanConnection conn) {
        	// Set hallway
            this.hallway = hallway;
            // Create doors for each side
            for (Side s : Side.values()) {
                int index = ReplicationComputer.computeReplicationId(hallway, s);
                ReadableCanMailbox m = CanMailbox.getReadableCanMailbox(
                		MessageDictionary.DOOR_MOTOR_COMMAND_BASE_CAN_ID + index);
                DoorMotorCanPayloadTranslator t = new DoorMotorCanPayloadTranslator(m, hallway, s);
                conn.registerTimeTriggered(m);
                translatorArray.put(index, t);
            }
        }

        /**
         * Helper function
         * @return whether any door is opening
         */
        public boolean anyOpening() {
        	return translatorArray.get(ReplicationComputer.computeReplicationId(
        			hallway, Side.LEFT)).getCommand() == DoorCommand.OPEN
        		|| translatorArray.get(ReplicationComputer.computeReplicationId(
                		hallway, Side.RIGHT)).getCommand() == DoorCommand.OPEN;
        }
    }

    /**
     * Array of at floor sensors
     *
     */
    public static class AtFloorArray {
    	// Store translators
        public HashMap<Integer, AtFloorCanPayloadTranslator> networkAtFloorsTranslators = new HashMap<Integer, AtFloorCanPayloadTranslator>();
        public final int numFloors = Elevator.numFloors;

        /**
         * Constructor
         * @param conn CAN connection
         */
        public AtFloorArray(CANNetwork.CanConnection conn) {
            for (int i = 0; i < numFloors; i++) {
                int floor = i + 1;
                for (Hallway h : Hallway.replicationValues) {
                    int index = ReplicationComputer.computeReplicationId(floor, h);
                    ReadableCanMailbox m = CanMailbox.getReadableCanMailbox(MessageDictionary.AT_FLOOR_BASE_CAN_ID + index);
                    AtFloorCanPayloadTranslator t = new AtFloorCanPayloadTranslator(m, floor, h);
                    conn.registerTimeTriggered(m);
                    networkAtFloorsTranslators.put(index, t);
                }
            }
        }
        
        /**
         * Helper function, returns whether the elevator is at a floor
         * @param floor 
         * @param hallway
         * @return whether elevator is at specified floor
         */
        public boolean isAtFloor(int floor, Hallway hallway) {
            return networkAtFloorsTranslators.get(ReplicationComputer.computeReplicationId(floor, hallway)).getValue();
        }

        /**
         * Helper function, get the current floor
         * @return the current floor
         */
        public int getCurrentFloor() {
            int retval = MessageDictionary.NONE;
            for (int i = 0; i < numFloors; i++) {
                int floor = i + 1;
                for (Hallway h : Hallway.replicationValues) {
                    int index = ReplicationComputer.computeReplicationId(floor, h);
                    AtFloorCanPayloadTranslator t = networkAtFloorsTranslators.get(index);
                    if (t.getValue()) {
                        if (retval == MessageDictionary.NONE) {
                            //this is the first true atFloor
                            retval = floor;
                        } else if (retval != floor) {
                            //found a second floor that is different from the first one
                            throw new RuntimeException("AtFloor is true for more than one floor at " + Harness.getTime());
                        }
                    }
                }
            }
            return retval;
        }
    }
    
    /**
     * Array of HallCall sensors
     *
     */
    public static class HallCallArray {
    	// Array of translators
    	public HashMap<Integer, HallCallCanPayloadTranslator> networkHallCallsTranslators 
    		= new HashMap<Integer, HallCallCanPayloadTranslator>();
    	public final int numFloors = Elevator.numFloors;
    	
    	/**
    	 * Constructor
    	 * @param conn CAN connection
    	 */
    	public HallCallArray(CANNetwork.CanConnection conn) {
    		for (int i = 0; i < numFloors; i++) {
    			//do floor 1 seperately
    			int floor = i + 1;
    			for(Hallway h : Hallway.replicationValues) {
    				for(Direction d : Direction.replicationValues) {
    					int index = ReplicationComputer.computeReplicationId(floor, h, d);
    					ReadableCanMailbox m = CanMailbox.getReadableCanMailbox(MessageDictionary.HALL_CALL_BASE_CAN_ID + index);
    					HallCallCanPayloadTranslator t = new HallCallCanPayloadTranslator(m, floor, h, d);
                        conn.registerTimeTriggered(m);
                        networkHallCallsTranslators.put(index, t);
    				}
    			}    				
    		}
    	}
    	
    	/**
    	 * Get whether a call is true for this:
    	 * @param floor floor
    	 * @param hallway hallway
    	 * @param direction direction
    	 * @return whether the call is true for {floor, hallway, direction}
    	 */
    	public boolean getCall(int floor, Hallway hallway, Direction direction) {
    		return networkHallCallsTranslators.get(ReplicationComputer.computeReplicationId(floor, hallway, direction)).getValue();
    	}
    }
    
    /**
     * Array of CarCall sensors
     *
     */
    public static class CarCallArray {
    	
    	// Array of translators
    	public HashMap<Integer, CarCallCanPayloadTranslator> networkCarCallsTranslators = new HashMap<Integer, CarCallCanPayloadTranslator>();
    	public final int numFloors = Elevator.numFloors;
    	
    	/**
    	 * Constructor
    	 * @param conn CAN connection
    	 */
    	public CarCallArray(CANNetwork.CanConnection conn) {
    		for (int i = 0; i < numFloors; i++) {
    			//do floor 1 seperately
    			int floor = i + 1;
    			for(Hallway h : Hallway.replicationValues) {
					int index = ReplicationComputer.computeReplicationId(floor, h);
					ReadableCanMailbox m = CanMailbox.getReadableCanMailbox(MessageDictionary.CAR_CALL_BASE_CAN_ID + index);
					CarCallCanPayloadTranslator t = new CarCallCanPayloadTranslator(m, floor, h);
                    conn.registerTimeTriggered(m);
                    networkCarCallsTranslators.put(index, t);
    			}    				
    		}
    	}
    	
    	/**
    	 * Get whether a call is true for this:
    	 * @param floor floor
    	 * @param hallway hallway
    	 * @return whether a call is true for this {floor, hallway}
    	 */
    	public boolean getCall(int floor, Hallway hallway) {
    		return networkCarCallsTranslators.get(ReplicationComputer.computeReplicationId(floor, hallway)).getValue();
    	}
    }
}
