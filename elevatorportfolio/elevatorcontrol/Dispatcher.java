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

import jSimPack.SimTime;
import jSimPack.SimTime.SimTimeUnit;
import simulator.elevatorcontrol.Utility.AtFloorArray;
import simulator.elevatorcontrol.Utility.CarCallArray;
import simulator.elevatorcontrol.Utility.DoorClosedArray;
import simulator.elevatorcontrol.Utility.HallCallArray;
import simulator.framework.Controller;
import simulator.framework.Direction;
import simulator.framework.Elevator;
import simulator.framework.Hallway;
import simulator.payloads.CanMailbox;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.translators.IntegerCanPayloadTranslator;


/**
 * Dispatcher controller
 * No replication
 */
public class Dispatcher extends Controller {
	
	/**************************************************************************
    * Declarations
    **************************************************************************/
	
    // Note that inputs are Readable objects, while outputs are Writeable objects
	private final byte dwelltime = 4; // seconds
	private final int delaytime = 100; // ms
	int countDown = 0;
    
    // Received network messages
	DoorClosedArray doorClosedArrayFront;
	DoorClosedArray doorClosedArrayBack;
	AtFloorArray atFloorArray;
	CarCallArray carCallArray;
	HallCallArray hallCallArray;

    private ReadableCanMailbox networkCarPositionIndicator;

    // Sent network messages
    private WriteableCanMailbox networkDesiredFloor;
    private WriteableCanMailbox networkDesiredDwell;
    
    // CAN translators
    // ArrayList to store AtFloor message translators
    private DesiredFloorCanPayloadTranslator mDesiredFloor;
    private DesiredDwellCanPayloadTranslator mDesiredDwell;
    private IntegerCanPayloadTranslator mCarPositionIndicator;


    // Initialize State variables
    private int target = 1;	// First floor
    private Direction desiredDirection = Direction.STOP;
    private Hallway hallway = Hallway.NONE;
      
    
    // Store the period for the controller
    private SimTime period;
    
    // Enumerate states:
    private enum State {
        STATE_UP,
        STATE_DOWN,
        STATE_WAIT,
        STATE_DOORSOPEN,
        STATE_HALLWAYLOCKED,
        STATE_DOORSCLOSED
    }
    
    // Set initial state
    private State state = State.STATE_WAIT;
    
    
    /**************************************************************************
     * Functions
     **************************************************************************/

    /**
     * Constructor
     * @param numFloors the number of floors
     * @param period the period to run at
     * @param verbose verbose mode
     */
	public Dispatcher(int numFloors, SimTime period, boolean verbose) {
        // Call superclass constructor
        super("Dispatcher", verbose);
        
        // Set internal period
        this.period = period;
        
        // Log creation of dispatcher
        log("Created dispatcher with period = ", period);
        
    	doorClosedArrayFront = new DoorClosedArray(Hallway.FRONT, this.canInterface);
    	doorClosedArrayBack = new DoorClosedArray(Hallway.BACK, this.canInterface);
    	atFloorArray = new AtFloorArray(this.canInterface);
    	carCallArray = new CarCallArray(this.canInterface);
    	hallCallArray = new HallCallArray(this.canInterface);
        
        // Initialize network interface  

        //mDesiredFloor
        networkDesiredFloor = CanMailbox.getWriteableCanMailbox(MessageDictionary.DESIRED_FLOOR_CAN_ID);
        //mDesiredDwell
        networkDesiredDwell = CanMailbox.getWriteableCanMailbox(
        		MessageDictionary.DESIRED_DWELL_CAN_ID);
 	   	// Car position indicator
	    networkCarPositionIndicator = CanMailbox.getReadableCanMailbox(
	    		MessageDictionary.CAR_POSITION_CAN_ID);


        // Create network translators

        // Desired Floor
        mDesiredFloor = new DesiredFloorCanPayloadTranslator(networkDesiredFloor);
        // Desired Dwell
        mDesiredDwell = new DesiredDwellCanPayloadTranslator(networkDesiredDwell);

	    mCarPositionIndicator = new IntegerCanPayloadTranslator(networkCarPositionIndicator);
        
        // Register periodic broadcast mailboxes
        canInterface.sendTimeTriggered(networkDesiredFloor, period);
        canInterface.sendTimeTriggered(networkDesiredDwell, period);
        
	    canInterface.registerTimeTriggered(networkCarPositionIndicator);
        
        // Start the timer, callback in one period
        timer.start(period);
	}
	
