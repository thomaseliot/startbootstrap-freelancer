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

import java.util.ArrayList;

import jSimPack.SimTime;
import jSimPack.SimTime.SimTimeUnit;
import simulator.framework.Direction;
import simulator.framework.DoorCommand;
import simulator.framework.Elevator;
import simulator.framework.Hallway;
import simulator.framework.Harness;
import simulator.framework.ReplicationComputer;
import simulator.framework.RuntimeMonitor;
import simulator.framework.Side;
import simulator.framework.Speed;
import simulator.payloads.CanMailbox;
import simulator.payloads.AtFloorPayload.ReadableAtFloorPayload;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CarCallPayload.ReadableCarCallPayload;
import simulator.payloads.CarLanternPayload.ReadableCarLanternPayload;
import simulator.payloads.CarWeightPayload.ReadableCarWeightPayload;
import simulator.payloads.DoorClosedPayload.ReadableDoorClosedPayload;
import simulator.payloads.DoorMotorPayload.ReadableDoorMotorPayload;
import simulator.payloads.DoorOpenPayload.ReadableDoorOpenPayload;
import simulator.payloads.DrivePayload.ReadableDrivePayload;
import simulator.payloads.DriveSpeedPayload.ReadableDriveSpeedPayload;
import simulator.payloads.HallCallPayload.ReadableHallCallPayload;

/**
 * Runtime Requirements Monitor
 * Monitors high-level requirements RT6-RT10 and reports violations of those
 * requirements during runtime.
 */
public class RuntimeRequirementsMonitor extends RuntimeMonitor {
	// At no floor constant value
	private static final int AT_NO_FLOOR = 0;
	
	// Network messages
	private ReadableCanMailbox[][][] networkHallCalls = new ReadableCanMailbox[Elevator.numFloors][2][2];
	private ReadableCanMailbox[][] networkCarCalls = new ReadableCanMailbox[Elevator.numFloors][2];
	private HallCallCanPayloadTranslator[][][] mHallCalls = new HallCallCanPayloadTranslator[Elevator.numFloors][2][2];
	private CarCallCanPayloadTranslator[][] mCarCalls = new CarCallCanPayloadTranslator[Elevator.numFloors][2];

	// Requirement check state machines
	DriveFastStateMachine driveFastState = new DriveFastStateMachine();
	StopPendingStateMachine stopPendingState = new StopPendingStateMachine();
	DoorsPendingStateMachine doorsPendingFrontState = new DoorsPendingStateMachine(Hallway.FRONT);
	DoorsPendingStateMachine doorsPendingBackState = new DoorsPendingStateMachine(Hallway.BACK);
	DoorsNudgeStateMachine frontDoorsNudgeStateMachine = new DoorsNudgeStateMachine(Hallway.FRONT);
	DoorsNudgeStateMachine backDoorsNudgeStateMachine = new DoorsNudgeStateMachine(Hallway.BACK);
	LanternOnCallsStateMachine lanternOnCallsState = new LanternOnCallsStateMachine();
	LanternDirectionStateMachine lanternDirectionState = new LanternDirectionStateMachine();
	LanternDriveStateMachine lanternDriveState = new LanternDriveStateMachine();

	// Utility state machines
    DoorStateMachine doorState = new DoorStateMachine();
    DriveStateMachine driveState = new DriveStateMachine();
    WeightStateMachine weightState = new WeightStateMachine();
    LanternStateMachine lanternState = new LanternStateMachine();
    LandingStateMachine landingState = new LandingStateMachine();
    CarCallStateMachine carCallState = new CarCallStateMachine();
    HallCallStateMachine hallCallState = new HallCallStateMachine();

    // Constants
    private final double maxSlowToStopDuration = 12000.0;	// Max duration while slowing, in ms
    private final double maxSlowToFastDuration = 1500.0;	// Max duration while speeding up from stop to slow, in ms

    // Violation counters
    boolean hasMoved = false;
    boolean wasOverweight = false;
    int overWeightCount = 0;
    int badOpeningCount = 0;
    int badStopCount = 0;
    int badDoorsCount = 0;
    int badDoorNudgeCount = 0;
    int badDriveFastCount = 0;
    int badLanternCallsCount = 0;
    int badLanternSwitchCount = 0;
    int badLanternDriveCount = 0;

    /**
     * Constructor
     */
    public RuntimeRequirementsMonitor() {
    	// Register messages
    	// Replication over floors
    	for (int i = 0; i < Elevator.numFloors; ++i) {
    		// Replication over hallway
            for (Hallway h : Hallway.replicationValues) {
            	// Floor index is one less than actual floor
                int floor = i + 1;
                
                // Create network car call
            	networkCarCalls[i][h.ordinal()] = CanMailbox.getReadableCanMailbox(
            			MessageDictionary.CAR_CALL_BASE_CAN_ID
            			+ ReplicationComputer.computeReplicationId(floor, h));
            	// Messages for car call
            	mCarCalls[i][h.ordinal()]
            			= new CarCallCanPayloadTranslator(networkCarCalls[i][h.ordinal()], floor, h);
            	// Register message
            	canInterface.registerTimeTriggered(networkCarCalls[i][h.ordinal()]);
                
            	// Replication over direction
                for (Direction d : Direction.replicationValues) {
                	// Create network hall call
                	networkHallCalls[i][h.ordinal()][d.ordinal()] = CanMailbox.getReadableCanMailbox(
                			MessageDictionary.HALL_CALL_BASE_CAN_ID
                			+ ReplicationComputer.computeReplicationId(floor, h, d));
                	// Messages for hall call
                	mHallCalls[i][h.ordinal()][d.ordinal()] 
                			= new HallCallCanPayloadTranslator(networkHallCalls[i][h.ordinal()][d.ordinal()], 
                					floor, h, d);
                	// Register message
                	canInterface.registerTimeTriggered(networkHallCalls[i][h.ordinal()][d.ordinal()]);
                }
            }
    	}
    }

    @Override
    /**
     * Create human-readable summary
     * @return summary
     */
    protected String[] summarize() {
        String[] arr = new String[8];
        arr[0] = "Overweight Count = " + overWeightCount;
        arr[1] = "Bad Stop Count = " + badStopCount;
        arr[2] = "Bad Door Openings Count = " + badDoorsCount;
        arr[3] = "Bad Door Nudge Count = " + badDoorNudgeCount;
        arr[4] = "Drive Not Fast Enough Count = " + badDriveFastCount;
        arr[5] = "Lantern Not On With Calls Count = " + badLanternCallsCount;
        arr[6] = "Lantern Switched Direciton Count = " + badLanternSwitchCount;
        arr[7] = "Dispatch opposite direction of lantern with calls count = " + badLanternDriveCount;
        return arr;
    }

