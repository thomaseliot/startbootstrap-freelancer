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
import simulator.elevatormodules.AtFloorCanPayloadTranslator;
import simulator.framework.Controller;
import simulator.framework.Direction;
import simulator.framework.Elevator;
import simulator.framework.Hallway;
import simulator.framework.ReplicationComputer;
import simulator.payloads.CanMailbox;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.CarPositionIndicatorPayload;
import simulator.payloads.CarPositionIndicatorPayload.WriteableCarPositionIndicatorPayload;
import simulator.payloads.translators.IntegerCanPayloadTranslator;

/**
 * CarPosition Controller Class
 *
 */
public class CarPositionControl extends Controller {
	
	// Constants
	private static final int ACCEL_VAL = 1;
	private static final int AT_NO_FLOOR = -1;
	private final static float LEVELING_MARGIN = (float) 0.01;
	
	/*
	 * mAtFloor Variables
	 */
	
	// Get CAN mailboxes for atFloor messages
    private ArrayList<ReadableCanMailbox> networkAtFloorFront = new ArrayList<>(Elevator.numFloors);
    private ArrayList<ReadableCanMailbox> networkAtFloorBack = new ArrayList<>(Elevator.numFloors);

    // ArrayList to store AtFloor message translators
    private ArrayList<AtFloorCanPayloadTranslator> mAtFloorFront = new ArrayList<>(Elevator.numFloors);
    private ArrayList<AtFloorCanPayloadTranslator> mAtFloorBack = new ArrayList<>(Elevator.numFloors);
    
    /*
     * CarPositionIndicator
     */
    
    // Physical Actuator
    private WriteableCarPositionIndicatorPayload localCarPositionIndicator;
    
    // CAN mailbox for network message
    private WriteableCanMailbox networkCarPositionIndicator;
    private IntegerCanPayloadTranslator mCarPositionIndicator;
    private DriveCanPayloadTranslator mDrive;
    
    private ReadableCanMailbox networkCarLevelPosition;
    private ReadableCanMailbox networkDriveMsg;
    private IntegerCanPayloadTranslator mCarLevelPosition;
    
    /*
     * Sim and State Variables
     */
    
    // Store the period for the controller
    private SimTime period;
    
    // State variables
    private int nextFloor = 0;
    
    // Enumerate states
    private enum State {
        STATE_TRAVELING,
        STATE_REACHED_FLOOR
    }
    
    // State variable initialized to the initial state FLASH_OFF
    private State state = State.STATE_REACHED_FLOOR;
    
