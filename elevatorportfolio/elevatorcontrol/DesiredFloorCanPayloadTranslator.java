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
import simulator.framework.Hallway;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.translators.CanPayloadTranslator;


/**
 * CAN payload translator for desiredFloor
 */
public class DesiredFloorCanPayloadTranslator extends CanPayloadTranslator {

    /**
     * Constructor for WriteableCanMailbox for desiredFloor
     * @param payload
     */
    public DesiredFloorCanPayloadTranslator(WriteableCanMailbox payload) {
        super(payload, 8, MessageDictionary.DESIRED_FLOOR_CAN_ID);
    }

    /**
     * Constructor for ReadableCanMailbox for desiredFloor
     * @param payload
     */
    public DesiredFloorCanPayloadTranslator(ReadableCanMailbox payload) {
        super(payload, 8, MessageDictionary.DESIRED_FLOOR_CAN_ID);
    }
    
    /**
     * Set. Required for reflection.
     *
     * @param floor the desired floor
     * @param dir the desired direction 
     * @param hallway the desired hallway
     */
    public void set(int floor, Direction dir, Hallway hallway) {
        setFloor(floor);
        setDirection(dir);
        setHallway(hallway);
    }
    
    /**
     * Similar to the other set method, but the Hallway/Dir field order reversed.
     * @param floor the desired floor
     * @param hallway the desired direction
     * @param dir the desired hallway
     */
    public void set(int floor, Hallway hallway, Direction dir) {
        setFloor(floor);
        setDirection(dir);
        setHallway(hallway);
    }

    /**
     * Set the floor for mDesiredFloor into the lowest 32 bits of the payload
     * @param floor the floor to set to
     */
    public void setFloor(int floor) {
        BitSet b = getMessagePayload();
        addIntToBitset(b, floor, 0, 32);
        setMessagePayload(b, getByteSize());
    }

    /**
     * Get the desired floor
     * @return the floor value from the can message payload
     */
    public int getFloor() {
        return getIntFromBitset(getMessagePayload(), 0, 32);
    }

    /**
     * Set the direction for mDesiredFloor in bits 32-47 of the can payload
     * @param dir the desired direction
     */
    public void setDirection(Direction dir) {
        BitSet b = getMessagePayload();
        addIntToBitset(b, dir.ordinal(), 32, 16);
        setMessagePayload(b, getByteSize());
    }

    /**
     * Get the desired direction
     * @return the direction value from the can payload
     */
    public Direction getDirection() {
        int val = getIntFromBitset(getMessagePayload(), 32, 16);
        for (Direction d : Direction.values()) {
            if (d.ordinal() == val) {
                return d;
            }
        }
        throw new RuntimeException("Unrecognized Direction Value " + val);
    }

    /**
     * Set the hallway for mDesiredFloor in bits 48-63 of the can payload
     * @param hallway the desired hallway
     */
    public void setHallway(Hallway hallway) {
        BitSet b = getMessagePayload();
        addIntToBitset(b, hallway.ordinal(), 48, 16);
        setMessagePayload(b, getByteSize());
    }

    /**
     * Get the desired hallway
     * @return the hallway value from the CAN payload.
     */
    public Hallway getHallway() {
        int val = getIntFromBitset(getMessagePayload(), 48, 16);
        for (Hallway h : Hallway.values()) {
            if (h.ordinal() == val) {
                return h;
            }
        }
        throw new RuntimeException("Unrecognized Hallway Value " + val);
    }

    /**
     * Printing method for the translator.
     * @return human readable version of the payload
     */
    @Override
    public String payloadToString() {
        return "DesiredFloor = " + getFloor() + ", DesiredDirection = " + getDirection() + ", DesiredHallway = " + getHallway();
    }
}