    /**************************************************************************
     * high level event methods
     *
     * these are called by the logic in the message receiving methods and the
     * state machines
     **************************************************************************/

    /**
     * Called once when the doors close completely
     * @param hallway which door the event pertains to
     */
    private void doorClosed(Hallway hallway) {
        // Once all doors are closed, check to see if the car was overweight
        if (!doorState.anyDoorOpen()) {
            if (wasOverweight) {
                overWeightCount++;
                wasOverweight = false;
            }
        }
    }  

    /**
     * Called when the car weight changes
     * @param hallway which door the event pertains to
     */
    private void weightChanged(int newWeight) {
    	// If overweight, set flag
        if (newWeight > Elevator.MaxCarCapacity) {
            wasOverweight = true;
        }
    }

    /**************************************************************************
     * low level message receiving methods
     * 
     * These mostly forward messages to the appropriate state machines
     **************************************************************************/
    @Override
    public void receive(ReadableDoorClosedPayload msg) {
        doorState.receive(msg);
    }

    @Override
    public void receive(ReadableDoorOpenPayload msg) {
        doorState.receive(msg);
    }

    @Override
    public void receive(ReadableDoorMotorPayload msg) {
        doorState.receive(msg);
    }

    @Override
    public void receive(ReadableCarWeightPayload msg) {
        weightState.receive(msg);
    }

    @Override
    public void receive(ReadableDriveSpeedPayload msg) {
    	// Check if the drive has moved
        if (msg.speed() > 0) {
            hasMoved = true;
        }
    }
    
    @Override
    public void receive(ReadableAtFloorPayload msg) {
        // Call the update function for LandingState
    	landingState.receive(msg);
    }
    
    @Override
    public void receive(ReadableCarCallPayload msg) {
        // Call the update function for LandingState
    	carCallState.receive(msg);
    }
    
    @Override
    public void receive(ReadableHallCallPayload msg) {
        // Call the update function for LandingState
    	hallCallState.receive(msg);
    }
    
    @Override
    public void receive(ReadableDrivePayload msg) {
        // Call the update function for LandingState
    	driveState.receive(msg);
    }
    @Override
    public void receive(ReadableCarLanternPayload msg) {
        // Call the update function for LandingState
    	lanternState.receive(msg);
    }
    
    /*************************************************************************/
    /***************** REQUIREMENT CHECKING STATE MACHINES *******************/
    /*************************************************************************/
    
    // State enumeration for door state machine
    private static enum DoorState {
        CLOSED,
        OPENING,
        OPEN,
        CLOSING,
        NUDGING
    }
    
    // State enumeration for lanterns lit on calls state machine
    private static enum LanternCallsState {
        DOOR_CLOSED,
        DOOR_OPEN,
        ERROR
    }
    
    // State enumeration for lanterns not changing while doors open
    // state machine
    private static enum LanternDirectionState {
    	DOORS_CLOSED,
    	LANTERN_ON_UP,
    	LANTERN_ON_DOWN
    }
    
    // State enumeration for elevator going same direction as lantern
    // state machine
    private static enum LanternDriveState {
    	LANTERN_OFF,
    	DOWN_LANTERN_ON,
    	UP_LANTERN_ON,
    	DOWN_CALLS_LATCH,
    	UP_CALLS_LATCH,
    	GOING_DOWN,
    	GOING_UP
    }
    
    // State enumeration for lantern state
    private static enum LanternState {
    	LANTERN_OFF,
    	LANTERN_UP,
    	LANTERN_DOWN
    }
    
    // State enumeration for drive state
    private static enum DriveFastState {
        STOPPED,
        SLOW,
        FAST,
        SLOWING,
        ERROR
    }
    
    // State enumeration for a generic checker, which only has nominal and
    // error states
    private static enum GenericState {
    	NOMINAL,
    	ERROR
    }
    
    /*************************************************************************/
    /***************** REQUIREMENT CHECKING STATE MACHINES *******************/
    /*************************************************************************/
    
    /**
     * Class to make sure that the drive always goes as fast as possible.
     * Used for checking R-T9
     */
    private class DriveFastStateMachine {
    	// State, start in stopped
    	DriveFastState state = DriveFastState.STOPPED;
    	// State duration, for timing slow times
    	Stopwatch stateDuration;
    	
    	/**
    	 * Constructor
    	 */
    	public DriveFastStateMachine() {
    		// Construct the stop watch
    		stateDuration = new Stopwatch();
    	}
    	
    	/**
    	 * Update the current state
    	 */
    	public void updateState() {
    		// New state
    		DriveFastState nextState = state;
    		
    		// Get current drive speed
    		Speed currentSpeed = driveState.getSpeed();
    		
    		// State action and transitions
    		switch(state) {
    			case STOPPED:
    				// Transition T1
    				if(currentSpeed == Speed.SLOW) {
    					nextState = DriveFastState.SLOW;
    				}
    				break;
    				
    			case SLOW:
    				// Transition T2
    				if(currentSpeed == Speed.STOP) {
    					nextState = DriveFastState.STOPPED;
    				}
    				// Transition T4
    				if(currentSpeed == Speed.FAST) {
    					nextState = DriveFastState.FAST;
    				}
    				break;
    				
    			case FAST:
    				// Transition T5
    				if(currentSpeed == Speed.SLOW) {
    					nextState = DriveFastState.SLOWING;
    				}
    				break;
    				
				case SLOWING:
					// Transition T7
					if(currentSpeed == Speed.STOP) {
    					nextState = DriveFastState.STOPPED;
    				}
    				break;
    				
				case ERROR:
					// Transition T8
					if(currentSpeed == Speed.STOP) {
    					nextState = DriveFastState.STOPPED;
    				}
    				break;
    			
    			default:
    				throw new RuntimeException("DriveFastStateMachine in illegal state");
    		}
    		
    		// Start timer if transitioning to either SLOW or SLOWING state
    		if((nextState == DriveFastState.SLOW || nextState == DriveFastState.SLOWING) && nextState != state) {
    			stateDuration.start();
    		}
    		
    		// If transitioning from SLOW or SLOWING to any state, check duration in slow to see if error
    		// Transitions T3, T6
    		else if((state == DriveFastState.SLOW || state == DriveFastState.SLOWING) && nextState != state) {
    			// Stop the stopwatch and check time
    			stateDuration.stop();
    			SimTime slowTime = stateDuration.getAccumulatedTime();
    			
    			// See if greater than allowable slow time
    			if((state == DriveFastState.SLOW 
    					&& slowTime.getFrac(SimTimeUnit.MILLISECOND) > maxSlowToFastDuration)
    					|| (state == DriveFastState.SLOWING 
    					&& slowTime.getFrac(SimTimeUnit.MILLISECOND) > maxSlowToStopDuration)) {
    				// If it is, then transition into error state instead
    				message("Slowtime = " + slowTime.getFrac(SimTimeUnit.MILLISECOND));
    				nextState = DriveFastState.ERROR;
    			}
    			
    			// Reset the stopwatch
    			stateDuration.resetAccumulated();
    		}
    		
    		// If transitioning from SLOWING to FAST state, then violation
    		// Transition T6
    		else if(state == DriveFastState.SLOWING && nextState == DriveFastState.FAST) {
    			nextState = DriveFastState.ERROR;
    		}
    		
    		// Report error if transitioning to error state
    		if(nextState == DriveFastState.ERROR && nextState != state) {
    			warning("R-T.9 Violated: Drive not commanded to max speed to maximum degree practicable.");
    			badDriveFastCount++;
    		}
    		
    		// Print out the transition
    		if(nextState != state) {
    		}
    		
    		// Transition state
    		state = nextState;
    	}
    }
    
