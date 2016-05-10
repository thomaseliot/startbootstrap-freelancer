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
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.translators.CanPayloadTranslator;

/**
 * CAN payload translator for the drive speed, which includes a speed value and a direction.
 */
public class DriveSpeedCanPayloadTranslator extends CanPayloadTranslator {

	/**
	 * Create a WriteableCanMailbox for DriveSpeed
	 * @param p the mailbox
	 */
    public DriveSpeedCanPayloadTranslator(WriteableCanMailbox p) {
        super(p, 8, MessageDictionary.DRIVE_SPEED_CAN_ID);
    }
    
    /**
	 * Create a ReadableCanMailbox for DriveSpeed
	 * @param p the mailbox
	 */
    public DriveSpeedCanPayloadTranslator(ReadableCanMailbox p) {
        super(p, 8, MessageDictionary.DRIVE_SPEED_CAN_ID);
    }

    /**
     * Set. Required for reflection
     *
     * @param speed the speed to set to
     * @param direction the direction to set to
     */
    public void set(float speed, Direction direction) {
        setSpeed(speed);
        setDirection(direction);
    }
    
    /**
     * Set the speed
     * @param speed speed to set to
     */
    public void setSpeed(float speed) {
        BitSet b = getMessagePayload();
        addFloatToBitset(b, speed, 0, 32);
        setMessagePayload(b, getByteSize());
    }

    /**
     * Get the speed
     * @return the current speed
     */
    public float getSpeed() {
        float val = getFloatFromBitset(getMessagePayload(), 0, 32);
        return val;
    }

    /**
     * Set the direction
     * @param direction direction to set to
     */
    public void setDirection(Direction direction) {
        BitSet b = getMessagePayload();
        addIntToBitset(b, direction.ordinal(), 32, 32);
        setMessagePayload(b, getByteSize());
    }

    /**
     * Get the direction
     * @retur the current direction
     */
    public Direction getDirection() {
        int val = getIntFromBitset(getMessagePayload(), 32, 32);
        for (Direction d : Direction.values()) {
            if (d.ordinal() == val) {
                return d;
            }
        }
        throw new RuntimeException("Unrecognized Direction Value " + val);
    }

    @Override
    /**
     * Make human readable string
     */
    public String payloadToString() {
        return "DriveSpeed:  speed=" + getSpeed() + " direction=" + getDirection();
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
    public static void addFloatToBitset(BitSet b, float value, int startLocation,
        int bitSize)
    {
        if (bitSize != 32)
        {
            throw new IllegalArgumentException("This is not a float, yo.");
        }
        int mask = 0x1;
        int bitOffset = startLocation;
        int val = Float.floatToRawIntBits(value);
        for (int i = 0; i < bitSize; i++)
        {
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
    public static float getFloatFromBitset(BitSet b, int startLocation, int bitSize)
    {
        if (bitSize != 32)
        {
            throw new RuntimeException("This is not a float, yo");
        }

        int value = 0;
        int mask = 0x1;
        int bitOffset = startLocation;
        for (int i = 0; i < bitSize; i++)
        {
            if (b.get(bitOffset))
            {
                value = value | mask;
            }
            mask = mask << 1;
            bitOffset++;
        }
        return Float.intBitsToFloat(value);
    }
}