    /**
     * Timer callback, called with rate of period
     */
	public void timerExpired(Object callbackData) {
    	int currentFloor = mCarPositionIndicator.getValue();
		// New state
        State newState = state;
        
        // Case on current state
        switch (state) {
        	// State 1: Waiting for Call
        	case STATE_WAIT:
        		target = currentFloor;
        		desiredDirection = Direction.STOP;
        		hallway = Hallway.NONE;
        		
        		//#transition '11.T.1'
        		if (existCalls(currentFloor, Elevator.numFloors))
        			newState = State.STATE_UP;
        		
        		//#transition '11.T.3'
        		else if (existCalls(1,currentFloor-1))
        			newState = State.STATE_DOWN;
        		
        		
        		// Set output interface
        		mDesiredFloor.set(target, Direction.STOP, Hallway.NONE);
        		mDesiredDwell.setDwellFront(dwelltime);
        		mDesiredDwell.setDwellBack(dwelltime);
        		break;
        		
        	// State 2: Up
        	case STATE_UP:
        		// Set target
        		if(existHC(currentFloor, Elevator.numFloors, Direction.UP) 
        				|| existCC(currentFloor, Elevator.numFloors))
        			for (int i = target; i <= Elevator.numFloors; i++) {
        				if(carCallArray.getCall(i, Hallway.FRONT) 
        						|| carCallArray.getCall(i,  Hallway.BACK)) {
        					target = i;
        					break;
        				}
        				if((hallCallArray.getCall(i, Hallway.FRONT, Direction.UP) 
        						|| hallCallArray.getCall(i, Hallway.BACK, Direction.UP))) { 
        					target = i;
        					break;
        				}
        			}
        		else if(existHC(currentFloor, Elevator.numFloors, Direction.DOWN))
        			for (int i = Elevator.numFloors; i >= target; i--) {
        				if(
        						(hallCallArray.getCall(i, Hallway.FRONT, Direction.DOWN) || //does not account for no sensor existing on UP, 8. should be ok.
        					hallCallArray.getCall(i, Hallway.BACK, Direction.DOWN))) { //desiredDirection should never be NONE
        					target = i;
        					break;
        				}
        			}
        		else {
        			target = currentFloor;
        		}
        		
        		// Set desired direction
        		if(target == Elevator.numFloors)
        			desiredDirection = Direction.DOWN;
        		else if(existHC(target, target, Direction.UP) || existCalls(target+1, Elevator.numFloors))
        				desiredDirection = Direction.UP;
        		else if(existHC(target, target, Direction.DOWN) || existCalls(1, target-1))
        			desiredDirection = Direction.DOWN;
        		else
        			desiredDirection = Direction.UP;	// Set to current direction
        				
        		
        		// Set hallway
        		hallway = getHallway(currentFloor);
        		
        		// Set output interface
        		mDesiredFloor.set(target, desiredDirection, hallway);
        		mDesiredDwell.setDwellFront(dwelltime);
        		mDesiredDwell.setDwellBack(dwelltime);
        		
        		
                //#transition '11.T.2'
        		if(!existCalls(currentFloor, Elevator.numFloors))
        			newState = State.STATE_WAIT;
        		
                //#transition '11.T.5'
        		else if(hallway != Hallway.NONE)
        			newState = State.STATE_HALLWAYLOCKED;
        		
        		else
        			newState = state;
        		
        		break;
        		
        	//State 3: Down	
        	case STATE_DOWN:	
        		// Set target
        		if(existHC(1,currentFloor, Direction.DOWN) || existCC(1, currentFloor)) {
        			for (int i = target; i >= 1; i--) {
        				if(carCallArray.getCall(i, Hallway.FRONT) 
        						|| carCallArray.getCall(i,  Hallway.BACK)) {
        					target = i;
        					break;
        				}
        				if((hallCallArray.getCall(i, Hallway.FRONT, Direction.DOWN) ||
        					hallCallArray.getCall(i, Hallway.BACK, Direction.DOWN))) {
        					target = i;
        					break;
        				}
        			}
        		}
        		else if(existHC(1, currentFloor, Direction.UP)) {
        			for (int i = 1; i <= target; i++) {
        				if((hallCallArray.getCall(i, Hallway.FRONT, Direction.UP) 
        						|| hallCallArray.getCall(i, Hallway.BACK, Direction.UP))) { 
        					target = i;
        					break;
        				}
        			}
        		}
        		else {
        			target = currentFloor;
        		}
        		
        		// Set desired direction
        		if(target == 1)
        			desiredDirection = Direction.UP;
        		else if(existHC(target, target, Direction.DOWN) || existCalls(1, target-1))
        				desiredDirection = Direction.DOWN;
        		else if(existHC(target, target, Direction.UP) || existCalls(target+1, Elevator.numFloors))
        			desiredDirection = Direction.UP;
        		else
        			desiredDirection = Direction.DOWN;
        		
        		// Set hallway
        		hallway = getHallway(currentFloor);
        		
        		// Set output interface
        		mDesiredFloor.set(target, desiredDirection, hallway);
        		mDesiredDwell.setDwellFront(dwelltime);
        		mDesiredDwell.setDwellBack(dwelltime);
        		
        		// Transitions
        		//#transition '11.T.4'
        		if(!existCalls(1,currentFloor))
        			newState = State.STATE_WAIT;
        		//#transition '11.T.6'
        		else if(hallway != Hallway.NONE)
        			newState = State.STATE_HALLWAYLOCKED;
        		else
        			newState = state;
        		break;

        	// State 4: Hallway Locked	
        	case STATE_HALLWAYLOCKED:
        		// Set desired floor and desired dwell
        		mDesiredFloor.set(target, desiredDirection, hallway);
        		mDesiredDwell.setDwellFront(dwelltime);
        		mDesiredDwell.setDwellBack(dwelltime);
        		
        		//#transition '11.T.7'
        		if(allDoorsClosed() == false)
        			newState = State.STATE_DOORSOPEN;
        		else
        			newState = state;
        		break;
        	
        	// State 5: Doors Open
        	case STATE_DOORSOPEN:
        		// Set target, hallway
        		target = currentFloor;
        		hallway = Hallway.NONE;
        		// Get countdown
        		countDown = (int)(delaytime / period.getFrac(SimTimeUnit.MILLISECOND));

        		// Set desired floor and desired dwell
        		mDesiredFloor.set(target, desiredDirection, hallway);
        		mDesiredDwell.setDwellFront(dwelltime);
        		mDesiredDwell.setDwellBack(dwelltime);
        		
        		//#transition '11.T.8'
        		if(allDoorsClosed() == true)
        			newState = State.STATE_DOORSCLOSED;
        		else 
        			newState = state;
        		break;
        		
        	// State 6: Doors Closed
        	case STATE_DOORSCLOSED:
        		// Set target, hallway
        		target = currentFloor;
        		hallway = Hallway.NONE;
        		// Decrement countdown
        		countDown = countDown - 1;
        		
        		// Set desired floor and desired dwell
        		mDesiredFloor.set(target, desiredDirection, hallway);
        		mDesiredDwell.setDwellFront(dwelltime);
        		mDesiredDwell.setDwellBack(dwelltime);
        		
        		//#transition '11.T.9'
        		if(countDown == 0 && desiredDirection == Direction.UP)
        			newState = State.STATE_UP;
        		//#transition '11.T.10'
        		else if(countDown == 0 && desiredDirection == Direction.DOWN)
        			newState = State.STATE_DOWN;
        		else
        			newState = state;
        		break;
        		
        	// This should never happen
            default:
                throw new RuntimeException("State " + state + " was not recognized.");
        }
        
        // Log the results of this iteration
        if (state == newState) {
            log("remains in state: ", state);
        } else {
        	log("Transition:", state, "->", newState);
        }
        

        // Update the state variable
        state = newState;

        // Report the current state
        setState(STATE_KEY, newState.toString());

        // Schedule the next iteration of the controller
        timer.start(this.period);
    }
	
