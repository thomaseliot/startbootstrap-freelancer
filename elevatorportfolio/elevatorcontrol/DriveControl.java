/**
 * 18-649 Fall 2015
 * 
 * Group 12
 * ---------------------------
 * Daniel Gorziglia - dgorzigl
 * Shepard Emerson - semerson
 * Tom Eliot - tke
 * Daniel Haddox - dhaddox
 */
package simulator.elevatorcontrol;

import java.util.ArrayList;


import jSimPack.SimTime;
import simulator.elevatorcontrol.Utility.DoorMotorArray;
import simulator.elevatormodules.AtFloorCanPayloadTranslator;
import simulator.elevatormodules.CarWeightCanPayloadTranslator;
import simulator.elevatormodules.DoorClosedCanPayloadTranslator;
import simulator.elevatormodules.LevelingCanPayloadTranslator;
import simulator.elevatormodules.SafetySensorCanPayloadTranslator;
import simulator.framework.Direction;
import simulator.framework.Elevator;
import simulator.framework.Hallway;
import simulator.framework.ReplicationComputer;
import simulator.framework.Side;
import simulator.framework.Speed;
import simulator.payloads.CanMailbox;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.DrivePayload;
import simulator.payloads.DrivePayload.WriteableDrivePayload;
import simulator.payloads.DriveSpeedPayload;
import simulator.payloads.DriveSpeedPayload.ReadableDriveSpeedPayload;
import simulator.payloads.translators.IntegerCanPayloadTranslator;


/**
 * DriveControl
 * Replication: none
 */
public class DriveControl extends simulator.framework.Controller {
	// Constants
	private final static int AT_NO_FLOOR = -1;
	private final static double DISTANCE_BUFFER_VAL = 0.2;
	private static final int ACCEL_VAL = 1;
	
	/***************************************************************************
     * Declarations
     **************************************************************************/
	
    // Note that inputs are Readable objects, while outputs are Writeable objects

    // Local physical state
	private WriteableDrivePayload localDriveCommand;
    private ReadableDriveSpeedPayload localDriveSpeed;
    
    // Network interface
    // Setup write mailboxes
    private WriteableCanMailbox networkDriveMsg;

    // Setup read mailboxes
    private ReadableCanMailbox networkDoorClosedFrontLeft;
    private ReadableCanMailbox networkDoorClosedFrontRight;
    private ReadableCanMailbox networkDoorClosedBackLeft;
    private ReadableCanMailbox networkDoorClosedBackRight;
    private ReadableCanMailbox networkCarWeight;
    private ReadableCanMailbox networkEmergencyBrake;
    private ReadableCanMailbox networkDesiredFloor;
    private ReadableCanMailbox networkLevelUp;
    private ReadableCanMailbox networkLevelDown;
    private ReadableCanMailbox networkCarPositionIndicator;
    private ReadableCanMailbox networkCarLevelPosition;
    
    // ArrayList to store AtFloor sensor CAN messages
    ArrayList<ReadableCanMailbox> networkAtFloorFront = new ArrayList<>(Elevator.numFloors);
    ArrayList<ReadableCanMailbox> networkAtFloorBack = new ArrayList<>(Elevator.numFloors);
    
    // Translate all messages
    private DoorClosedCanPayloadTranslator mDoorClosedFrontLeft; 
    private DoorClosedCanPayloadTranslator mDoorClosedFrontRight; 
    private DoorClosedCanPayloadTranslator mDoorClosedBackLeft; 
    private DoorClosedCanPayloadTranslator mDoorClosedBackRight;
	DoorMotorArray doorMotorArrayFront;
	DoorMotorArray doorMotorArrayBack;
    private CarWeightCanPayloadTranslator mCarWeight;
    private SafetySensorCanPayloadTranslator mEmergencyBrake;
    private DesiredFloorCanPayloadTranslator mDesiredFloor;
    private DriveCanPayloadTranslator mDrive;
    private LevelingCanPayloadTranslator mLevelUp;
    private LevelingCanPayloadTranslator mLevelDown;
    private IntegerCanPayloadTranslator mCarLevelPosition;
    
    // ArrayList to store AtFloor message translators
    ArrayList<AtFloorCanPayloadTranslator> mAtFloorFront = new ArrayList<>(Elevator.numFloors);
    ArrayList<AtFloorCanPayloadTranslator> mAtFloorBack = new ArrayList<>(Elevator.numFloors);
    