    /**
     * Class to keep track of stopping at floors where there are pending calls
     * Used for checking R-T6
     */
    private class StopPendingStateMachine {
    	// State, start in nominal
    	GenericState state = GenericState.NOMINAL;
    	// Started, to get rid of some startup transients
    	private boolean started = false;
    	
    	/**
    	 * Update the current state
    	 */
    	public void updateState() {
    		// New state
    		GenericState nextState = state;
    		
    		// Get current floor, current speed
    		int currentFloor = landingState.getFloor();
    		Speed currentSpeed = driveState.getSpeed();
    		
    		// If speed is not STOP, then start monitoring
    		if(!started && currentSpeed == Speed.SLOW) started = true; 
    		
    		switch(state) {
    			case NOMINAL:
    				// Error transition
    				// Must be at floor and stopped with no car/hall buttons pressed
    	    		if(currentFloor != 0 && currentSpeed == Speed.STOP 
    	    				&& !carCallState.anyPressedOnFloor(currentFloor)
    	    				&& !hallCallState.anyPressedOnFloor(currentFloor)
    	    				&& started) {
    	    			// Stopped with no buttons pressed, go to error state
    	    			nextState = GenericState.ERROR;
    	    		}
    				break;
    				
    			case ERROR:
    				// Nominal transition
    				if(currentSpeed != Speed.STOP && currentSpeed != Speed.LEVEL) {
    	    			// Back to running. Go back to nominal state
    	    			nextState = GenericState.NOMINAL;
    	    		}
    				break;
    		}
    		
    		// Report error if transitioning to error state
    		if(nextState == GenericState.ERROR && nextState != state) {
    			warning("R-T.6 Violated: Car stopped on floor " + currentFloor + " with no pending calls.");
    			badStopCount++;
    		}
    		
    		// Transition state
    		state = nextState;
    	}
    }
    
    /**
     * Class to keep track of stopping at floors where there are pending calls
     * Used for checking R-T6
     */
    private class DoorsPendingStateMachine {
    	GenericState state = GenericState.NOMINAL;	// Start in the nominal state
    	Hallway hallway;	// Instance variable for hallway
    	
    	public DoorsPendingStateMachine(Hallway h) {
    		// Set the hallway for this instance
    		this.hallway = h;
    	}
    	
    	/**
    	 * Update the current state
    	 */
    	public void updateState() {
    		// New state
    		GenericState nextState = state;
    		
    		// Get current floor, door state for hallway
    		int currentFloor = landingState.getFloor();
    		boolean doorOpen = doorState.anyDoorOpen(this.hallway);
    		
    		switch(state) {
    			case NOMINAL:
    				// Error transition
    				// Must be at floor and stopped with no car/hall buttons pressed
    	    		if(currentFloor != 0 && 
    	    				doorOpen
    	    				&& !carCallState.isPressed(currentFloor, this.hallway)
    	    				&& !hallCallState.anyPressedOnFloorHallway(currentFloor, this.hallway)) {
    	    			// Stopped with no buttons pressed, go to error state
    	    			nextState = GenericState.ERROR;
    	    		}
    				break;
    				
    			case ERROR:
    				// Nominal transition
    				if(doorState.allDoorsClosed(this.hallway)) {
    	    			// Back to running. Go back to nominal state
    	    			nextState = GenericState.NOMINAL;
    	    		}
    				break;
    		}
    		
    		// Report error if transitioning to error state
    		if(nextState == GenericState.ERROR && nextState != state) {
    			warning("R-T.7 Violated: Doors open in " + this.hallway.toString() + 
    					" hallway on floor " + currentFloor + " with no pending calls.");
    			badDoorsCount++;
    		}
    		
    		// Transition state
    		state = nextState;
    	}
    }
    
    /**
     * Class to keep track of doors only nudging after they've been previously commanded to close
     * Used for checking R-T10
     */
    private class DoorsNudgeStateMachine {
       	DoorState state = DoorState.CLOSED;	// Start in the Closed state
    	Hallway hallway;	// Instance variable for hallway
    	int doorOpeningCount = 0;
    	
    	public DoorsNudgeStateMachine(Hallway h) {
    		// Set the hallway for this instance
    		this.hallway = h;
    	}
    	
