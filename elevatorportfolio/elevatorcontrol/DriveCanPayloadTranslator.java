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

import java.util.BitSet;
import simulator.framework.Direction;
import simulator.framework.Speed;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.translators.CanPayloadTranslator;

/**
 * Can payload translator for the drive controller, which includes a drive sensor speed and direction, as 
 * well as a commanded speed and direction.
 */
public class DriveCanPayloadTranslator extends CanPayloadTranslator {

	/**
	 * Constructor for WriteableCanmailbox for DriveCommand
	 * @param p the mailbox
	 */
    public DriveCanPayloadTranslator(WriteableCanMailbox p) {
        super(p, 5, MessageDictionary.DRIVE_CAN_ID);
    }
    
    /**
     * Constructor for WriteableCanmailbox for DriveCommand
     * @param p the mailbox
     */
    public DriveCanPayloadTranslator(ReadableCanMailbox p) {
        super(p, 5, MessageDictionary.DRIVE_CAN_ID);
    }

    /**
     * Set method, required for reflection
     *
     * @param speed speed to set to
     * @param dir direction to set to
     */
    public void set(float sensorSpeed, Direction sensorDir, Speed commandSpeed, Direction commandDir) {
        setSensorSpeed(sensorSpeed);
        setSensorDirection(sensorDir);
        setCommandSpeed(commandSpeed);
        setCommandDirection(commandDir);
    }
    
    /**
     * Set the sensor speed
     * @param speed to set to network drive sensor reading to
     */
    public void setSensorSpeed(float sensorSpeed) {
        BitSet b = getMessagePayload();
        addFloatToBitset(b, sensorSpeed, 0, 32);
        setMessagePayload(b, getByteSize());
    }
    
    /**
     * Set the commanded speed
     * @param commandSpeed to command the drive controller
     */
    public void setCommandSpeed(Speed commandSpeed) {
        BitSet b = getMessagePayload();
        addUnsignedIntToBitset(b, commandSpeed.ordinal(), 34, 2);
        setMessagePayload(b, getByteSize());
    }

    /**
     * Get the sensor speed
     * @return current drive sensor speed
     */
    public float getSensorSpeed() {
    	float val = getFloatFromBitset(getMessagePayload(), 0, 32);
        return val;
    }
    
    /**
     * Get the commanded speed
     * @return current drive commanded speed
     */
    public Speed getCommandSpeed() {
        int val = getUnsignedIntFromBitset(getMessagePayload(), 34, 2);
        for (Speed s : Speed.values()) {
            if (s.ordinal() == val) {
                return s;
            }
        }
        throw new RuntimeException("Unrecognized Speed Value " + val);
    }

    /**
     * Set the sensor direction
     * @param sensorDir direction to set to sensor network message to
     */
    public void setSensorDirection(Direction sensorDir) {
        BitSet b = getMessagePayload();
        addUnsignedIntToBitset(b, sensorDir.ordinal(), 32, 2);
        setMessagePayload(b, getByteSize());
    }
    
    /**
     * Set the commanded direction
     * @param commandDir direction to command the drive to
     */
    public void setCommandDirection(Direction commandDir) {
        BitSet b = getMessagePayload();
        addUnsignedIntToBitset(b, commandDir.ordinal(), 36, 2);
        setMessagePayload(b, getByteSize());
    }
    
    /**
     * Get the drive speed sensor direction
     * @return drive speed sensor direction
     */
    public Direction getSensorDirection() {
        int val = getUnsignedIntFromBitset(getMessagePayload(), 32, 2);
        for (Direction d : Direction.values()) {
            if (d.ordinal() == val) {
                return d;
            }
        }
        throw new RuntimeException("Unrecognized Direction Value " + val);
    }

    /**
     * Get the commanded direction
     * @return commanded direction
     */
    public Direction getCommandDirection() {
        int val = getUnsignedIntFromBitset(getMessagePayload(), 36, 2);
        for (Direction d : Direction.values()) {
            if (d.ordinal() == val) {
                return d;
            }
        }
        throw new RuntimeException("Unrecognized Direction Value " + val);
    }
    

    @Override
    /**
     * Make human-readable string of state
     */
    public String payloadToString() {
        return "Drive:  Commanded Speed=" + getCommandSpeed() + 
        		" Commanded direction=" + getCommandDirection() + 
        		" Sensor Speed =" + getSensorSpeed() + 
        		" Sensor Direction=" + getSensorDirection();
    }



	/**
	 * Utility method to add an float value to a bit set. This method
	 * modifies the bits from <code>startLocation</code> to <code>startLocation
	 * + bitSize</code> by setting them according to the given integer value.
	 * By calling this method several times with different startLocations,
	 * multiple values can be stored in a single bitset.
	 * 
	 * @param b
	 *        BitSet to modify.
	 * @param value
	 *        float value to set. Negative values will be preserved
	 * @param startLocation
	 *        the index in the bit set that corresponds to the least significant bit of the value.
	 *        This value is zero-indexed.
	 * @param bitSize
	 *        the number of bits used to represent the integer. Values larger
	 *        than 32 will generate an error.
	 */
	public static void addFloatToBitset(BitSet b, float value, int startLocation, int bitSize) {
	    if (bitSize != 32){
	        throw new IllegalArgumentException("This is not a float, yo.");
	    }
	    int mask = 0x1;
	    int bitOffset = startLocation;
	    int val = Float.floatToRawIntBits(value);
	    for (int i = 0; i < bitSize; i++){
	        b.set(bitOffset, (val & mask) == mask);
	        mask = mask << 1;
	        bitOffset++;
	    }
	}

	/**
	 * Recovers an float value from the specified bit range. This method is
	 * designed to be used in conjunction with addFloatToBitset.
	 * 
	 * @param b
	 *        The BitSet to read
	 * @param startLocation
	 *        The location of the lsb of the value. This value is zero-indexed.
	 * @param bitSize
	 *        The number of bits to read.
	 * @return The recovered float value.
	 */
	public static float getFloatFromBitset(BitSet b, int startLocation, int bitSize) {
	    if (bitSize != 32) {
	        throw new RuntimeException("This is not a float, yo");
	    }
	
	    int value = 0;
	    int mask = 0x1;
	    int bitOffset = startLocation;
	    for (int i = 0; i < bitSize; i++) {
	        if (b.get(bitOffset)) {
	            value = value | mask;
	        }
	        mask = mask << 1;
	        bitOffset++;
	    }
	    return Float.intBitsToFloat(value);
	}
}

