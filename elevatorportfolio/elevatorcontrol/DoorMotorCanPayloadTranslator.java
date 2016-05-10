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

import simulator.framework.DoorCommand;
import simulator.framework.Hallway;
import simulator.framework.ReplicationComputer;
import simulator.framework.Side;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.translators.CanPayloadTranslator;

/**
 * This is a CAN payload translator for door motor command messages.  It
 * takes two data fields (hall, side) and packages them into a bit-level 
 * representation of the message.
 */
public class DoorMotorCanPayloadTranslator extends CanPayloadTranslator {

	// Name for this translator
	String name;
	
    /**
     * Constructor for WriteableCanMailbox for DoorMotor
     * @param payload
     * @param h hallway
     * @param s side 
     */
    public DoorMotorCanPayloadTranslator(WriteableCanMailbox payload, Hallway h, Side s) {
        super(payload, 8, MessageDictionary.DOOR_MOTOR_COMMAND_BASE_CAN_ID
        		+ ReplicationComputer.computeReplicationId(h, s));
        name = "DoorMotor" + h.name() + "," + s.name();
    }

    /**
     * Constructor for ReadableCanMailbox for DoorMotor
     * @param payload
     * @param h hallway
     * @param s side
     */
    public DoorMotorCanPayloadTranslator(ReadableCanMailbox payload, Hallway h, Side s) {
    	super(payload, 8, MessageDictionary.DOOR_MOTOR_COMMAND_BASE_CAN_ID
        		+ ReplicationComputer.computeReplicationId(h, s));
    	name = "DoorMotor" + h.name() + "," + s.name();
    }

    /**
     * Set method, required for reflection
     *
     * @param door command
     */
    public void set(DoorCommand c) {
    	setCommand(c);
    }
    /**
     * This method sets the current DoorMotor command
     * Uses bits 0-32 of the CAN payload
     *
     * @param c door command
     */
    public void setCommand(DoorCommand c) {
        BitSet b = getMessagePayload();
        addIntToBitset(b, c.ordinal(), 0, 8);
        setMessagePayload(b, getByteSize());
    }
    
    /**
     * Get the current door command
     * @return the current DoorMotor command
     */
    public DoorCommand getCommand() {
        int val = getIntFromBitset(getMessagePayload(), 0, 8);
        for (DoorCommand c : DoorCommand.values()) {
            if (c.ordinal() == val) {
                return c;
            }
        }
        throw new RuntimeException("Unrecognized Hallway Value " + val);
    }

    /**
     * Implement a printing method for the translator.
     * @return human readable version of the payload
     */
    @Override
    public String payloadToString() {
    	return name + ": " + getCommand().name();
    }
}