    	/**
    	 * Update the current state
    	 */
    	public void updateState() {
    		// New state
    		DoorState nextState = state;
    		
    		switch(state) {
    			case CLOSED:
    				doorOpeningCount = 0;
    				
    				// Switch state to opening if any door motor opening
    				// T1
    				if (doorState.anyDoorMotorOpening(hallway)) {
    					doorOpeningCount++;
    					nextState = DoorState.OPENING;
    				}
    				break;
    				
    			case OPENING:
    				// T2
    				if (doorState.anyDoorMotorClosing(hallway) == true) {
    					nextState = DoorState.CLOSING;
    				// T4
    				} else if (doorState.anyDoorMotorNudging(hallway)) {
    					// Violation check
    					// A violation occurs if doorOpeningCount < 1 which indicates
    					// that we've only opened the door once before nudging. This 
    					// implies that a door reversal did not take place. If a door
    					// reversal happened, doorOpeningCount would be 2
    					// Therefore if doorOpeningCount < 2 -> violation
    					if (doorOpeningCount < 2) {
    		    			warning("R-T.10 Violated: Doors nudged before door reversal at hallway " + this.hallway);
    		    			badDoorNudgeCount++;
    					}
    					nextState = DoorState.NUDGING;
    				}
    				break;
    				
    			case CLOSING:
    				// T3
    				if (doorState.anyDoorMotorOpening(hallway) == true) {
    					doorOpeningCount++;
    					nextState = DoorState.OPENING;
    				// T6
    				} else if (doorState.allDoorsClosed(hallway) == true) {
    					nextState = DoorState.CLOSED;
    				}
    				break;
    				
    			case NUDGING:
    				// T5
    				if (doorState.allDoorsClosed(hallway) == true) {
    					nextState = DoorState.CLOSED;
    				}
    				break;
    			
    			default:
    				throw new RuntimeException("state not expected");
    		}
    		state = nextState;
    	}
    }
    
    /**
     * Class to make sure lanterns turn on when there are calls in the system
     * Used for checking R-T8.1
     */
    private class LanternOnCallsStateMachine {
    	// Start in the door closed
    	LanternCallsState state = LanternCallsState.DOOR_CLOSED;
    	
    	/**
    	 * Update the current state
    	 */
    	public void updateState() {
    		// New state
    		LanternCallsState nextState = state;
    		
    		switch(state) {
    			case DOOR_CLOSED:
    				// T2
    				if(doorState.anyDoorOpen()) 
    					nextState = LanternCallsState.DOOR_OPEN;
    				break;
    				
    			case DOOR_OPEN:
    				// T1
    				if(doorState.allDoorsClosed()) 
    					nextState = LanternCallsState.DOOR_CLOSED;
    				
    				// T3
    				if((carCallState.anyPressed() || hallCallState.anyPressed()) 
    						&& lanternState.noLanternsOn())
    					nextState = LanternCallsState.ERROR;
    				break;
    				
    			case ERROR:
    				// T4
    				if(doorState.allDoorsClosed())
    					nextState = LanternCallsState.DOOR_CLOSED;
    				break;
    			
    			default:
    				throw new RuntimeException("state not expected");
    		}
    		
    		// Throw error if lantern is not on with calls
    		if(nextState == LanternCallsState.ERROR && nextState != state) {
    			warning("R-T.8.1 Violated: Lantern not on when other calls in system");
    			badLanternCallsCount++;
    		}
    		
    		// Transition state
    		state = nextState;
    	}
    }

    /**
     * Class to make sure lanterns don't change direction when the doors are open
     * Used for checking R-T8.2
     */
    private class LanternDirectionStateMachine {
    	// Start in the door closed
    	LanternDirectionState state = LanternDirectionState.DOORS_CLOSED;
    	
    	/**
    	 * Update the current state
    	 */
    	public void updateState() {
    		// New state
    		LanternDirectionState nextState = state;
    		
    		switch(state) {
    			case DOORS_CLOSED:
    				// T1
    				if(doorState.anyDoorOpen() && lanternState.upLanternOn()) 
    					nextState = LanternDirectionState.LANTERN_ON_UP;
    				
    				// T4
    				if(doorState.anyDoorOpen() && lanternState.downLanternOn()) 
    					nextState = LanternDirectionState.LANTERN_ON_DOWN;
    				break;
    				
    			case LANTERN_ON_UP:
    				// T2
    				if(doorState.anyDoorMotorClosing() || doorState.anyDoorMotorNudging())
    					nextState = LanternDirectionState.DOORS_CLOSED;
    				
    				// T5
    				if(lanternState.downLanternOn()) {	// Violation
    					nextState = LanternDirectionState.LANTERN_ON_DOWN;
    					warning("R-T.8.2 Violated: Lantern changed lighting direction while door was open");
    	    			badLanternSwitchCount++;
    				}
    				break;
    				
    			case LANTERN_ON_DOWN:
    				// T3
    				if(doorState.anyDoorMotorClosing() || doorState.anyDoorMotorNudging()) 
    					nextState = LanternDirectionState.DOORS_CLOSED;
    				
    				// T6
    				if(lanternState.upLanternOn()) {	// Violation
    					nextState = LanternDirectionState.LANTERN_ON_UP;
    					warning("R-T.8.2 Violated: Lantern changed lighting direction while door was open");
    					badLanternSwitchCount++;
    				}
    				break;
    			
    			default:
    				throw new RuntimeException("state not expected");
    		}
    		
    		// Transition state
    		state = nextState;
    	}
    }
    
    /**
     * Class to make sure elevator services calls in lantern direction first
     * Used for checking R-T8.3
     */
    private class LanternDriveStateMachine {
    	// Start in the door closed
    	LanternDriveState state = LanternDriveState.LANTERN_OFF;
    	int currentFloor;
    	boolean existCallsInCurrentDirection = false;
    	boolean lastExistCallsInCurrentDirection = false;
    	
