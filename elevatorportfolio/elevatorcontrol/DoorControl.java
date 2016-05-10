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
import jSimPack.SimTime.SimTimeUnit;
import simulator.elevatormodules.AtFloorCanPayloadTranslator;
import simulator.elevatormodules.CarWeightCanPayloadTranslator;
import simulator.elevatormodules.DoorClosedCanPayloadTranslator;
import simulator.elevatormodules.DoorOpenedCanPayloadTranslator;
import simulator.elevatormodules.DoorReversalCanPayloadTranslator;
import simulator.framework.Controller;
import simulator.framework.Direction;
import simulator.framework.DoorCommand;
import simulator.framework.Elevator;
import simulator.framework.Hallway;
import simulator.framework.ReplicationComputer;
import simulator.framework.Side;
import simulator.payloads.CanMailbox;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.DoorMotorPayload;
import simulator.payloads.DoorMotorPayload.WriteableDoorMotorPayload;

/**
 * This DoorControl can be instantiated for any [hallway,side] combination.
 * Replication: side, hallway
 */
public class DoorControl extends Controller {
	// Constants
	private final static int AT_NO_FLOOR = -1;
	
	/***************************************************************************
     * Declarations
     **************************************************************************/
	
	// Local physical state: just door motor output
	private WriteableDoorMotorPayload localDoorMotor;
	
	// Receivers for CAN messages
    private ReadableCanMailbox networkDesiredDwell;
    private ReadableCanMailbox networkDesiredFloor;
    private ReadableCanMailbox networkDriveMsg;
    private ReadableCanMailbox networkCarWeight;
    private ReadableCanMailbox networkDoorOpened;
    private ReadableCanMailbox networkDoorClosed;
    private ReadableCanMailbox networkDoorReversalLeft;
    private ReadableCanMailbox networkDoorReversalRight;
    
    // ArrayList to store AtFloor sensor CAN messages
    ArrayList<ReadableCanMailbox> networkAtFloor = new ArrayList<>(Elevator.numFloors);

    // Translators for CAN receive messages
    private DesiredDwellCanPayloadTranslator mDesiredDwell;
    private DesiredFloorCanPayloadTranslator mDesiredFloor;
    private DriveCanPayloadTranslator mDrive;
    private CarWeightCanPayloadTranslator mCarWeight;
    private DoorOpenedCanPayloadTranslator mDoorOpened;
    private DoorClosedCanPayloadTranslator mDoorClosed;
    private DoorReversalCanPayloadTranslator mDoorReversalLeft;
    private DoorReversalCanPayloadTranslator mDoorReversalRight;

    // ArrayList to store AtFloor message translators
    ArrayList<AtFloorCanPayloadTranslator> mAtFloor = new ArrayList<>(Elevator.numFloors);

	// Transmitters for CAN messages
    private WriteableCanMailbox networkDoorMotorOut;

    // Translators for CAN transmit messages
    private DoorMotorCanPayloadTranslator mDoorMotor;
    
	// Enumerate states
    private enum State {
        STATE_DOOR_CLOSED,
        STATE_DOOR_OPENED,
        STATE_DOOR_REVERSAL,
        STATE_CLOSE_DOOR,
        STATE_OPEN_DOOR,
        STATE_NUDGE_DOOR
    }
    
    // State variable initialized to the initial state NUDGE_DOORS
    private State state = State.STATE_CLOSE_DOOR;
    
    // Current requested Dwell time
    private long dwell;
    
    // Current CountDown time
    private long countDown;
    
    // DoorHasReversed state variable
    private boolean doorHasReversed;
    
    // Variables to keep track of instance
	private final Hallway hallway;
	private final Side side;
	
	// Store the period for the controller
    private SimTime period;
    