    // Store the period for the controller
    private SimTime period;
    
    // Commit point state variable
    private int commitPoint;
    
    // Enumerate states
    private enum State {
        STATE_SLOW_UP,
        STATE_SLOW_DOWN,
        STATE_STOP,
        STATE_LEVEL_UP,
        STATE_LEVEL_DOWN,
        STATE_FAST_UP,
        STATE_FAST_DOWN
    }
    
    // State variable initialized to the initial state FLASH_OFF
    private State state = State.STATE_STOP;

    /**
     * Constructor for DriveControl
     * @param period the period to run this controller at
     * @param verbose verbose mode
     */
	public DriveControl(SimTime period, boolean verbose) {
		// Call superclass constructor
	    super("DriveControl", verbose);
	    	    
	    // Stored the constructor arguments in internal state
	    this.period = period;
	    
	    // Log creation of object
	    log("Created DriveControl with period = ", period);
	
	    // Initialize physical state
	    // Create a payload object for driveControl using the
	    // static factory method in DriveSpeedPayload.
	    localDriveCommand = DrivePayload.getWriteablePayload();
	    localDriveSpeed = DriveSpeedPayload.getReadablePayload();
	    
	    // Register the payload with the physical interface to be updated periodically
	    physicalInterface.registerTimeTriggered(localDriveSpeed);
	    physicalInterface.sendTimeTriggered(localDriveCommand, period);
	    log("Initialized physical states");
	    
	    // Initialize network interface        
	   
	    // Drive command and speed
	    networkDriveMsg = CanMailbox.getWriteableCanMailbox(
	    		MessageDictionary.DRIVE_CAN_ID);
	    // Car weight
	    networkCarWeight = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.CAR_WEIGHT_CAN_ID);
	    // Emergency brake
	    networkEmergencyBrake = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.EMERGENCY_BRAKE_CAN_ID);
	    // Desired floor
	    networkDesiredFloor = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.DESIRED_FLOOR_CAN_ID);
	    // Front left door
	    networkDoorClosedFrontLeft = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID 
	    		+ ReplicationComputer.computeReplicationId(Hallway.FRONT, Side.LEFT));
	    // Front right
	    networkDoorClosedFrontRight = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID 
	    		+ ReplicationComputer.computeReplicationId(Hallway.FRONT, Side.RIGHT));
	    // Back left
	    networkDoorClosedBackLeft = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID 
	    		+ ReplicationComputer.computeReplicationId(Hallway.BACK, Side.LEFT));
	    // Back right
	    networkDoorClosedBackRight = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID 
	    		+ ReplicationComputer.computeReplicationId(Hallway.BACK, Side.RIGHT));
	    // Level up
	    networkLevelUp = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.LEVELING_BASE_CAN_ID 
	    		+ ReplicationComputer.computeReplicationId(Direction.UP));
	    // Level down
	    networkLevelDown = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.LEVELING_BASE_CAN_ID 
	    		+ ReplicationComputer.computeReplicationId(Direction.DOWN));
	    // Car position indicator
	    networkCarPositionIndicator = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.CAR_POSITION_CAN_ID);
	    // Car level position
	    networkCarLevelPosition = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.CAR_LEVEL_POSITION_CAN_ID);
	    
	    // Create CAN mailboxes for AtFloor received messages
	    for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
	    	// Front Hallway
	    	// Only add a floor if the elevator has a landing
	    	if(Elevator.hasLanding(floorNum, Hallway.FRONT)) {
	        	networkAtFloorFront.add(floorNum - 1, CanMailbox.getReadableCanMailbox(
	        			MessageDictionary.AT_FLOOR_BASE_CAN_ID
	        			+ ReplicationComputer.computeReplicationId(floorNum, Hallway.FRONT)));
	    	} else {
	    		// If no landing add null
	    		networkAtFloorFront.add(floorNum - 1, null);
	    	}
	    	// Back Hallway
	    	// Only add a floor if the elevator has a landing
	    	if(Elevator.hasLanding(floorNum, Hallway.BACK)) {
	        	networkAtFloorBack.add(floorNum - 1, CanMailbox.getReadableCanMailbox(
	        			MessageDictionary.AT_FLOOR_BASE_CAN_ID
	        			+ ReplicationComputer.computeReplicationId(floorNum, Hallway.BACK)));
	    	} else {
	    		// If no landing add null
	    		networkAtFloorBack.add(floorNum - 1, null);
	    	}
	    }
	    
	    // Log that interface is initialized
	    log("Network interface initialized");
	    
	    // Create CAN translators
	    mDrive = new DriveCanPayloadTranslator(networkDriveMsg);
	    mCarWeight = new CarWeightCanPayloadTranslator(networkCarWeight);
	    mEmergencyBrake = new SafetySensorCanPayloadTranslator(networkEmergencyBrake);
	    mDesiredFloor = new DesiredFloorCanPayloadTranslator(networkDesiredFloor);
	    mDoorClosedFrontLeft = new DoorClosedCanPayloadTranslator(networkDoorClosedFrontLeft, Hallway.FRONT, Side.LEFT);
	    mDoorClosedFrontRight = new DoorClosedCanPayloadTranslator(networkDoorClosedFrontRight, Hallway.FRONT, Side.RIGHT);
	    mDoorClosedBackLeft = new DoorClosedCanPayloadTranslator(networkDoorClosedBackLeft, Hallway.BACK, Side.LEFT);
	    mDoorClosedBackRight = new DoorClosedCanPayloadTranslator(networkDoorClosedBackRight, Hallway.BACK, Side.RIGHT);
	    mLevelUp = new LevelingCanPayloadTranslator(networkLevelUp,Direction.UP);
	    mLevelDown = new LevelingCanPayloadTranslator(networkLevelDown,Direction.DOWN);
	    mCarLevelPosition = new IntegerCanPayloadTranslator(networkCarLevelPosition);
	    canInterface.sendTimeTriggered(networkDriveMsg, period);
	    
    	doorMotorArrayFront = new DoorMotorArray(Hallway.FRONT, this.canInterface);
    	doorMotorArrayBack = new DoorMotorArray(Hallway.BACK, this.canInterface);
	    
	
	    // Create translators for AtFloor received messages for front and back hallway
	    for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
	    	// Front Hallway
	    	// Only add a floor if the elevator has a landing
	    	if(Elevator.hasLanding(floorNum, Hallway.FRONT)) {
	    		mAtFloorFront.add(floorNum - 1, new AtFloorCanPayloadTranslator(
	    				networkAtFloorFront.get(floorNum - 1), floorNum, Hallway.FRONT));
	    	} else {
	    		// If no landing, add null
	    		mAtFloorFront.add(floorNum - 1, null);
	    	}
	    	// Back Hallway
	    	// Only add a floor if the elevator has a landing
	    	if(Elevator.hasLanding(floorNum, Hallway.BACK)) {
	    		mAtFloorBack.add(floorNum - 1, new AtFloorCanPayloadTranslator(
	    				networkAtFloorBack.get(floorNum - 1), floorNum, Hallway.BACK));
	    	} else {
	    		// If no landing, add null
	    		mAtFloorBack.add(floorNum - 1, null);
	    	}
	    }
	    
	    // Register to receive periodic updates
	    canInterface.registerTimeTriggered(networkDoorClosedFrontLeft);
	    canInterface.registerTimeTriggered(networkDoorClosedFrontRight);
	    canInterface.registerTimeTriggered(networkDoorClosedBackLeft);
	    canInterface.registerTimeTriggered(networkDoorClosedBackRight);
	    canInterface.registerTimeTriggered(networkCarWeight);
	    canInterface.registerTimeTriggered(networkEmergencyBrake);
	    canInterface.registerTimeTriggered(networkDesiredFloor);
	    canInterface.registerTimeTriggered(networkLevelUp);
	    canInterface.registerTimeTriggered(networkLevelDown);
	    canInterface.registerTimeTriggered(networkCarPositionIndicator);
	    canInterface.registerTimeTriggered(networkCarLevelPosition);
	    
	    // Register all AtFloor receive messages to receive periodic updates
	    for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
	    	ReadableCanMailbox floorNumAtFloorFront = networkAtFloorFront.get(floorNum - 1);
	    	if(floorNumAtFloorFront != null) {
	    		// Only register if this sensor exists
	    		canInterface.registerTimeTriggered(networkAtFloorFront.get(floorNum - 1));
	    	}
	    	ReadableCanMailbox floorNumAtFloorBack = networkAtFloorBack.get(floorNum - 1);
	    	if(floorNumAtFloorBack != null) {
	    		// Only register if this sensor exists
	    		canInterface.registerTimeTriggered(networkAtFloorBack.get(floorNum - 1));
	    	}
	    }
	    // Initialize commit point to first floor
	    commitPoint = 1;
	    
	    // Schedule first iteration of the controller
	    timer.start(period);
	}
	
	/**
	 * Timer expired callback, called with controller period
	 */
	public void timerExpired(Object callbackData) {
		// Variable to hold new state, initially currentState
	    State newState = state;
	    
	    // Commit point calculation
		double buffer = DISTANCE_BUFFER_VAL; 	// Distance buffer in m
		// Position in m/s, 1000 converts mm to m
		double x0 = ((double)mCarLevelPosition.getValue())/1000;
		double v0 = 0; 			// Speed in m/s
		double a0 = 0; 			// Acceleration in m/s
		// New commit point
		int newCommitPoint = 0;
		
		// If going up
		if(mDrive.getSensorDirection() == Direction.UP) {
			 v0 = localDriveSpeed.speed(); 
			 a0 = -ACCEL_VAL;
		} else { // If going down
			 v0 = -localDriveSpeed.speed();
			 a0 = ACCEL_VAL;
		}
		
		// Stopping time and distance
		double tS = -v0/a0;
		double xS = x0 + v0*(tS)+0.5*a0*(tS)*(tS) + buffer*v0;
		
		// If going up
		if(mDrive.getSensorDirection() == Direction.UP) {
			// Commit point should only increase. This reduces jitter if the 
			// elevator slows down faster than 1m/s/s
			newCommitPoint = ((int) Math.floor(xS/Elevator.DISTANCE_BETWEEN_FLOORS))+1;
			
			//
			if(newCommitPoint > commitPoint) commitPoint = newCommitPoint;
		} else {
			newCommitPoint = ((int) Math.ceil(xS/Elevator.DISTANCE_BETWEEN_FLOORS))+1;
			if(newCommitPoint < commitPoint) commitPoint = newCommitPoint;
		}
		
	    // Case on the states
	    switch (state) {
	    	// Drive is stopped
	        case STATE_STOP:
	        	// State actions for 'STOP'
	        	localDriveCommand.set(Speed.STOP, Direction.STOP);
	        	
	        	mDrive.set((float)localDriveSpeed.speed(), localDriveSpeed.direction(),
	        			Speed.STOP, Direction.STOP);
	
	            //#transition '6.T.1'
	        	if ((mEmergencyBrake.getValue() == false) &&
	        		(localDriveSpeed.speed() == simulator.elevatormodules.DriveObject.StopSpeed) &&
        			((desiredDirection() == Direction.UP && allDoorsClosed()) &&
        					!anyDoorMotorOpen() &&
        					mCarWeight.getWeight() < simulator.framework.Elevator.MaxCarCapacity|| 
        					(mLevelUp.getValue() == false))){
	        		newState = State.STATE_LEVEL_UP;
	        	} 
	            //#transition '6.T.7'
	        	else if ((mEmergencyBrake.getValue() == false) &&
	        		(localDriveSpeed.speed() == simulator.elevatormodules.DriveObject.StopSpeed) &&
        			((desiredDirection() == Direction.DOWN && allDoorsClosed() &&
					!anyDoorMotorOpen()) || (mLevelDown.getValue() == false))){
	        		newState = State.STATE_LEVEL_DOWN;
	        	}
	            break;
	        	
	        case STATE_LEVEL_UP:
	        	//state actions for 'LEVEL_UP'
	        	localDriveCommand.set(Speed.LEVEL, Direction.UP);
	        	mDrive.set((float)localDriveSpeed.speed(), localDriveSpeed.direction(),
	        			Speed.LEVEL, Direction.UP);
	            
	        	//#transition '6.T.2'
	        	if ((mCarWeight.getWeight() < simulator.framework.Elevator.MaxCarCapacity) &&
		        		(allDoorsClosed() == true) &&
		        		(!anyDoorMotorOpen()) &&
		        		(mEmergencyBrake.getValue() == false) &&
		        		(localDriveSpeed.speed() == simulator.elevatormodules.DriveObject.LevelingSpeed) &&
		        		(localDriveSpeed.direction() == Direction.UP) &&
	        			(desiredDirection() == Direction.UP)){
		        		newState = State.STATE_SLOW_UP;
		        	} 
	        	//#transition '6.T.6'
	        	else if (mEmergencyBrake.getValue() == true || 
	        		(mLevelUp.getValue() == true && 
	        		(allDoorsClosed() == false || desiredDirection() == Direction.STOP))){
	        		newState = State.STATE_STOP;
	        	}
	            break;    
	        case STATE_SLOW_UP:
	            //state actions for 'SLOW_UP'
	        	localDriveCommand.set(Speed.SLOW, Direction.UP);
	        	mDrive.set((float)localDriveSpeed.speed(), localDriveSpeed.direction(),
	        			Speed.SLOW, Direction.UP);
	        	
	        	//#transition '6.T.3'
	        	if ((mEmergencyBrake.getValue() == false) &&
	        		(localDriveSpeed.speed() == simulator.elevatormodules.DriveObject.SlowSpeed) &&
	        		(localDriveSpeed.direction() == Direction.UP) &&
        			(commitPoint < mDesiredFloor.getFloor())){
	        		newState = State.STATE_FAST_UP;
		        } 
	        	//#transition '6.T.5'
	        	else if((mEmergencyBrake.getValue() == false) &&
	        			(localDriveSpeed.speed() == simulator.elevatormodules.DriveObject.SlowSpeed) &&
		        		(localDriveSpeed.direction() == Direction.UP) &&
		        		(getCurrentFloor() == mDesiredFloor.getFloor())){
	        		newState = State.STATE_LEVEL_UP;
	        	}
	        	//#transition '6.T.16'
	        	else if(mEmergencyBrake.getValue() == true)
	        		newState = State.STATE_STOP;
	        	break;
	        case STATE_FAST_UP:
	        	//state actions for 'FAST_UP'
	        	localDriveCommand.set(Speed.FAST, Direction.UP);
	        	mDrive.set((float)localDriveSpeed.speed(), localDriveSpeed.direction(),
	        			Speed.FAST, Direction.UP);
	        	
	        	//#transition '6.T.4'
	        	if((mEmergencyBrake.getValue() == false) &&
	        		(commitPoint == mDesiredFloor.getFloor())){
	        		newState = State.STATE_SLOW_UP;
	        	}
	        	//#transition '6.T.13'
	        	else if(mEmergencyBrake.getValue() == true)
	        		newState = State.STATE_STOP;
	        	break;
	        	
	       
	    
	    case STATE_LEVEL_DOWN:
        	//state actions for 'LEVEL_DOWN'
        	localDriveCommand.set(Speed.LEVEL, Direction.DOWN);
        	mDrive.set((float)localDriveSpeed.speed(), localDriveSpeed.direction(),
        			Speed.LEVEL, Direction.DOWN);
            
        	//#transition '6.T.8'
        	if ((mCarWeight.getWeight() < simulator.framework.Elevator.MaxCarCapacity) &&
	        		(allDoorsClosed() == true) &&
	        		(!anyDoorMotorOpen()) &&
	        		(mEmergencyBrake.getValue() == false) &&
	        		(localDriveSpeed.speed() == simulator.elevatormodules.DriveObject.LevelingSpeed) &&
	        		(localDriveSpeed.direction() == Direction.DOWN) &&
        			(desiredDirection() == Direction.DOWN)){
	        		newState = State.STATE_SLOW_DOWN;
	        	} 
        	//#transition '6.T.12'
        	else if (mEmergencyBrake.getValue() == true || 
        		(mLevelDown.getValue() == true && 
        		(allDoorsClosed() == false || desiredDirection() == Direction.STOP))){
        		newState = State.STATE_STOP;
        	}
            break;    
        case STATE_SLOW_DOWN:
            //state actions for 'SLOW_UP'
        	localDriveCommand.set(Speed.SLOW, Direction.DOWN);
        	mDrive.set((float)localDriveSpeed.speed(), localDriveSpeed.direction(),
        			Speed.SLOW, Direction.DOWN);
        	
        	//#transition '6.T.9'
        	if ((mEmergencyBrake.getValue() == false) &&
        		(localDriveSpeed.speed() == simulator.elevatormodules.DriveObject.SlowSpeed) &&
        		(localDriveSpeed.direction() == Direction.DOWN) &&
    			(commitPoint > mDesiredFloor.getFloor())){
        		newState = State.STATE_FAST_DOWN;
	        } 
        	//#transition '6.T.11'
        	else if((mEmergencyBrake.getValue() == false) &&
        			(localDriveSpeed.speed() == simulator.elevatormodules.DriveObject.SlowSpeed) &&
	        		(localDriveSpeed.direction() == Direction.DOWN) &&
	        		(getCurrentFloor() == mDesiredFloor.getFloor())){
        		newState = State.STATE_LEVEL_DOWN;
        	}
        	//#transition '6.T.14'
        	else if(mEmergencyBrake.getValue() == true)
        		newState = State.STATE_STOP;
        	break;
        case STATE_FAST_DOWN:
        	//state actions for 'FAST_UP'
        	localDriveCommand.set(Speed.FAST, Direction.DOWN);
        	mDrive.set((float)localDriveSpeed.speed(), localDriveSpeed.direction(),
        			Speed.FAST, Direction.DOWN);
        	
        	//#transition '6.T.10'
        	if((mEmergencyBrake.getValue() == false) &&
        		(commitPoint == mDesiredFloor.getFloor())){
        		newState = State.STATE_SLOW_DOWN;
        	}
        	//#transition '6.T.15'
        	else if(mEmergencyBrake.getValue() == true)
        		newState = State.STATE_STOP;
        	break;
        	
	        // If no state matches. This should never happen
        default:
            throw new RuntimeException("State " + state + " was not recognized.");
    }
        	
	    
	    // Log the results of this iteration
	    if (state == newState) {
	        log("remains in state: ",state);
	    } else {
	        log("Transition:",state,"->",newState);
	    }
	
	    // Update the state variable
	    state = newState;
	
	    // Report the current state
	    setState(STATE_KEY,newState.toString());
	
	    // Schedule the next iteration of the controller
	    timer.start(period);
	}
	
	/**
	 * See if all doors are closed
	 * @return true if all doors are closed
	 */
	private boolean allDoorsClosed(){
		return (mDoorClosedFrontLeft.getValue() == true) &&
	    		(mDoorClosedFrontRight.getValue() == true) &&
	    		(mDoorClosedBackLeft.getValue() == true) &&
	    		(mDoorClosedBackRight.getValue() == true);
	}
	
	/**
	 * Helper function to get current floor
	 * @return current floor, or AT_NO_FLOOR if no current floor
	 */
	private int getCurrentFloor() {
		int floor = atFloorFrontTrue();
		if (floor != AT_NO_FLOOR) return floor;
		else return atFloorBackTrue();
	}
	
	/**
	 * Helper function to get desired direction based on currentFloor. 
	 * Assumes some mAtFloor is true so current floor must be valid.
	 * @return desired direction
	 */
	private Direction desiredDirection() {
		int currentFloor = getCurrentFloor();
		if (currentFloor == AT_NO_FLOOR){
			//should never get here
            throw new RuntimeException("currentFloor is somehow AT_NO_FLOOR in desiredDirection, driveControl");
		}
		else if (currentFloor < mDesiredFloor.getFloor())
			//you're below your desired floor, desired direction up
			return Direction.UP;
		else if (currentFloor > mDesiredFloor.getFloor())
			//you're above your desired floor, desired direction down
			return Direction.DOWN;
		else { //if you're not in between floors and not moving, you must be at your stop
			return Direction.STOP;
		}
	}
	
	/**
	 * Helper function. See if any AtFloor is true (for front hallway).
	 * @return the floor number, or AT_NO_FLOOR if not at any floors
	 */
	private int atFloorFrontTrue() {
		for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
			// Return floorNum if true
			AtFloorCanPayloadTranslator floorNumAtFloorFront = mAtFloorFront.get(floorNum - 1);
			if (floorNumAtFloorFront != null && floorNumAtFloorFront.getValue()) {
				return floorNum;
			}
		}
		
		// If none were true, return AT_NO_FLOOR
		return AT_NO_FLOOR;
	}
	
	/**
	 * Helper function
	 * Checks if any of all four doors are commanded to open.
	 * @return true if any door commanded to open
	 */
	private boolean anyDoorMotorOpen(){
		return (doorMotorArrayFront.anyOpening() &&
				doorMotorArrayBack.anyOpening());
	}
	
	/**
	 * Helper function. See if any AtFloor is true (for back hallway).
	 * @return the floor number, or AT_NO_FLOOR if not at any floors
	 */
	private int atFloorBackTrue() {
		for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
			// Return floorNum if true
			AtFloorCanPayloadTranslator floorNumAtFloorBack = mAtFloorBack.get(floorNum - 1);
			if (floorNumAtFloorBack != null && floorNumAtFloorBack.getValue()) {
				return floorNum;
			}
		}
		
		// If none were true, return AT_NO_FLOOR
		return AT_NO_FLOOR;
	}
}