    	/**
    	 * Update the current state
    	 */
    	public void updateState() {
    		// New state
    		LanternDriveState nextState = state;
    		
    		switch(state) {
    			case LANTERN_OFF:
    				// Update current floor
    				currentFloor = landingState.getFloor();
    				// Set no existing calls
    				existCallsInCurrentDirection = false;
    				lastExistCallsInCurrentDirection = false;
    				
    				// T1
    				if(lanternState.downLanternOn()) 
    					nextState = LanternDriveState.DOWN_LANTERN_ON;
    				
    				// T2
    				if(lanternState.upLanternOn()) 
    					nextState = LanternDriveState.UP_LANTERN_ON;
    				
    				break;
    				
    			case DOWN_LANTERN_ON:
    				// Set existing calls to last calculated value, in case we transition
    				// This fixes an edge condition where a call is made just after
    				// the doors close, and it is picked up here but then a transition
    				// is made.
    				existCallsInCurrentDirection = lastExistCallsInCurrentDirection;
    				// Set existing calls
    				lastExistCallsInCurrentDirection = carCallState.anyPressedBelowFloor(currentFloor)
						|| hallCallState.anyPressedBelowFloor(currentFloor);
    				
    				// T3
    				if(doorState.allDoorsClosed()) {
    					nextState = LanternDriveState.DOWN_CALLS_LATCH;
    				}
    				break;
    				
    			case UP_LANTERN_ON:
    				// Set existing calls to last calculated value, in case we transition
    				// This fixes an edge condition where a call is made just after
    				// the doors close, and it is picked up here but then a transition
    				// is made.
    				existCallsInCurrentDirection = lastExistCallsInCurrentDirection;
    				// Set existing calls
    				lastExistCallsInCurrentDirection = carCallState.anyPressedAboveFloor(currentFloor)
						|| hallCallState.anyPressedAboveFloor(currentFloor);
    				// T4
    				if(doorState.allDoorsClosed()) {
    					nextState = LanternDriveState.UP_CALLS_LATCH;
    				}
    				break;
    				
    			case DOWN_CALLS_LATCH:
    				// T5
    				if(driveState.getDirection() == Direction.DOWN
    						&& driveState.getSpeed() != Speed.LEVEL) {
    					nextState = LanternDriveState.GOING_DOWN;
    				}
    				
    				// T8	// Maybe violation
    				if(driveState.getDirection() == Direction.UP 
    						&& driveState.getSpeed() != Speed.LEVEL) {
    					nextState = LanternDriveState.GOING_UP;
    					
    					// See if there was a violation, because elevator is going
    					// opposite direction as lantern indicated
    					if(existCallsInCurrentDirection) {
    						// Violation, as there were calls that could have been serviced
    						// in the indicated direction
    						warning("R-T.8.3 Violated: Car traveled opposite lantern direction when there were calls in lantern direction.");
        					badLanternDriveCount++;
    					}
    				}
    				
    				// T12
    				if(lanternState.anyLanternOn()) {
    					// If any lantern turned on, the doors are probably reopening to pick someone else up.
    					// Reset the state machine
    					nextState = LanternDriveState.LANTERN_OFF;
    				}
    				break;
    				
    			case UP_CALLS_LATCH:
    				// T4
    				if(driveState.getDirection() == Direction.UP
    						&& driveState.getSpeed() != Speed.LEVEL) {
    					nextState = LanternDriveState.GOING_UP;
    				}
    				
    				// T7	// Maybe violation
    				if(driveState.getDirection() == Direction.DOWN
    						&& driveState.getSpeed() != Speed.LEVEL) {
    					nextState = LanternDriveState.GOING_DOWN;
    					
    					// See if there was a violation, because elevator is going
    					// opposite direction as lantern indicated
    					if(existCallsInCurrentDirection) {
    						// Violation, as there were calls that could have been serviced
    						// in the indicated direction
    						warning("R-T.8.3 Violated: Car traveled opposite lantern direction when there were calls in lantern direction.");
        					badLanternDriveCount++;
    					}
    				}
    				
    				// T11
    				if(lanternState.anyLanternOn()) {
    					// If any lantern turned on, the doors are probably reopening to pick someone else up.
    					// Reset the state machine
    					nextState = LanternDriveState.LANTERN_OFF;
    				}
    				break;
    				
    			case GOING_DOWN:
    				// T9, transition after delay
    				nextState = LanternDriveState.LANTERN_OFF;
    				break;
    				
    			case GOING_UP:
    				// T10, always transition
    				nextState = LanternDriveState.LANTERN_OFF;
    				break;
    			
    			default:
    				throw new RuntimeException("state not expected");
    		}
    		
    		// Transition state
    		state = nextState;
    	}
    }
    
    /*************************************************************************/
    /**************************** UTILITY CLASSES ****************************/
    /*************************************************************************/
    
    /**
     * Utility class to detect drive speed changes
     */
    private class DriveStateMachine {
    	Speed currentSpeed = Speed.STOP;	// Initialize to stop
    	Direction currentDirection = Direction.STOP;
    	
    	/**
    	 * Receive a DriveSpeed message
    	 * @param msg message
    	 */
    	public void receive(ReadableDrivePayload msg) {
    		// Update the current speed
    		currentSpeed = msg.speed();
    		// Update the current direction
    		currentDirection = msg.direction();
    		// Update the stop pending state machine
    		stopPendingState.updateState();
    		// Update the drive fast state machine
    		driveFastState.updateState();
    	}
    	
    	/**
    	 * Get the current speed
    	 */
    	public Speed getSpeed() {
    		return this.currentSpeed;
    	}
    	
    	/**
    	 * Get the current direction
    	 */
    	public Direction getDirection() {
    		return this.currentDirection;
    	}
    }
    
    /**
     * Utility class to detect landing changes
     */
    private class LandingStateMachine {
    	// Check both atFloor sensors
    	ArrayList<Boolean> atFloorFront = new ArrayList<Boolean>();
    	ArrayList<Boolean> atFloorBack = new ArrayList<Boolean>();
    	
    	/**
    	 * Constructor
    	 */
    	public LandingStateMachine() {
    		// Initialize atFloor sensor arrays
    		for(int i = 0; i < Elevator.numFloors; i++) {
    			// Fill with falses; landings that don't exist will never be updated to true
    			atFloorFront.add(false);
    			atFloorBack.add(false);
    		}
    	}
    	
    	/**
    	 * Receive an AtFloor message
    	 * @param msg message
    	 */
        public void receive(ReadableAtFloorPayload msg) {
        	// Get floor and hallway from message
            Hallway h = msg.getHallway();
            int f = msg.getFloor();
            
            // Check the sensor value
            if(msg.value()) {
            	// If true, set the corresponding hallway/floor to true
            	if(h == Hallway.FRONT) {
            		this.atFloorFront.set(f - 1, true);
            	} else {
            		this.atFloorBack.set(f - 1, true);
            	}
            } else {
            	// If not true, set corresponding hallway/floor to false
            	if(h == Hallway.FRONT) {
            		this.atFloorFront.set(f - 1, false);
            	} else {
            		this.atFloorBack.set(f - 1, false);
            	}
            }
        }
       
        
        /**
         * Return which floor is true
         * @return floor number, or 0 if none
         */
        public int getFloor() {
        	// Loop through atFloor arrays on each hallway, if any are true 
        	// than that is the current floor
        	for(int i = 0; i < Elevator.numFloors; i++) {
        		// Add one because zero-index on array
        		if(atFloorFront.get(i) || atFloorBack.get(i)) return i + 1;	
        	}
        	
        	// If we get to here, none were found
        	return AT_NO_FLOOR;	// zero indicates no floor
        }
    }
    
