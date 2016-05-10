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
 * Can payload translator for the drive command, which includes a speed value and a direction.
 */
public class DriveCommandCanPayloadTranslator extends CanPayloadTranslator {

	/**
	 * Constructor for WriteableCanmailbox for DriveCommand
	 * @param p the mailbox
	 */
    public DriveCommandCanPayloadTranslator(WriteableCanMailbox p) {
        super(p, 8, MessageDictionary.DRIVE_COMMAND_CAN_ID);
    }
    
    /**
     * Constructor for WriteableCanmailbox for DriveCommand
     * @param p the mailbox
     */
    public DriveCommandCanPayloadTranslator(ReadableCanMailbox p) {
        super(p, 8, MessageDictionary.DRIVE_COMMAND_CAN_ID);
    }

    /**
     * Set method, required for reflection
     *
     * @param speed speed to set to
     * @param dir direction to set to
     */
    public void set(Speed speed, Direction dir) {
        setSpeed(speed);
        setDirection(dir);
    }
    
    /**
     * Set the speed
     * @param speed the speed to set to
     */
    public void setSpeed(Speed speed) {
        BitSet b = getMessagePayload();
        addIntToBitset(b, speed.ordinal(), 0, 32);
        setMessagePayload(b, getByteSize());
    }

    /**
     * Get the speed
     * @return current speed
     */
    public Speed getSpeed() {
        int val = getIntFromBitset(getMessagePayload(), 0, 32);
        for (Speed s : Speed.values()) {
            if (s.ordinal() == val) {
                return s;
            }
        }
        throw new RuntimeException("Unrecognized Speed Value " + val);
    }

    /**
     * Set the direction
     * @param dir direction to set to
     */
    public void setDirection(Direction dir) {
        BitSet b = getMessagePayload();
        addIntToBitset(b, dir.ordinal(), 32, 32);
        setMessagePayload(b, getByteSize());
    }

    /**
     * Get the direction
     * @return current direction
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
     * Make human-readable string of state
     */
    public String payloadToString() {
        return "DriveCommand:  speed=" + getSpeed() + " direction=" + getDirection();
    }
}