	/**
	 * Helper function to see if there exist hall calls between two floors in a
	 * direction
	 * @param floor1 start floor
	 * @param floor2 end floor
	 * @param direction direction
	 * @return if there are calls
	 */
	private boolean existHC(int floor1, int floor2, Direction direction) {
		// Guard against bad inputs
		if (floor1 < 1 || floor1 > Elevator.numFloors 
				|| floor2 < 1 || floor2 > Elevator.numFloors)
			return false;
		// Swap floors to allow them to be passed in in either order
		if(floor1 > floor2) {
			int swap = floor1;
			floor1 = floor2;
			floor2 = swap;
		}
		// Check all hall calls between floor1 and floor2
		for(int floorNum = floor1; floorNum <= floor2; floorNum++){
			if(hallCallArray.getCall(floorNum, Hallway.FRONT, direction) ||
					hallCallArray.getCall(floorNum, Hallway.BACK, direction)) {
				return true;				
			}
		}
		// Return false if none found
		return false;
	}
	
	/**
	 * Helper function to see if there exist car calls between two floors
	 * @param floor1 start floor
	 * @param floor2 end floor
	 * @return if there exist any calls
	 */
	private boolean existCC(int floor1, int floor2) {
		if (floor1 < 1 || floor1 > Elevator.numFloors 
				|| floor2 < 1 || floor2 > Elevator.numFloors)
			return false;
		// Swap floors to allow them to be passed in in either order
		if(floor1 > floor2) {
			int swap = floor1;
			floor1 = floor2;
			floor2 = swap;
		}
		// Check car calls between floor1 and floor2
		for(int floorNum = floor1; floorNum <= floor2; floorNum++){
			if(carCallArray.getCall(floorNum, Hallway.FRONT) ||
					carCallArray.getCall(floorNum, Hallway.BACK)) {
				return true;				
			}
		}
		// If none found, return false
		return false;
	}
	