    /**
     * Constructor for DoorControl.
     * @param hallway: the hallway this door is in
     * @param side: the side (left,right) this door is.
     * @param period: the period at which to run this task
     * @param verbose: verbose mode toggle
     * 
     */
	public DoorControl(Hallway hallway, Side side, SimTime period, boolean verbose) {
		// Call to the Controller superclass constructor is required
        super("DoorControl" + ReplicationComputer.makeReplicationString(hallway, side), verbose);
                
        // Store the constructor arguments in internal state
        this.period = period;
        this.hallway = hallway;
        this.side = side;
        
        // Log the creation of the door controller
        log("Created doorcontrol with period = ", this.period);
        
        ///////////// Initialize physical state ///////////////
        
        // Create a payload object for this hallway,side using the static factory method 
        // in DoorMotorPayload.
        localDoorMotor = DoorMotorPayload.getWriteablePayload(this.hallway, this.side);
        // Register the payload to be sent periodically
        physicalInterface.sendTimeTriggered(localDoorMotor, this.period);

        ///////////// Initialize network interface ///////////////
       
        // Transmit objects
        
        // Create a CAN mailbox for the DoorMotor message
        networkDoorMotorOut = CanMailbox.getWriteableCanMailbox(MessageDictionary.DOOR_MOTOR_COMMAND_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(this.hallway, this.side));
        
        // Create a translator for the DoorMotor message
        mDoorMotor = new DoorMotorCanPayloadTranslator(networkDoorMotorOut, this.hallway, this.side);
        
        // Register the mailbox to be sent periodically with given period.
        canInterface.sendTimeTriggered(networkDoorMotorOut, this.period);
        
        // Receive objects
        
        // Create CAN mailboxes for all received messages
        networkDesiredDwell = CanMailbox.getReadableCanMailbox(MessageDictionary.DESIRED_DWELL_CAN_ID);
        networkDesiredFloor = CanMailbox.getReadableCanMailbox(MessageDictionary.DESIRED_FLOOR_CAN_ID);
        networkDriveMsg = CanMailbox.getReadableCanMailbox(MessageDictionary.DRIVE_CAN_ID);
        networkCarWeight = CanMailbox.getReadableCanMailbox(MessageDictionary.CAR_WEIGHT_CAN_ID);
        networkDoorOpened = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_OPEN_SENSOR_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(this.hallway, this.side));
        networkDoorClosed = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(this.hallway, this.side));
        networkDoorReversalLeft = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_REVERSAL_SENSOR_BASE_CAN_ID
        		+ ReplicationComputer.computeReplicationId(this.hallway, Side.LEFT));
        networkDoorReversalRight = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_REVERSAL_SENSOR_BASE_CAN_ID
        		+ ReplicationComputer.computeReplicationId(this.hallway, Side.RIGHT));
      
        // Create CAN mailboxes for AtFloor received messages
        for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, this.hallway)) {
	        	networkAtFloor.add(floorNum - 1, CanMailbox.getReadableCanMailbox(
	        			MessageDictionary.AT_FLOOR_BASE_CAN_ID
	        			+ ReplicationComputer.computeReplicationId(floorNum, this.hallway)));
        	} else {
        		// Otherwise add null
        		networkAtFloor.add(floorNum - 1, null);
        	}
        }
        
        // Create translators for the received messages
        mDesiredDwell = new DesiredDwellCanPayloadTranslator(networkDesiredDwell);
        mDesiredFloor = new DesiredFloorCanPayloadTranslator(networkDesiredFloor);
        mDrive = new DriveCanPayloadTranslator(networkDriveMsg);
        mCarWeight = new CarWeightCanPayloadTranslator(networkCarWeight);
        mDoorOpened = new DoorOpenedCanPayloadTranslator(networkDoorOpened, this.hallway, this.side);
        mDoorClosed = new DoorClosedCanPayloadTranslator(networkDoorClosed, this.hallway, this.side);
        mDoorReversalLeft = new DoorReversalCanPayloadTranslator(networkDoorReversalLeft, this.hallway, Side.LEFT);
        mDoorReversalRight = new DoorReversalCanPayloadTranslator(networkDoorReversalRight, this.hallway, Side.RIGHT);
        
        // Create translators for AtFloor received messages
        for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
        	// Only add a floor if the elevator has a landing
        	if(Elevator.hasLanding(floorNum, this.hallway)) {
        		mAtFloor.add(floorNum - 1, new AtFloorCanPayloadTranslator(networkAtFloor.get(floorNum - 1), floorNum, this.hallway));
        	} else {
        		// Otherwise add null
        		mAtFloor.add(floorNum - 1, null);
        	}
        }
        
        // Register all receive messages to receive periodic updates, determined by sender
        canInterface.registerTimeTriggered(networkDesiredDwell);
        canInterface.registerTimeTriggered(networkDesiredFloor);
        canInterface.registerTimeTriggered(networkDriveMsg);
        canInterface.registerTimeTriggered(networkCarWeight);
        canInterface.registerTimeTriggered(networkDoorOpened);
        canInterface.registerTimeTriggered(networkDoorClosed);
        canInterface.registerTimeTriggered(networkDoorReversalLeft);
        canInterface.registerTimeTriggered(networkDoorReversalRight);

        // Register all AtFloor receive messages to receive periodic updates
        for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
        	ReadableCanMailbox floorNumAtFloor = networkAtFloor.get(floorNum - 1);
        	if(floorNumAtFloor != null) {
        		// Only register if this sensor exists
        		canInterface.registerTimeTriggered(floorNumAtFloor);
        	}
        }
        
        // Start the timer
        timer.start(this.period);
	}
	
	
	/**
	 * The code executed for each timer iteration. Executes current state actions
	 * and determines next state.
	 */
	public void timerExpired(Object callbackData) {
		// Next state to transition to, defaults to current state (no transition)
        State newState = state;
        
        // Switch on the current state
        switch (state) {
        	// State 1: Door is closed
            case STATE_DOOR_CLOSED:
            	// Outputs 
            	// DoorMotor = Stop
            	localDoorMotor.set(DoorCommand.STOP);
            	// mDoorMotor = DoorMotor
            	mDoorMotor.setCommand(localDoorMotor.command());
            	// Dwell = mDesiredDwell
            	
            	if(this.hallway == Hallway.FRONT)
            		dwell = mDesiredDwell.getDwellFront();
            	else
            		dwell = mDesiredDwell.getDwellBack();

            	// CountDown = 0
            	countDown = (long)0;
            	// DoorHasReversed = False
            	doorHasReversed = false;
            	
				// Next state logic
            	int f = atFloorTrue();
            	//#transition '5.T.2'
            	if (f != AT_NO_FLOOR && f == mDesiredFloor.getFloor() && (mDesiredFloor.getHallway() == this.hallway
            			|| mDesiredFloor.getHallway() == Hallway.BOTH)
            			&& (mDrive.getSensorSpeed() == 0 || mDrive.getSensorDirection() == Direction.STOP)) {
            		newState = State.STATE_OPEN_DOOR;
            	} else {
            		newState = State.STATE_DOOR_CLOSED;
            	}
            	break;    
            	
            // State 2: Door is opening    
            case STATE_OPEN_DOOR:
            	// Outputs 
            	// DoorMotor = Open
            	localDoorMotor.set(DoorCommand.OPEN);
            	// mDoorMotor = DoorMotor
            	mDoorMotor.setCommand(localDoorMotor.command());
            	// Dwell = mDesiredDwell
            	if(this.hallway == Hallway.FRONT)
            		dwell = mDesiredDwell.getDwellFront();
            	else
            		dwell = mDesiredDwell.getDwellBack();
            	// CountDown = Dwell
            	countDown = (long)(dwell / period.getFrac(SimTimeUnit.SECOND));
            	// DoorHasReversed = False
            	doorHasReversed = false;
            	
            	// Next state logic
            	//#transition '5.T.3'
            	if (mDoorOpened.getValue() == true) {
            		newState = State.STATE_DOOR_OPENED;
            	} else {
            		newState = State.STATE_OPEN_DOOR;
            	}
            	break;
                        
            // State 3: Door is opened
            case STATE_DOOR_OPENED:
            	// Outputs 
            	// DoorMotor = Stop
            	localDoorMotor.set(DoorCommand.STOP);
            	// mDoorMotor = DoorMotor
            	mDoorMotor.setCommand(localDoorMotor.command());
            	// Dwell = mDesiredDwell
            	if(this.hallway == Hallway.FRONT)
            		dwell = mDesiredDwell.getDwellFront();
            	else
            		dwell = mDesiredDwell.getDwellBack();
            	// CountDown = CountDown - 1
            	countDown = countDown - 1;
            	// DoorHasReversed = DoorHasReversed
            	// java throws warning when explicitly setting variable to itself
            	// so the comment is just here for completeness 
            	
            	// Next state logic
            	//#transition '5.T.4'
            	if (countDown <= 0 && doorHasReversed == false
            			&& mCarWeight.getWeight() < Elevator.MaxCarCapacity) {
            		newState = State.STATE_CLOSE_DOOR;
                //#transition '5.T.7'
            	} else if (countDown <= 0 && doorHasReversed == true
            			&& mCarWeight.getWeight() < Elevator.MaxCarCapacity) {
            		newState = State.STATE_NUDGE_DOOR;
            	} else {
            		newState = State.STATE_DOOR_OPENED;
            	}
            	break;
                     	
            // State 4: Door is closing (normal close)
            case STATE_CLOSE_DOOR:
            	// Outputs 
            	// DoorMotor = Close
            	localDoorMotor.set(DoorCommand.CLOSE);
            	// mDoorMotor = DoorMotor
            	mDoorMotor.setCommand(localDoorMotor.command());
            	// Dwell = mDesiredDwell
            	if(this.hallway == Hallway.FRONT)
            		dwell = mDesiredDwell.getDwellFront();
            	else
            		dwell = mDesiredDwell.getDwellBack();
            	// CountDown = 0
            	countDown = (long)0;
            	// DoorHasReversed = False
            	doorHasReversed = false;
            	            	
            	// Next state logic
            	//#transition '5.T.1'
            	if (mDoorClosed.getValue() == true) {
            		newState = State.STATE_DOOR_CLOSED;
                //#transition '5.T.5'
            	} else if (mDoorReversalLeft.getValue() == true || mDoorReversalRight.getValue() == true ||
            			mCarWeight.getWeight() >= Elevator.MaxCarCapacity) {
            		newState = State.STATE_DOOR_REVERSAL;
            	} else {
            		newState = State.STATE_CLOSE_DOOR;
            	}           	
            	break;
            	
            // State 5: Door reversal 
            case STATE_DOOR_REVERSAL:
            	// Outputs 
            	// DoorMotor = Open
            	localDoorMotor.set(DoorCommand.OPEN);
            	// mDoorMotor = DoorMotor
            	mDoorMotor.setCommand(localDoorMotor.command());
            	// Dwell = mDesiredDwell
            	if(this.hallway == Hallway.FRONT)
            		dwell = mDesiredDwell.getDwellFront();
            	else
            		dwell = mDesiredDwell.getDwellBack();
            	// CountDown = Dwell
            	countDown = (long)(dwell / period.getFrac(SimTimeUnit.SECOND));
            	// DoorHasReversed = True
            	doorHasReversed = true;
            	
            	// Next state logic
            	//#transition '5.T.6'
            	if (mDoorOpened.getValue() == true) {
            		newState = State.STATE_DOOR_OPENED;
            	} else {
            		newState = State.STATE_DOOR_REVERSAL;
            	}        	
            	break;            	            	
            	
            // State 6: Door is nudging close
            case STATE_NUDGE_DOOR:
            	// Outputs 
            	// DoorMotor = Nudge
            	localDoorMotor.set(DoorCommand.NUDGE);
            	// mDoorMotor = DoorMotor
            	mDoorMotor.setCommand(localDoorMotor.command());
            	// Dwell = mDesiredDwell
            	if(this.hallway == Hallway.FRONT)
            		dwell = mDesiredDwell.getDwellFront();
            	else
            		dwell = mDesiredDwell.getDwellBack();
            	// CountDown = Dwell
            	countDown = (long)0;
            	// DoorHasReversed = False
            	doorHasReversed = false;     	
            	
            	// Next state logic
            	//#transition '5.T.8'
            	if (mDoorClosed.getValue() == true) {
            		newState = State.STATE_DOOR_CLOSED;
            	}
            	//#transition '5.T.9'
            	else if (mCarWeight.getWeight() >= Elevator.MaxCarCapacity) {
            		newState = State.STATE_OPEN_DOOR;
            	} else {
            		newState = State.STATE_NUDGE_DOOR;
            	}
            	break;

            // No matching state. This should never happen
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
        timer.start(this.period);
	}
	
	/**
	 * Helper function. See if any AtFloor is true (for this hallway).
	 * @return the floor number, or AT_NO_FLOOR if not at a floor
	 */
	private int atFloorTrue() {
		for (int floorNum = 1; floorNum <= Elevator.numFloors; floorNum++) {
			// Return floorNum if true
			AtFloorCanPayloadTranslator floorNumAtFloor = mAtFloor.get(floorNum - 1);
			
			if (floorNumAtFloor != null && floorNumAtFloor.getValue()) {
				return floorNum;
			}
		}
		
		// If none were true, return AT_NO_FLOOR
		return AT_NO_FLOOR;
	}
}