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

/**
 * This class defines constants for CAN IDs that are used throughout the simulator.
 *
 * The default values will work for early projects.  Later on, you will modify these
 * values when you create a network schedule.
 *
 * @author justinr2
 */
public class MessageDictionary {

    //controller periods
    public final static int NONE = -1;
    public final static SimTime HALL_BUTTON_CONTROL_PERIOD = new SimTime(100, SimTimeUnit.MILLISECOND);
    public final static SimTime CAR_BUTTON_CONTROL_PERIOD = new SimTime(100, SimTimeUnit.MILLISECOND);
    public final static SimTime LANTERN_CONTROL_PERIOD = new SimTime(200, SimTimeUnit.MILLISECOND);
    public final static SimTime CAR_POSITION_CONTROL_PERIOD = new SimTime(50, SimTimeUnit.MILLISECOND);
    public final static SimTime DISPATCHER_PERIOD = new SimTime(50, SimTimeUnit.MILLISECOND);
    public final static SimTime DOOR_CONTROL_PERIOD = new SimTime(10, SimTimeUnit.MILLISECOND);
    public final static SimTime DRIVE_CONTROL_PERIOD = new SimTime(10, SimTimeUnit.MILLISECOND);

    //controller message IDs
    public final static int DRIVE_CAN_ID =              		0x0BF2B500;
    public final static int DRIVE_SPEED_CAN_ID =                DRIVE_CAN_ID;
    public final static int DRIVE_COMMAND_CAN_ID =              DRIVE_CAN_ID;

    public final static int DESIRED_DWELL_CAN_ID = 				0x0C2EB600;
    public final static int DESIRED_DWELL_BASE_CAN_ID =         DESIRED_DWELL_CAN_ID;
    public final static int DESIRED_FLOOR_CAN_ID =              0x0C24B600;
    public final static int CAR_POSITION_CAN_ID =               0x0C38B700;
    public final static int DOOR_MOTOR_COMMAND_BASE_CAN_ID =    0x0BFCB800;
    public final static int HALL_CALL_BASE_CAN_ID =             0x0C4CB900;
    public final static int HALL_LIGHT_BASE_CAN_ID =            0x0C9CB900;
    public final static int CAR_CALL_BASE_CAN_ID =              0x0C60BA00;
    public final static int CAR_LIGHT_BASE_CAN_ID =             0x0CA6BA00;
    public final static int CAR_LANTERN_BASE_CAN_ID =           0x0C7EBB00;
    
    //module message IDs
    public final static int AT_FLOOR_BASE_CAN_ID =              0x0C1A2800;
    public final static int CAR_LEVEL_POSITION_CAN_ID =         0x0C423C00;
    public final static int CAR_WEIGHT_CAN_ID =                 0x0C927800;
    public final static int CAR_WEIGHT_ALARM_CAN_ID =           0x0C888C00;
    public final static int DOOR_OPEN_SENSOR_BASE_CAN_ID =      0x0C74A000;
    public final static int DOOR_CLOSED_SENSOR_BASE_CAN_ID =    0x0C565000;
    public final static int DOOR_REVERSAL_SENSOR_BASE_CAN_ID =  0x0C066400;
    public final static int HOISTWAY_LIMIT_BASE_CAN_ID =        0x0C6AB400;
    public final static int EMERGENCY_BRAKE_CAN_ID =            0x0C101400;
    public final static int LEVELING_BASE_CAN_ID =              0x0BE81400;
    
}