    /**
     * Utility class to detect car button changes
     */
    private class CarCallStateMachine {
    	// Arrays to store car button state, one for each hallway
    	ArrayList<Boolean> carCallsFront = new ArrayList<Boolean>();
    	ArrayList<Boolean> carCallsBack = new ArrayList<Boolean>();
    	
    	/**
    	 * Constructor, initialize car buttons
    	 */
    	public CarCallStateMachine() {
    		// Initialize the car button states
        	for(int i = 1; i <= Elevator.numFloors; i++) {
        		// Add false for every possible button.
        		// If a button doesn't exist, this will just always stay false.
        		this.carCallsFront.add(false);
        		this.carCallsBack.add(false);
        	}
    	}

		/**
    	 * Receive a CarCall message
    	 * @param msg message
    	 */
        public void receive(ReadableCarCallPayload msg) {
        	// Get floor and hallway from message
            Hallway h = msg.getHallway();
            int f = msg.getFloor();
            int currentFloor = landingState.getFloor();
            
            // Check the button value
            if(mCarCalls[f - 1][h.ordinal()].getValue()) {
            	// If true, set the corresponding [floor,hallway]'s value to true
            	if(h == Hallway.FRONT) this.carCallsFront.set(f - 1, true);
            	else this.carCallsBack.set(f - 1, true);
            } else if(currentFloor == 0) {	// Keep status on floors
            	// If not true, set corresponding [floor,hallway]'s value to false
            	if(h == Hallway.FRONT) this.carCallsFront.set(f - 1, false);
            	else this.carCallsBack.set(f - 1, false);
            }
        }
        
        /**
         * Return whether a button is pressed on a floor, hallway
         * @param f floor
         * @param h hallway
         * @return if any car button on f, h is pressed
         */
    	public boolean isPressed(int f, Hallway h) {
			if(h == Hallway.FRONT) return this.carCallsFront.get(f - 1);
			else return this.carCallsBack.get(f - 1);
		}
        
        /**
         * Return whether any car button is pressed on a floor
         * @param f floor
         * @return if any car button on f is pressed
         */
        public boolean anyPressedOnFloor(int f) {
        	// Return true if any car buttons pressed
        	return this.carCallsFront.get(f - 1) || this.carCallsBack.get(f - 1);
        }
        
        /**
         * Return whether any hall call button is pressed on any floor
         * @return if any car button is pressed
         */
        public boolean anyPressed() {
        	// Return true if any hall buttons pressed
        	for(int i = 1; i < Elevator.numFloors; i++) {
        		if(anyPressedOnFloor(i)) return true;
        	}
        	return false;
        }
        
        /**
         * Return whether any car call button is pressed below a floor
         * @param f floor
         * @return if any car button below f is pressed
         */
        public boolean anyPressedBelowFloor(int f) {
        	// Return true if any hall buttons pressed
        	for(int i = 1; i < f; i++) {
        		if(anyPressedOnFloor(i)) return true;
        	}
        	return false;
        }
        
        /**
         * Return whether any car call button is pressed on above a floor
         * @param f floor
         * @return if any car button above f is pressed
         */
        public boolean anyPressedAboveFloor(int f) {
        	// Return true if any hall buttons pressed
        	for(int i = Elevator.numFloors; i > f; i--) {
        		if(anyPressedOnFloor(i)) return true;
        	}
        	return false;
        }
    }
    
    /**
     * Utility class to detect hall call changes
     */
    private class HallCallStateMachine {
    	// Arrays to store hall button state, one for each hallway, direction
    	ArrayList<Boolean> hallCallsFrontUp = new ArrayList<Boolean>();
    	ArrayList<Boolean> hallCallsFrontDown = new ArrayList<Boolean>();
    	ArrayList<Boolean> hallCallsBackUp = new ArrayList<Boolean>();
    	ArrayList<Boolean> hallCallsBackDown = new ArrayList<Boolean>();
    	
    	/**
    	 * Constructor, initialize hall buttons
    	 */
    	public HallCallStateMachine() {
    		// Initialize the car button states
        	for(int i = 1; i <= Elevator.numFloors; i++) {
        		// Add false for every possible button.
        		// If a button doesn't exist, this will just always stay false.
        		this.hallCallsFrontUp.add(false);
        		this.hallCallsFrontDown.add(false);
        		this.hallCallsBackUp.add(false);
        		this.hallCallsBackDown.add(false);
        	}
    	}

		/**
    	 * Receive a HallCall message
    	 * @param msg message
    	 */
        public void receive(ReadableHallCallPayload msg) {
        	// Get floor, hallway, direction from message
            Hallway h = msg.getHallway();
            int f = msg.getFloor();
            Direction d = msg.getDirection();
            // Get current floor
            int currentFloor = landingState.getFloor();
            
            // Check the network message for this hall call
            if(mHallCalls[f - 1][h.ordinal()][d.ordinal()].getValue()) {
            	// If true, set the corresponding [floor,hallway,direction]'s value to true
            	if(h == Hallway.FRONT && d == Direction.UP) this.hallCallsFrontUp.set(f - 1, true);
            	else if(h == Hallway.FRONT && d == Direction.DOWN) this.hallCallsFrontDown.set(f - 1, true);
            	else if(h == Hallway.BACK && d == Direction.UP) this.hallCallsBackUp.set(f - 1, true);
            	else if(h == Hallway.BACK && d == Direction.DOWN) this.hallCallsBackDown.set(f - 1, true);
            	else throw new RuntimeException("HallCallState hallway or direction out of range.");
            } else if(currentFloor == AT_NO_FLOOR) {	// Keep status when not at floors
            	// If not true, set corresponding [floor,hallway]'s value to false
            	if(h == Hallway.FRONT && d == Direction.UP) this.hallCallsFrontUp.set(f - 1, false);
            	else if(h == Hallway.FRONT && d == Direction.DOWN) this.hallCallsFrontDown.set(f - 1, false);
            	else if(h == Hallway.BACK && d == Direction.UP) this.hallCallsBackUp.set(f - 1, false);
            	else if(h == Hallway.BACK && d == Direction.DOWN) this.hallCallsBackDown.set(f - 1, false);
            	else throw new RuntimeException("HallCallState hallway or direction out of range.");
            }
        }