	/**
	 * See if there exist any calls (car or hall) between two floors
	 * @param floor1 start floor
	 * @param floor2 end floor
	 * @return if there exist any calls
	 */
	private boolean existCalls(int floor1, int floor2) {
		return(existCC(floor1, floor2) || existHC(floor1,floor2, Direction.UP)
				|| existHC(floor1,floor2,Direction.DOWN));
	}

	/**
	 * Helper function
	 * Checks if all four doors are closed.
	 * @return true if all doors closed
	 */
	private boolean allDoorsClosed(){
		return (doorClosedArrayFront.bothClosed() &&
				doorClosedArrayBack.bothClosed());
	}
	
	/**
	 * Get which hallways have calls at the current floor
	 * @return hallway, all hallways that the dispatcher is servicing
	 */
	private Hallway getHallway(int currentFloor) {
		// Return none if target is not equal to currentfloor
		if(target != currentFloor)
			return Hallway.NONE;
		
		// Get car calls at front and back
		boolean pickupFront = carCallArray.getCall(target, Hallway.FRONT);
		boolean pickupBack = carCallArray.getCall(target, Hallway.BACK);

		// If desiring a direction, get hall calls too
		if(desiredDirection != Direction.STOP) {
			pickupFront |= hallCallArray.getCall(target, Hallway.FRONT, desiredDirection);
			pickupBack |= hallCallArray.getCall(target, Hallway.BACK, desiredDirection);
		}
		
		// Return FRONT, BACK, BOTH, or NONE
		if(pickupFront && pickupBack)
			return Hallway.BOTH;
		else if(pickupFront)
			return Hallway.FRONT;
		else if(pickupBack)
			return Hallway.BACK;
		else
			return Hallway.NONE;
	}
}