    // currentFloor
    private int currentFloor = 1;
    
    
    /**
     * Constructor
     * @param period the period at which to run this controller
     * @param verbose
     */
	public CarPositionControl(SimTime period, boolean verbose) {
		// Call superclass constructor
		super("CarPositionControl", verbose);
				
		// Store the constructor arguments in internal state
	    this.period = period;
	    
        // Log creation of object
        log("Created CarPositionControl with period = ", this.period);
        
        // Create CAN mailboxes for AtFloor received messages
        for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
        	// Front Hallway
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, Hallway.FRONT)) {
            	networkAtFloorFront.add(floorNum - 1, CanMailbox.getReadableCanMailbox(
            			MessageDictionary.AT_FLOOR_BASE_CAN_ID
            			+ ReplicationComputer.computeReplicationId(floorNum, Hallway.FRONT)));
        	} else {
        		// If no landing, add null
        		networkAtFloorFront.add(floorNum - 1, null);
        	}
        	
        	// Back Hallway
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, Hallway.BACK)) {
            	networkAtFloorBack.add(floorNum - 1, CanMailbox.getReadableCanMailbox(
            			MessageDictionary.AT_FLOOR_BASE_CAN_ID
            			+ ReplicationComputer.computeReplicationId(floorNum, Hallway.BACK)));
        	} else {
        		// If no landing, add null
        		networkAtFloorBack.add(floorNum - 1, null);
        	}
        }
        
        // Create translators for AtFloor received messages for front and back hallway
        for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
        	// Front Hallway
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, Hallway.FRONT)) {
        		mAtFloorFront.add(floorNum - 1, new AtFloorCanPayloadTranslator(networkAtFloorFront.get(floorNum - 1), floorNum, Hallway.FRONT));
        	} else {
        		// If no landing, add null
        		mAtFloorFront.add(floorNum - 1, null);
        	}
        	
        	// Back Hallway
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, Hallway.BACK)) {
        		mAtFloorBack.add(floorNum - 1, new AtFloorCanPayloadTranslator(networkAtFloorBack.get(floorNum - 1), floorNum, Hallway.BACK));
        	} else {
        		// If no landing add null
        		mAtFloorBack.add(floorNum - 1, null);
        	}
        }
        
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
        
        // Initialize network interface
        networkCarPositionIndicator = CanMailbox.getWriteableCanMailbox(MessageDictionary.CAR_POSITION_CAN_ID);
        networkCarLevelPosition = CanMailbox.getReadableCanMailbox(MessageDictionary.CAR_LEVEL_POSITION_CAN_ID);
        networkDriveMsg = CanMailbox.getReadableCanMailbox(MessageDictionary.DRIVE_CAN_ID);
        
        // Create translator using generic integer translator
        mCarPositionIndicator = new IntegerCanPayloadTranslator(networkCarPositionIndicator);
        mCarLevelPosition = new IntegerCanPayloadTranslator(networkCarLevelPosition);
        mDrive = new DriveCanPayloadTranslator(networkDriveMsg);
        
        // Register mailboxes
        canInterface.sendTimeTriggered(networkCarPositionIndicator, period);
        canInterface.registerTimeTriggered(networkCarLevelPosition);
        canInterface.registerTimeTriggered(networkDriveMsg);
        
        // Initialize physical state
        localCarPositionIndicator = CarPositionIndicatorPayload.getWriteablePayload();
        
        // Register payload with physical interface
        physicalInterface.sendTimeTriggered(localCarPositionIndicator, period);
        
        // Start the timer for the next iteration
        timer.start(period);
	}

	@Override
	/**
	 * timerExpired function, called on each period
	 */
	public void timerExpired(Object callbackData) {
		// Next state, default to current state
		State nextState = state;
		
		// Update current floor
		int floorBack = atFloorFrontTrue();
		int floorFront = atFloorBackTrue();
		
		// Case on the state
		switch (state) {
		
			// Traveling between floors
			case STATE_TRAVELING:
				// Commit point calculation
				double buffer = 0.0; // Distance buffer, in m
				// Position in m/s, using 1000 to convert from mm to m
				double x0 = ((double)mCarLevelPosition.getValue())/1000;
				double v0 = 0; // Speed in m/s
				double a0 = 0; // Acceleration in m/s
				// Initialize new next floor
				int newNextFloor = 0;
				
				// If going up
				if(mDrive.getSensorDirection() == Direction.UP) {
					 v0 = mDrive.getSensorSpeed(); 
					 // Set acceleration
					 a0 = -ACCEL_VAL;
				} else {	// If going down
					 v0 = -mDrive.getSensorSpeed();
					 a0 = ACCEL_VAL;
				}
				
				// Calculate time (tS) and distance (xS) to stop
				double tS = -v0/a0; 
				double xS = x0 + v0*(tS)+0.5*a0*(tS)*(tS) + buffer*v0;
				
				// If going up
				if(mDrive.getSensorDirection() == Direction.UP) {
					// Commit point should only increase. This reduces jitter if 
					// the elevator slows down faster than 1m/s/s
					newNextFloor = ((int) Math.ceil(xS/Elevator.DISTANCE_BETWEEN_FLOORS))+1;
					if(newNextFloor > nextFloor)
						nextFloor = newNextFloor;
				} 
				else {	// If going down
					newNextFloor = ((int) Math.floor(xS/Elevator.DISTANCE_BETWEEN_FLOORS))+1;
					if(newNextFloor < nextFloor)
						nextFloor = newNextFloor;
				}
				
				// Outputs
				if(nextFloor < 1)	// Guard against floor being less than first floor
					nextFloor = 1;
				if(nextFloor > Elevator.numFloors)	// Guard against floor being greater than last floor
					nextFloor = Elevator.numFloors;
				mCarPositionIndicator.set(nextFloor);
				localCarPositionIndicator.set(nextFloor);
								
				// Next state logic
                //#transition '9.T.2'
				if (mDrive.getSensorSpeed() <= simulator.elevatormodules.DriveObject.LevelingSpeed) {
					nextState = State.STATE_REACHED_FLOOR;
				} else {
					nextState = State.STATE_TRAVELING;
				}
				break;
				                               
			// Reached a floor
			case STATE_REACHED_FLOOR: 
				// Internal state
				if(floorFront != AT_NO_FLOOR)
					currentFloor = floorFront;
				else if(floorBack != AT_NO_FLOOR)
					currentFloor = floorBack;
				// Else, keep current floor the same, as transition will happen
				// Output
				mCarPositionIndicator.set(currentFloor);
				localCarPositionIndicator.set(currentFloor);
				
				// Next state logic
                //#transition '9.T.1'
				if (mDrive.getSensorSpeed() > simulator.elevatormodules.DriveObject.LevelingSpeed + LEVELING_MARGIN) {
					nextState = State.STATE_TRAVELING;
				} else {
					nextState = State.STATE_REACHED_FLOOR;
				}
				break;
			
			// If no states match, throw an exception. This should never happen.
			default:
                throw new RuntimeException("State " + state + " was not recognized.");
		}
		
        // Log the results of this iteration
        if (state == nextState) {
        	log("remains in state: ", state);
        } else {
        	log("Transition: ", state, " -> ", nextState);
        }
		
        // Update the state variable
        state = nextState;

        // Report the current state
        setState(STATE_KEY, nextState.toString());

        // Schedule the next iteration of the controller
        timer.start(period);
	}

	/**
	 * Helper function. See if any AtFloor is true (for front hallway), and if so, 
	 * return the floorNumber.
	 * @return which floor is true, or AT_NO_FLOOR if none
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
	 * Helper function. See if any AtFloor is true (for back hallway), and if so, 
	 * return the floorNumber.
	 * @return which floor is true, or AT_NO_FLOOR if none
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