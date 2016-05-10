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
import simulator.elevatormodules.AtFloorCanPayloadTranslator;
import simulator.elevatormodules.DoorClosedCanPayloadTranslator;
import simulator.framework.Controller;
import simulator.framework.Hallway;
import simulator.framework.ReplicationComputer;
import simulator.framework.Side;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.CanMailbox;
import simulator.payloads.CarCallPayload;
import simulator.payloads.CarCallPayload.ReadableCarCallPayload;
import simulator.payloads.CarLightPayload;
import simulator.payloads.CarLightPayload.WriteableCarLightPayload;

/**
 * Car button controller class
 * Replication: floor, hallway
 */
public class CarButtonControl extends Controller {
	
	// Physical Inputs
	private ReadableCarCallPayload localCarCall;
	
	// Physical Outputs
	private WriteableCarLightPayload localCarLight;
	
	/*
	 * For mDoorClosed
	 */
	
    // Receive mDoorClosed messages
    private ReadableCanMailbox networkDoorClosedLeft;
    private ReadableCanMailbox networkDoorClosedRight;
    
    // Translate mDoorClosed messages
    private DoorClosedCanPayloadTranslator mDoorClosedLeft; 
    private DoorClosedCanPayloadTranslator mDoorClosedRight; 
    
    /*
     * For mCarCall
     */
    
    private WriteableCanMailbox networkCarCallOut;
    private CarCallCanPayloadTranslator mCarCall;
    
    /*
     * For mAtFloor
     */
    
    private ReadableCanMailbox networkAtFloor;
    private AtFloorCanPayloadTranslator mAtFloor;
		
    // Store the period for the controller
    private SimTime period;
    
    // Keep track of which instance this is
    private final Hallway hallway;
    private final int floor;

    // State definitions
	private enum State {
		STATE_NO_FLOOR_SELECTED,
		STATE_FLOOR_SELECTED,
	}
	
	// Initialize to no floor selected state
	private State state = State.STATE_NO_FLOOR_SELECTED;

	/**
	 * Constructor
	 * @param floor The floor number for this instance
	 * @param hallway The hallway number for this instance
	 * @param period The run period for this instance
	 * @param verbose Verbose mode flag, controls logging
	 */
	public CarButtonControl(int floor, Hallway hallway, SimTime period,  boolean verbose) {		
        // Call superclass constructor
		super("CarButtonControl" + ReplicationComputer.makeReplicationString(floor, hallway), verbose);
				
        // Store the constructor arguments in internal state
        this.period = period;
        this.hallway = hallway;
        this.floor = floor;
        
        // Log creation of object
        log("Created CarButtonControl with period = ", period);
        
        // Initialize physical state
        localCarCall = CarCallPayload.getReadablePayload(this.floor, this.hallway);
        
        // Register payload with physical interface
        physicalInterface.registerTimeTriggered(localCarCall);
        
        // Initialized physical state
        localCarLight = CarLightPayload.getWriteablePayload(this.floor, this.hallway);
        
        // Register payload with physical interface
        physicalInterface.sendTimeTriggered(localCarLight, period);
        
        // Initialize network interface
        networkCarCallOut = CanMailbox.getWriteableCanMailbox(MessageDictionary.CAR_CALL_BASE_CAN_ID + ReplicationComputer.computeReplicationId(this.floor, this.hallway));
        
        // Create translator using generic bool translator
        mCarCall = new CarCallCanPayloadTranslator(networkCarCallOut, floor, hallway);
        
        // Register mailboxes
        canInterface.sendTimeTriggered(networkCarCallOut, period);
        
        // LeftDoorClosed
        networkDoorClosedLeft = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID + ReplicationComputer.computeReplicationId(this.hallway, Side.LEFT));
        mDoorClosedLeft = new DoorClosedCanPayloadTranslator(networkDoorClosedLeft, this.hallway, Side.LEFT);
        
        // RightDoorClosed
        networkDoorClosedRight = CanMailbox.getReadableCanMailbox(MessageDictionary.DOOR_CLOSED_SENSOR_BASE_CAN_ID + ReplicationComputer.computeReplicationId(this.hallway, Side.RIGHT));
        mDoorClosedRight = new DoorClosedCanPayloadTranslator(networkDoorClosedRight, this.hallway, Side.RIGHT);
        
        // Register to receive periodic updates to the mailbox via the CAN network
        // The period of updates will be determined by the sender of the message
        canInterface.registerTimeTriggered(networkDoorClosedLeft);
        canInterface.registerTimeTriggered(networkDoorClosedRight);
        
        // mAtFloor
        networkAtFloor = CanMailbox.getReadableCanMailbox(MessageDictionary.AT_FLOOR_BASE_CAN_ID + ReplicationComputer.computeReplicationId(this.floor, this.hallway));
        mAtFloor = new AtFloorCanPayloadTranslator(networkAtFloor, this.floor, this.hallway);
        
        // Register mailbox for mAtFloor
        canInterface.registerTimeTriggered(networkAtFloor);
        
        // Start the timer, for expiration after period
        timer.start(period);
	}
	
	@Override
	/**
	 * Timer expired function, called at each period
	 */
	public void timerExpired(Object callbackData) {
		// Next state to transition to, defaults to current state (no transition)
		State nextState = state;
		
		// Case on the current state
		switch (state) {
			// No floor selected
			case STATE_NO_FLOOR_SELECTED: 
				// Output
				localCarLight.set(false);
				mCarCall.set(false);
												
				// Next state logic
                //#transition '9.T.1'
				if (localCarCall.pressed() == true) {
					nextState = State.STATE_FLOOR_SELECTED;
				} else {
					nextState = State.STATE_NO_FLOOR_SELECTED;
				}
				break;	
				
			// Some floor selected
			case STATE_FLOOR_SELECTED: 
				// Output
				localCarLight.set(true);
				mCarCall.set(true);
				
				// Next state logic
                //#transition '9.T.2'
				if ((mDoorClosedLeft.getValue() == false || mDoorClosedRight.getValue() == false) && mAtFloor.getValue() == true ) {
					nextState = State.STATE_NO_FLOOR_SELECTED;
				} else {
					nextState = State.STATE_FLOOR_SELECTED;
				}
				break;
				
				
			// Not one of the defined states, shouldn't happen
			default:
                throw new RuntimeException("State " + state + " was not recognized.");
		}
		
        // Log the results of this iteration
        if (state == nextState) {
            log("remains in state: ",state);
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
}