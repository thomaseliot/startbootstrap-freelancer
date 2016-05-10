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

import simulator.elevatorcontrol.MessageDictionary;
import simulator.framework.Direction;
import simulator.framework.ReplicationComputer;
import simulator.payloads.CanMailbox.ReadableCanMailbox;
import simulator.payloads.CanMailbox.WriteableCanMailbox;
import simulator.payloads.translators.CanPayloadTranslator;

/**
 * Car lantern CAN payload translator
 * Replication: direction
 */
public class CarLanternCanPayloadTranslator extends CanPayloadTranslator {
	
	// The name of this can payload translator, for logging purposes.
	String name;

    /**
     * CAN payload translator for car lantern network messages, transmit
     * @param p  CAN payload object whose message is interpreted by this translator
     * @param direction  replication index
     */
    public CarLanternCanPayloadTranslator(WriteableCanMailbox p, Direction direction) {
        // Create the CanPayloadTranslator, size 1 byte
    	super(p, 1, MessageDictionary.CAR_LANTERN_BASE_CAN_ID + ReplicationComputer.computeReplicationId(direction));
        // Compute name for this instance
        this.name = "CarLantern" + ReplicationComputer.makeReplicationString(direction);
        
        // Set default value to off
        this.setValue(false);
    }

    /**
     * CAN payload translator for car lantern network messages, receive
     * @param p  CAN payload object whose message is interpreted by this translator
     * @param direction  replication index
     */
    public CarLanternCanPayloadTranslator(ReadableCanMailbox p, Direction direction) {
    	super(p, 1, MessageDictionary.CAR_LANTERN_BASE_CAN_ID + ReplicationComputer.computeReplicationId(direction));
    }
    
    /**
     * Set function, required for reflection
     * @param value
     */
    public void set(boolean value) {
        this.setValue(value);
    }

    /**
     * Set value to true or false
     * @param value new value
     */
    public void setValue(boolean value) {
        BitSet b = getMessagePayload();
        b.set(0, value);
        setMessagePayload(b, getByteSize());
    }
    
    /**
     * Get the current value
     * @return current value
     */
    public boolean getValue() {
        return getMessagePayload().get(0);
    }
    
    /**
     * Return whether or not this lantern is light
     * @return whether the lantern is light
     */
    public boolean lighted() {
        return getValue();
    }

    
    @Override
    /**
     * Return the name of this translator
     */
    public String payloadToString() {
        return name + " = " + getValue();
    }
}
