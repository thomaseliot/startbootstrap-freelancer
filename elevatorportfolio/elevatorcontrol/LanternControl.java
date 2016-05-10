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
import simulator.elevatormodules.DoorClosedCanPayloadTranslator;
import simulator.framework.Controller;
import simulator.framework.Direction;
import simulator.framework.Elevator;
import simulator.framework.Hallway;
import simulator.framework.ReplicationComputer;
import simulator.framework.Side;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.CanMailbox;
import simulator.payloads.CarLanternPayload;
import simulator.payloads.CarLanternPayload.WriteableCarLanternPayload;

/**
 * LanternControl, replicated {Up, Down}
 */
public class LanternControl extends Controller {	
	// Local outputs
	private WriteableCarLanternPayload localCarLantern;
	
	// Receivers for CAN messages
    private ReadableCanMailbox networkDesiredFloor;
    private ReadableCanMailbox networkDoorClosedFrontLeft;
    private ReadableCanMailbox networkDoorClosedFrontRight;
    private ReadableCanMailbox networkDoorClosedBackLeft;
    private ReadableCanMailbox networkDoorClosedBackRight;
    
    // ArrayList to store AtFloor sensor CAN messages
    ArrayList<ReadableCanMailbox> networkAtFloorFront = new ArrayList<>(Elevator.numFloors);
    ArrayList<ReadableCanMailbox> networkAtFloorBack = new ArrayList<>(Elevator.numFloors);
  
    // Translators for CAN receive messages
    private DesiredFloorCanPayloadTranslator mDesiredFloor;
    private DoorClosedCanPayloadTranslator mDoorClosedFrontLeft;
    private DoorClosedCanPayloadTranslator mDoorClosedFrontRight;
    private DoorClosedCanPayloadTranslator mDoorClosedBackLeft;
    private DoorClosedCanPayloadTranslator mDoorClosedBackRight;

    // ArrayList to store AtFloor message translators
    ArrayList<AtFloorCanPayloadTranslator> mAtFloorFront = new ArrayList<>(Elevator.numFloors);
    ArrayList<AtFloorCanPayloadTranslator> mAtFloorBack = new ArrayList<>(Elevator.numFloors);

    
	// Transmitters for CAN messages
    private WriteableCanMailbox networkCarLanternOut;

    // Translators for CAN transmit messages
    private CarLanternCanPayloadTranslator mCarLantern;
    
    
	// Enumerate states
    private enum State {
    	STATE_DOOR_CLOSED,
        STATE_LANTERN_OFF,
        STATE_LANTERN_ON
    }
    
    // State variable initialized to the initial state DOOR_CLOSED
    private State state = State.STATE_DOOR_CLOSED;
    
    // Variables to keep track of instance
	private final Direction direction;
	
	// Store the period for the controller
    private SimTime period;

    Direction desiredDirection;
    
