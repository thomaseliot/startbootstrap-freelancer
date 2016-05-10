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

import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.translators.CanPayloadTranslator;

/**
 * Desired Dwell CAN Payload Translator
 */
public class DesiredDwellCanPayloadTranslator extends CanPayloadTranslator {

    /**
     * Constructor for use with WriteableCanMailbox objects
     * @param payload
     */
    public DesiredDwellCanPayloadTranslator(WriteableCanMailbox payload) {
    	// Two bytes
        super(payload, 2, MessageDictionary.DESIRED_DWELL_CAN_ID);
    }

    /**
     * Constructor for use with ReadableCanMailbox objects
     * @param payload
     */
    public DesiredDwellCanPayloadTranslator(ReadableCanMailbox payload) {
    	// Two bytes
        super(payload, 2, MessageDictionary.DESIRED_DWELL_CAN_ID);
    }

    /**
     * Set function, required for reflection
     * @param value the value to set to
     */
    public void set(byte valueFront, byte valueBack)
    {
    	setDwellFront(valueFront);
    	setDwellBack(valueBack);
    }
    
    /**
     * Set the value of desiredDwell
     * @param value the value to set
     */
    public void setDwellFront(byte value) {
        BitSet b = new BitSet();
        // Eight bytes for int
        addIntToBitset(b, value, 0, 8);
        setMessagePayload(b, getByteSize());		
    }
    
    public void setDwellBack(byte value) {
        BitSet b = new BitSet();
        // Eight bytes for int
        addIntToBitset(b, value, 0, 8);
        setMessagePayload(b, getByteSize());		
    }
    
    /**
     * Get the current value of desiredDwell
     * @return the current value of desiredDwell
     */
    public byte getDwellFront() {
    	// Eight bytes for int
		return (byte) getIntFromBitset(getMessagePayload(), 0, 8);
    	
    }
    public byte getDwellBack() {
    	// Eight bytes for int
		return (byte) getIntFromBitset(getMessagePayload(), 0, 8);
    	
    }
    @Override
    /**
     * Return a string with this number
     */
    public String payloadToString() {

        return "" + Integer.toString(getDwellFront()) + "," +
        		Integer.toString(getDwellBack());
    	
    }
    
    /**
     * Utility method to add an long value to a bit set. Use for 
     * a 64-bit long.
     * 
     * @param b
     *        BitSet to modify.
     * @param value
     *        long value to set. Negative values will be preserved
     */
    public static void addLongToBitset(BitSet b, long value) {
    	// Split the long into two ints and add them
    	int highBits, lowBits;
    	
    	highBits = (int)((value >> 32) & 0x0000FFFF);
    	lowBits = (int)(value & 0x0000FFFF);
    	
    	// Add the two ints to the bitset, highBits first
    	addIntToBitset(b, highBits, 0, 32);
    	addIntToBitset(b, lowBits, 32, 32);
    	// Done
    }
    
    /**
     * Recovers a long value from the specified bit set
     * 
     * @param b
     *        The BitSet to read
     * @return The recovered (positive) long value.
     */
    public static long getLongFromBitset(BitSet b)
    {
        long value;
        int highBits;
        int lowBits;
        
        // Get highBits and lowBits from the bitset
        highBits = getIntFromBitset(b, 0, 32);
        lowBits = getIntFromBitset(b, 32, 32);
        
        // Reconstruct the long from high and low bits
        value = ((((long)highBits) << 32) & 0xFFFF0000);
        value |= (long)lowBits;
        
        // Return the reconstructed value
        return value;
    }
}