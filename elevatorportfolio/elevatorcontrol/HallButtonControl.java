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
import simulator.elevatorcontrol.Utility.DoorClosedArray;
import simulator.elevatormodules.AtFloorCanPayloadTranslator;
import simulator.framework.Controller;
import simulator.framework.Direction;
import simulator.framework.Hallway;
import simulator.framework.ReplicationComputer;
import simulator.payloads.CanMailbox;
import simulator.payloads.HallCallPayload;
import simulator.payloads.HallLightPayload;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.HallCallPayload.ReadableHallCallPayload;
import simulator.payloads.HallLightPayload.WriteableHallLightPayload;


/**
 * HallButtonControl
 * Replication: floor, hallway, direction
 */
public class HallButtonControl extends Controller {
	
	/***************************************************************************
     * Declarations
     **************************************************************************/

	// Macro for desired direction
	private Direction DesiredDirection;
    // Local physical state
    private ReadableHallCallPayload localHallCall;
    private WriteableHallLightPayload localHallLight;
    
    // Network messages
    // Received network messages
    private ReadableCanMailbox networkAtFloor;
    private ReadableCanMailbox networkDesiredFloor;

    // Sent network messages
    private WriteableCanMailbox networkHallCall;
    
    // CAN translators
    private AtFloorCanPayloadTranslator mAtFloor;
    private DesiredFloorCanPayloadTranslator mDesiredFloor;
    private HallCallCanPayloadTranslator mHallCall;

    // Door closed sensors for this hallway
    DoorClosedArray doorClosedArray;
    DoorClosedArray doorClosedArrayFront;
    DoorClosedArray doorClosedArrayBack;
    
    // Keep track of which instance this is.
    private final Direction direction;
    private final int floor;
    
    // Store the period for the controller
    private SimTime period;
    
    // Enumerate states
    private enum State {
        STATE_NO_HALL_SELECTED,
        STATE_HALL_SELECTED,
        STATE_HALL_SERVICING
    }
    
    // Set initial state
    private State state = State.STATE_NO_HALL_SELECTED;

    
    /**
     * Constructor
     * @param floor this instances's floor
     * @param hallway this instances's hallway
     * @param direction this instances's direction
     * @param period period for this controller
     * @param verbose verbose mode
     */
	public HallButtonControl(int floor, Hallway hallway, Direction direction, SimTime period, boolean verbose) {
        // Call controller superclass
        super("HallButtonControl" + ReplicationComputer.makeReplicationString(floor, hallway, direction), verbose);
                
        // Set instance variables
        this.period = period;
        this.direction = direction;
        this.floor = floor;
        
        // Log that controller has been created
        log("Created HallButtonControl with period = ", period);
        
        // Initialize physical state
        localHallCall = HallCallPayload.getReadablePayload(floor, hallway, direction);
        // Register the payload with the physical interface
        physicalInterface.registerTimeTriggered(localHallCall);

        // Create a payload object for this floor,hallway,direction
        localHallLight = HallLightPayload.getWriteablePayload(floor, hallway, direction);
        
        // Register the payload to be sent periodically
        physicalInterface.sendTimeTriggered(localHallLight, period);
        
        // Initialize network interface
        networkAtFloor = CanMailbox.getReadableCanMailbox(
        		MessageDictionary.AT_FLOOR_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(floor, hallway));
        mAtFloor = new AtFloorCanPayloadTranslator(networkAtFloor, floor, hallway);
        
        // Register the mailbox to have its value broadcast on the network periodically
        canInterface.registerTimeTriggered(networkAtFloor);
        
        // Desired floor
        networkDesiredFloor = CanMailbox.getReadableCanMailbox(MessageDictionary.DESIRED_FLOOR_CAN_ID);
        mDesiredFloor = new DesiredFloorCanPayloadTranslator(networkDesiredFloor);
        canInterface.registerTimeTriggered(networkDesiredFloor);
        
        // Door closed array
        doorClosedArray = new DoorClosedArray(hallway, canInterface);
        doorClosedArrayFront = new DoorClosedArray(Hallway.FRONT, this.canInterface);
    	doorClosedArrayBack = new DoorClosedArray(Hallway.BACK, this.canInterface);
        
        // Network hall call
        networkHallCall = CanMailbox.getWriteableCanMailbox(
        		MessageDictionary.HALL_CALL_BASE_CAN_ID 
        		+ ReplicationComputer.computeReplicationId(floor, hallway, direction));
        mHallCall = new HallCallCanPayloadTranslator(networkHallCall, floor, hallway, direction);
        canInterface.sendTimeTriggered(networkHallCall, period);

        // Schedule first iteration of this controller
        timer.start(period);
	}
	
    /**
     * Timer callback
     */
	public void timerExpired(Object callbackData) {
		// New state, first set to current state
        State newState = state;
        
        // Case on the current state
        switch (state) {
        	// No hall is selected
            case STATE_NO_HALL_SELECTED:
                // State actions for 'NO HALL SELECTED'
                localHallLight.set(false);
                mHallCall.set(false);
                
                // Update desired direction
                DesiredDirection = desiredDirection(floor, mDesiredFloor.getFloor());
                
                // Transitions -- note that transition conditions are mutually exclusive
                //#transition '8.T.1'
                if (localHallCall.pressed() == true) {
                    newState = State.STATE_HALL_SELECTED;
                } 
                break;
                
            // Hall is selected, call has not been serviced
            case STATE_HALL_SELECTED:
                // State actions for 'HALL SELECTED'
                localHallLight.set(true);
                mHallCall.set(true);
                                
                // Update desired direction
                DesiredDirection = desiredDirection(floor, mDesiredFloor.getFloor());
                
                // Transitions -- note that transition conditions are mutually exclusive
                //#transition '8.T.2'
                if ((doorClosedArray.anyOpen()) && (mAtFloor.getValue() == true) 
                		&& (DesiredDirection == this.direction || DesiredDirection == Direction.STOP)) {
                    newState = State.STATE_HALL_SERVICING;
                }
                break;
                
             // Hall is selected, call has not been serviced
            case STATE_HALL_SERVICING:
                // State actions for 'HALL SELECTED'
                localHallLight.set(true);
                mHallCall.set(false);
                                
                // Do not update desired direction
                
                // Transitions -- note that transition conditions are mutually exclusive
                //#transition '8.T.3'
                if (doorClosedArrayFront.bothClosed() && doorClosedArrayBack.bothClosed()) {
                    newState = State.STATE_NO_HALL_SELECTED;
                }
                break;
                
            // Default case, no state matched, should never happen
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
	 * Get the desired direction
	 * @param CurrentFloor the floor of this button
	 * @param DesiredFloor the desired floor
	 * @return
	 */
	private Direction desiredDirection(int CurrentFloor, int DesiredFloor) {
		return mDesiredFloor.getDirection();
	}
}