        /**
         * Return whether any hall button is pressed on a floor, hallway
         * @param f floor
         * @param h hallway
         * @return if any hall button on f,h is pressed
         */
        public boolean anyPressedOnFloorHallway(int f, Hallway h) {
        	// Return true if any hall buttons pressed
        	if(h == Hallway.FRONT) 
        		return this.hallCallsFrontDown.get(f - 1) || this.hallCallsFrontUp.get(f - 1);
        	else 
        		return this.hallCallsBackDown.get(f - 1) || this.hallCallsBackUp.get(f - 1);
		}
        
        /**
         * Return whether any hall call button is pressed on a floor
         * @param f floor
         * @return if any hall button on f is pressed
         */
        public boolean anyPressedOnFloor(int f) {
        	// Return true if any hall buttons pressed
        	return this.hallCallsBackDown.get(f - 1) || this.hallCallsBackUp.get(f - 1) 
        			|| this.hallCallsFrontDown.get(f - 1) || this.hallCallsFrontUp.get(f - 1);
        }
        
        /**
         * Return whether any hall call button is pressed on any floor
         * @return if any hall button is pressed
         */
        public boolean anyPressed() {
        	// Return true if any hall buttons pressed
        	for(int i = 1; i < Elevator.numFloors; i++) {
        		if(anyPressedOnFloor(i)) return true;
        	}
        	return false;
        }
        
        /**
         * Return whether any hall call button is pressed below a floor
         * @param f floor
         * @return if any hall button below f is pressed
         */
        public boolean anyPressedBelowFloor(int f) {
        	// Return true if any hall buttons pressed
        	for(int i = 1; i < f; i++) {
        		if(anyPressedOnFloor(i)) return true;
        	}
        	return false;
        }
        
        /**
         * Return whether any hall call button is pressed on above a floor
         * @param f floor
         * @return if any hall button above f is pressed
         */
        public boolean anyPressedAboveFloor(int f) {
        	// Return true if any hall buttons pressed
        	for(int i = Elevator.numFloors; i > f; i--) {
        		if(anyPressedOnFloor(i)) return true;
        	}
        	return false;
        }
        
    }

    /**
     * Utility class to detect weight changes
     */
    private class WeightStateMachine {

        int oldWeight = 0;

        public void receive(ReadableCarWeightPayload msg) {
            if (oldWeight != msg.weight()) {
                weightChanged(msg.weight());
            }
            oldWeight = msg.weight();
        }
    }

    /**
     * Utility class for keeping track of the door state.
     * 
     * Also provides external methods that can be queried to determine the
     * current door state.
     */
    private class DoorStateMachine {

    	// State for each door
        DoorState state[] = new DoorState[2];

        /**
         * Constructor
         */
        public DoorStateMachine() {
            state[Hallway.FRONT.ordinal()] = DoorState.CLOSED;
            state[Hallway.BACK.ordinal()] = DoorState.CLOSED;
        }
        
        /**
         * Receive a DoorClosed message
         * @param msg the message to receive
         */
        public void receive(ReadableDoorClosedPayload msg) {
            updateState(msg.getHallway());
        }

        /**
         * Receive a DoorOpen message 
         * @param msg message to receive 
         */
        public void receive(ReadableDoorOpenPayload msg) {
            updateState(msg.getHallway());
        }

        /**
         * Receive a DoorMotor message
         * @param msg message to receive 
         */
        public void receive(ReadableDoorMotorPayload msg) {
            updateState(msg.getHallway());
            frontDoorsNudgeStateMachine.updateState();
            backDoorsNudgeStateMachine.updateState();
        }

        /**
         * Update the state 
         * @param h the hallway for this door
         */
        private void updateState(Hallway h) {
        	// Save previous state
            DoorState previousState = state[h.ordinal()];
            // Initialize newState as previousState
            DoorState newState = previousState;
            
            // State transitions
            if (allDoorsClosed(h) && allDoorMotorsStopped(h)) {
                newState = DoorState.CLOSED;
            } else if (allDoorsCompletelyOpen(h) && allDoorMotorsStopped(h)) {
                newState = DoorState.OPEN;
            } else if (anyDoorMotorClosing(h)) {
                newState = DoorState.CLOSING;
            } else if (anyDoorMotorOpening(h)) {
                newState = DoorState.OPENING;
            } else if (anyDoorMotorNudging(h)) {
            	newState = DoorState.NUDGING;
            }

            // Call functions on transitions
            if (newState != previousState) {
                switch (newState) {
                    case CLOSED:
                        doorClosed(h);
                        break;
                    case OPEN:
                        break;
                    case OPENING:
                        break;
                    case CLOSING:
                        break;
                    case NUDGING: 
                    	break;
                }
            }

            // Set the newState
            state[h.ordinal()] = newState;
            
            // Update the doors state machine
            doorsPendingFrontState.updateState();
            doorsPendingBackState.updateState();
            
            // Update the lantern state machines
            lanternOnCallsState.updateState();
            lanternDirectionState.updateState();
    		lanternDriveState.updateState();
        }

        /**
         * Checks if all doors in hallway are completely open
         * @param h hallway
         * @return whether all doors are completely open in hallway h
         */
        public boolean allDoorsCompletelyOpen(Hallway h) {
            return doorOpeneds[h.ordinal()][Side.LEFT.ordinal()].isOpen()
                    && doorOpeneds[h.ordinal()][Side.RIGHT.ordinal()].isOpen();
        }

        /**
         * Checks whether any doors is open in all hallways
         * @return whether any door is open
         */
        public boolean anyDoorOpen() {
            return doorOpeneds[Hallway.FRONT.ordinal()][Side.LEFT.ordinal()].isOpen()
                    || doorOpeneds[Hallway.FRONT.ordinal()][Side.RIGHT.ordinal()].isOpen()
                    || doorOpeneds[Hallway.BACK.ordinal()][Side.LEFT.ordinal()].isOpen()
                    || doorOpeneds[Hallway.BACK.ordinal()][Side.RIGHT.ordinal()].isOpen();

        }

        /**
         * Checks whether any door is open in a hallway
         * @param h hallway
         * @return whether any door is open in hallway h
         */
        public boolean anyDoorOpen(Hallway h) {
            return !doorCloseds[h.ordinal()][Side.LEFT.ordinal()].isClosed()
                    || !doorCloseds[h.ordinal()][Side.RIGHT.ordinal()].isClosed();
        }