    /**
     * Constructor
     * @param direction this instance's direction
     * @param period the period for this controller
     * @param verbose verbose mode
     */
	public LanternControl(Direction direction, SimTime period,  boolean verbose) {
        // Call to superclass constructor
		super("LanternControl" + ReplicationComputer.makeReplicationString(direction), verbose);
				
        // Store the constructor arguments in internal state
        this.period = period;
        this.direction = direction;
        
        // Log creation of object
        log("Created LanternControl with period = ", period);
        
        ///////////// Initialize physical state ///////////////
        
        // Create a payload object for this direction
        localCarLantern = CarLanternPayload.getWriteablePayload(direction);
        // Register the payload to be sent periodically
        physicalInterface.sendTimeTriggered(localCarLantern, period);
        

        ///////////// Initialize network interface ///////////////
       
        // Transmit objects
        
        // Create a CAN mailbox for the CarLantern message
        networkCarLanternOut = CanMailbox.getWriteableCanMailbox(MessageDictionary.CAR_LANTERN_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(direction));
        
        // Create a translator for the Carlantern message
        mCarLantern = new CarLanternCanPayloadTranslator(networkCarLanternOut, direction);
        
        // Register the mailbox to be sent periodically with given period.
        canInterface.sendTimeTriggered(networkCarLanternOut, period);
        
        
        // Receive objects
        
        // Create CAN mailboxes for all received messages
        networkDesiredFloor = CanMailbox.getReadableCanMailbox(MessageDictionary.DESIRED_FLOOR_CAN_ID);
        networkDoorClosedFrontLeft = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(Hallway.FRONT, Side.LEFT));
        networkDoorClosedFrontRight = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(Hallway.FRONT, Side.RIGHT));
        networkDoorClosedBackLeft = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(Hallway.BACK, Side.LEFT));
        networkDoorClosedBackRight = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(Hallway.BACK, Side.RIGHT));
      
        // Create CAN mailboxes for AtFloor received messages
        for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
        	// Front Hallway
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, Hallway.FRONT)) {
            	networkAtFloorFront.add(floorNum - 1, CanMailbox.getReadableCanMailbox(
            			MessageDictionary.AT_FLOOR_BASE_CAN_ID
            			+ ReplicationComputer.computeReplicationId(floorNum, Hallway.FRONT)));
        	} else {
        		// Otherwise add null
        		networkAtFloorFront.add(floorNum - 1, null);
        	}
        	// Back Hallway
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, Hallway.BACK)) {
            	networkAtFloorBack.add(floorNum - 1, CanMailbox.getReadableCanMailbox(
            			MessageDictionary.AT_FLOOR_BASE_CAN_ID
            			+ ReplicationComputer.computeReplicationId(floorNum, Hallway.BACK)));
        	} else {
        		// Otherwise add null
        		networkAtFloorBack.add(floorNum - 1, null);
        	}
        }
        
        
        // Create translators for the received messages
        mDesiredFloor = new DesiredFloorCanPayloadTranslator(networkDesiredFloor);
        mDoorClosedFrontLeft = new DoorClosedCanPayloadTranslator(networkDoorClosedFrontLeft, Hallway.FRONT, Side.LEFT);
        mDoorClosedFrontRight = new DoorClosedCanPayloadTranslator(networkDoorClosedFrontRight, Hallway.FRONT, Side.RIGHT);
        mDoorClosedBackLeft = new DoorClosedCanPayloadTranslator(networkDoorClosedBackLeft, Hallway.BACK, Side.LEFT);
        mDoorClosedBackRight = new DoorClosedCanPayloadTranslator(networkDoorClosedBackRight, Hallway.BACK, Side.RIGHT);
     
        // Create translators for AtFloor received messages for front and back hallway
        for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
        	//Front Hallway
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, Hallway.FRONT)) {
        		mAtFloorFront.add(floorNum - 1, new AtFloorCanPayloadTranslator(networkAtFloorFront.get(floorNum - 1), floorNum, Hallway.FRONT));
        	} else {
        		// Otherwise add null
        		mAtFloorFront.add(floorNum - 1, null);
        	}
        	//Back Hallway
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, Hallway.BACK)) {
        		mAtFloorBack.add(floorNum - 1, new AtFloorCanPayloadTranslator(networkAtFloorBack.get(floorNum - 1), floorNum, Hallway.BACK));
        	} else {
        		// Otherwise add null
        		mAtFloorBack.add(floorNum - 1, null);
        	}
        }
        
        // Register all receive messages to receive periodic updates, determined by sender
        canInterface.registerTimeTriggered(networkDesiredFloor);
        canInterface.registerTimeTriggered(networkDoorClosedFrontLeft);
        canInterface.registerTimeTriggered(networkDoorClosedFrontRight);
        canInterface.registerTimeTriggered(networkDoorClosedBackLeft);
        canInterface.registerTimeTriggered(networkDoorClosedBackRight);

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
        
        // Start the timer
        timer.start(period);
	}

	@Override
	/**
	 * Timer expired, called every period
	 */
	public void timerExpired(Object callbackData) {
		State newState = state;
		
		switch (state) {
			case STATE_DOOR_CLOSED:
				// Output
				// All lanterns off
				localCarLantern.set(false);
				mCarLantern.set(false);
				// Update desired direction
				desiredDirection = mDesiredFloor.getDirection();

				// Next state logic
                //#transition '7.T.1'
				if(!allDoorsClosed() && desiredDirection == this.direction) {
					// If all doors are not closed and the desired direction
					// is the direction of this CarLantern object, then turn 
					// lantern on.
					newState = State.STATE_LANTERN_ON;
				}
				
				//#transition '7.T.2'
				if(!allDoorsClosed() && desiredDirection != this.direction) {
					// If all doors are not closed and the desired direction
					// is not the direction of this CarLantern object, then
					// don't turn the lantern on.
					newState = State.STATE_LANTERN_OFF;
				}
				break;
			case STATE_LANTERN_OFF: 
				// Output
				// All lanterns off
				localCarLantern.set(false);
				mCarLantern.set(false);
				// desiredDirection unchanged

				// Next state logic
                //#transition '7.T.3'
				if(allDoorsClosed()) {
					// If all doors are not closed and the desired direction
					// is the direction of this CarLantern object, then turn 
					// lantern on.
					newState = State.STATE_DOOR_CLOSED;
				}
				
				break;	
			case STATE_LANTERN_ON: 
				// Output
				// Turn CarLantern on
				localCarLantern.set(true);
				// Set CarLanter message to on
				mCarLantern.set(true);
				
				// Next state logic
				//#transition '7.T.4'
				if(allDoorsClosed()) {
					// If all doors are  closed, then go to lantern off state
					newState = State.STATE_DOOR_CLOSED;
				}
				
				break;
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
        setState(STATE_KEY, newState.toString());

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
}