        /**
         * Checks whether all doors are closed in hallway h
         * @param h hallway
         * @return whether all doors are closed
         */
        public boolean allDoorsClosed(Hallway h) {
            return (doorCloseds[h.ordinal()][Side.LEFT.ordinal()].isClosed()
                    && doorCloseds[h.ordinal()][Side.RIGHT.ordinal()].isClosed());
        }
        
        /**
         * Checks whether all doors are closed in all hallways
         * @return whether all doors are closed
         */
        public boolean allDoorsClosed() {
        	return allDoorsClosed(Hallway.FRONT) && allDoorsClosed(Hallway.BACK);
        }

        /**
         * Checks whether all door motors are stopped in a hallway
         * @param h hallway 
         * @return whether all door motors are stopped in hallway h
         */
        public boolean allDoorMotorsStopped(Hallway h) {
            return doorMotors[h.ordinal()][Side.LEFT.ordinal()].command() == DoorCommand.STOP 
            		&& doorMotors[h.ordinal()][Side.RIGHT.ordinal()].command() == DoorCommand.STOP;
        }

        /**
         * Checks whether any door motor is opening in a hallway
         * @param h hallway
         * @return whether any door motor is opening in hallway h
         */
        public boolean anyDoorMotorOpening(Hallway h) {
            return doorMotors[h.ordinal()][Side.LEFT.ordinal()].command() == DoorCommand.OPEN 
            		|| doorMotors[h.ordinal()][Side.RIGHT.ordinal()].command() == DoorCommand.OPEN;
        }

        /**
         * Checks whether any door motor is closing in a hallway
         * @param h hallway
         * @return whether any door motor is closing in hallway h
         */
        public boolean anyDoorMotorClosing(Hallway h) {
            return doorMotors[h.ordinal()][Side.LEFT.ordinal()].command() == DoorCommand.CLOSE 
            		|| doorMotors[h.ordinal()][Side.RIGHT.ordinal()].command() == DoorCommand.CLOSE;
        }
        
        /**
         * Checks whether any door motors are closing in any hallway
         * @return whether any door motors are closing
         */
        public boolean anyDoorMotorClosing() {
        	return anyDoorMotorClosing(Hallway.FRONT) || anyDoorMotorClosing(Hallway.BACK);
        }
        
        /**
         * Checks whether any door motor is nudging in a hallway
         * @param h hallway
         * @return whether any door motor is nudging in a hallway
         */
        public boolean anyDoorMotorNudging(Hallway h) {
            return doorMotors[h.ordinal()][Side.LEFT.ordinal()].command() == DoorCommand.NUDGE 
            		|| doorMotors[h.ordinal()][Side.RIGHT.ordinal()].command() == DoorCommand.NUDGE;
        }
        
        /**
         * Checks whether any door motor is nuding in any hallway
         * @return whether any door motor is nudging in any hallway
         */
        public boolean anyDoorMotorNudging() {
            return anyDoorMotorNudging(Hallway.FRONT) || anyDoorMotorNudging(Hallway.BACK);
        }
    }

    /**
     * State machine for keeping track of lanterns
     *
     */
    private class LanternStateMachine {
    	// State, initialize to off
    	LanternState state = LanternState.LANTERN_OFF;
    	
    	/**
    	 * Receive a lantern payload
    	 * @param msg the payload
    	 */
    	public void receive(ReadableCarLanternPayload msg) {
            updateState(msg.getDirection(), msg.lighted());
        }
    	
    	/**
    	 * Update the state for a lantern
    	 * @param d direction of this lantern
    	 * @param lighted whether lantern is lighted
    	 */
    	public void updateState(Direction d, boolean lighted) {
    		// New state
            LanternState newState = state;
            
            // Transitions
            if (d == Direction.UP && lighted) {
                newState = LanternState.LANTERN_UP;
            } else if (d == Direction.DOWN && lighted) {
                newState = LanternState.LANTERN_DOWN;
            } else if ((state == LanternState.LANTERN_UP && d == Direction.UP && !lighted) 
            		|| (state == LanternState.LANTERN_DOWN && d == Direction.DOWN && !lighted)) {
                newState = LanternState.LANTERN_OFF;
            } 

            // Set the new state
            state = newState;
    	}
    	
    	/**
    	 * Return whether the up lantern is on
    	 * @return whether the up lantern is on
    	 */
    	public boolean upLanternOn() {
    		if(state == LanternState.LANTERN_UP) return true;
    		else return false;
    	}
    	
    	/**
    	 * Return whether the down lantern is on
    	 * @return whether the down lantern is on
    	 */
    	public boolean downLanternOn() {
    		if(state == LanternState.LANTERN_DOWN) return true;
    		else return false;
    	}
    	
    	/**
    	 * Return whether no lanterns are on
    	 * @return whether no lanterns are one
    	 */
    	public boolean noLanternsOn() {
    		if(state == LanternState.LANTERN_OFF) return true;
    		else return false;
    	}
    	
    	/**
    	 * Return whether any lanterns are on
    	 * @return whether any lanterns are on
    	 */
    	public boolean anyLanternOn() {
    		return !noLanternsOn();
    	}
    }

    /**
     * Keep track of the accumulated time for an event
     */
    private class Stopwatch {

        private boolean isRunning = false;
        private SimTime startTime = null;
        private SimTime accumulatedTime = SimTime.ZERO;

        /**
         * Start the stopwatch
         */
        public void start() {
            if (!isRunning) {
                startTime = Harness.getTime();
                isRunning = true;
            }
        }

        /**
         * Stop the stopwatch and add the interval to the accumulated total
         */
        public void stop() {
            if (isRunning) {
                SimTime offset = SimTime.subtract(Harness.getTime(), startTime);
                accumulatedTime = SimTime.add(accumulatedTime, offset);
                startTime = null;
                isRunning = false;
            }
        }
        
        /**
         * Reset the accumulated time
         */
        public void resetAccumulated() {
        	// Stop and the timer if it is running
        	if(isRunning) stop();
        	// Reset the accumulated time
        	accumulatedTime = SimTime.ZERO;
        }
        
        /**
         * Get current accumulated time
         * @return accumulated time
         */
        public SimTime getAccumulatedTime() {
            return accumulatedTime;
        }
    }

	@Override
	public void timerExpired(Object callbackData) {
		// Necessary to have this to make compiler happy
	}
}