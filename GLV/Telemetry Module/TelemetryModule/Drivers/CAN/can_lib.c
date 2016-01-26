/*
 * can_lib.c
 
	The contents of this file were originally from the CAN Software Library
	provided by Atmel written for AT90CAN devices. Use of this file is subject 
	to Atmel's End User License Agreement.
 
 */ 

#include "can_config.h"
#include "can_lib.h"
#include "can_drv.h"

//------------------------------------------------------------------------------
//  @fn can_init
//!
//! CAN macro initialization. Reset the CAN peripheral, initialize the bit
//! timing, initialize all the registers mapped in SRAM to put MObs in
//! inactive state and enable the CAN macro.
//!
//! @warning The CAN macro will be enable after seen on CAN bus a receceive
//!          level as long as of an inter frame (hardware feature).
//!
//! @param  Mode (for "can_fixed_baudrate" param not used)
//!         ==0: start CAN bit timing evaluation from faster baudrate
//!         ==1: start CAN bit timing evaluation with CANBTx registers
//!              contents
//!
//! @return Baudrate Status
//!         ==0: research of bit timing configuration failed
//!         ==1: baudrate performed 
//!
//------------------------------------------------------------------------------
uint8_t can_init(uint8_t mode)
{
    if ((Can_bit_timing(mode))==0) return (0);  // c.f. macro in "can_drv.h"
    can_clear_all_mob();                        // c.f. function in "can_drv.c"
    Can_enable();                               // c.f. macro in "can_drv.h" 
    return (1);
}

//------------------------------------------------------------------------------
//  @fn can_cmd
//!
//! This function takes a CAN descriptor, analyses the action to do:
//! transmit, receive or abort.
//! This function returns a status (CAN_CMD_ACCEPTED or CAN_CMD_REFUSED) if
//! a MOb for Rx or Tx has been found. If no MOB has been found, the
//! application must be retry at a later date.
//! This function also updates the CAN descriptor status (MOB_PENDING or
//! MOB_NOT_REACHED) if a MOb for Rx or Tx has been found. If aborting
//! is performed, the CAN descriptor status will be set to STATUS_CLEARED.
//!
//! @param  st_cmd_t* - Can_descriptor pointer on CAN descriptor structure
//!         to select the action to do.
//!
//! @return CAN_CMD_ACCEPTED - command is accepted
//!         CAN_CMD_REFUSED  - command is refused
//!
//------------------------------------------------------------------------------
uint8_t can_cmd(st_cmd_t* cmd)
{
  uint8_t mob_handle, cpt;
  uint32_t u32_temp;
  
  if (cmd->cmd == CMD_ABORT)
  {
    if (cmd->status == MOB_PENDING)
    {
      // Rx or Tx not yet performed
      Can_set_mob(cmd->handle);
      Can_mob_abort();
      Can_clear_status_mob();       // To be sure !
      cmd->handle = 0;
    }
    cmd->status = STATUS_CLEARED; 
  }
  else
  {
    mob_handle = can_get_mob_free();
    if (mob_handle!= NO_MOB)
    {
      cmd->status = MOB_PENDING; 
      cmd->handle = mob_handle;
      Can_set_mob(mob_handle);
      Can_clear_mob();
          
      switch (cmd->cmd)
      {
        //------------      
        case CMD_TX:    
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          for (cpt=0;cpt<cmd->dlc;cpt++) CANMSG = *(cmd->pt_data + cpt);
          if (cmd->ctrl.rtr) Can_set_rtr(); 
            else Can_clear_rtr();    
          Can_set_dlc(cmd->dlc);
          Can_config_tx();
          break;
        //------------      
        case CMD_TX_DATA:    
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          for (cpt=0;cpt<cmd->dlc;cpt++) CANMSG = *(cmd->pt_data + cpt);
          cmd->ctrl.rtr=0; Can_clear_rtr();
          Can_set_dlc(cmd->dlc);
          Can_config_tx();
          break;
        //------------      
        case CMD_TX_REMOTE:       
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          cmd->ctrl.rtr=1; Can_set_rtr();
          Can_set_dlc(cmd->dlc);
          Can_config_tx();
          break;
        //------------      
        case CMD_RX:
          u32_temp=0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          Can_clear_rtrmsk();
          Can_clear_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_DATA:
          u32_temp=0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=0; Can_set_rtrmsk(); Can_clear_rtr();
          Can_clear_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_REMOTE:
          u32_temp=0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=1; Can_set_rtrmsk(); Can_set_rtr();
          Can_clear_rplv();
          Can_clear_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_MASKED:
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          u32_temp=~0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          Can_clear_rtrmsk();
          Can_set_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_DATA_MASKED:
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          u32_temp=~0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=0; Can_set_rtrmsk(); Can_clear_rtr();
          Can_set_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_RX_REMOTE_MASKED:
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          u32_temp=~0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=1; Can_set_rtrmsk(); Can_set_rtr();
          Can_clear_rplv();
          Can_set_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_REPLY:
          for (cpt=0;cpt<cmd->dlc;cpt++) CANMSG = *(cmd->pt_data + cpt);
          u32_temp=0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=1; Can_set_rtrmsk(); Can_set_rtr();
          Can_set_rplv();
          Can_clear_idemsk();
          Can_config_rx();       
          break;
        //------------      
        case CMD_REPLY_MASKED:
          if (cmd->ctrl.ide){ Can_set_ext_id(cmd->id.ext);}
          else              { Can_set_std_id(cmd->id.std);}
          for (cpt=0;cpt<cmd->dlc;cpt++) CANMSG = *(cmd->pt_data + cpt);
          u32_temp=~0; Can_set_ext_msk(u32_temp);
          Can_set_dlc(cmd->dlc);
          cmd->ctrl.rtr=1; Can_set_rtrmsk(); Can_set_rtr();
          Can_set_rplv();
          Can_set_idemsk();
          Can_config_rx();       
          break;
        //------------      
        default:
          // case CMD_NONE or not implemented command
          cmd->status = STATUS_CLEARED; 
          break;
        //------------      
      } // switch (cmd ...
    } // if (mob_handle ...
    else
    {
      cmd->status = MOB_NOT_REACHED;
      return CAN_CMD_REFUSED;
    }
  } // else of no CMD_ABORT
  return CAN_CMD_ACCEPTED;
}

//------------------------------------------------------------------------------
//  @fn can_get_status
//!
//! This function allows to return if the command has been performed or not.
//! In an reception case, all the CAN message is stored in the structure.
//! This function also updates the CAN descriptor status (MOB_TX_COMPLETED,    
//!  MOB_RX_COMPLETED, MOB_RX_COMPLETED_DLCW or MOB_???_ERROR).         
//!
//! @param  st_cmd_t* pointer on CAN descriptor structure.
//!
//! @return CAN_STATUS_COMPLETED     - Rx or Tx is completed
//!         CAN_STATUS_NOT_COMPLETED - Rx or Tx is not completed
//!         CAN_STATUS_ERROR         - Error in configuration or in the
//!                                    CAN communication
//!
//------------------------------------------------------------------------------
uint8_t can_get_status (st_cmd_t* cmd)
{
    uint8_t a_status, rtn_val;
     
    a_status = cmd->status;
    if ((a_status==STATUS_CLEARED)||(a_status==MOB_NOT_REACHED)||(a_status==MOB_DISABLE))
    {
        return CAN_STATUS_ERROR;
    }

    Can_set_mob(cmd->handle);
    a_status = can_get_mob_status();
    
    switch (a_status)
    {
        case MOB_NOT_COMPLETED:
            // cmd->status not updated
            rtn_val = CAN_STATUS_NOT_COMPLETED;
            break;
        //---------------      
        case MOB_RX_COMPLETED:     
        case MOB_RX_COMPLETED_DLCW:
            cmd->dlc = Can_get_dlc();
            can_get_data(cmd->pt_data);
            cmd->ctrl.rtr = Can_get_rtr();
            if (Can_get_ide()) // if extended frame
            {
                cmd->ctrl.ide = 1; // extended frame
                Can_get_ext_id(cmd->id.ext);
            }
            else // else standard frame
                {
                    cmd->ctrl.ide = 0;
                    Can_get_std_id(cmd->id.std);
                }
            // Status field of descriptor: 0x20 if Rx completed
            // Status field of descriptor: 0xA0 if Rx completed with DLCWarning    
            cmd->status = a_status;
            Can_mob_abort();        // Freed the MOB
            Can_clear_status_mob(); //   and reset MOb status
            rtn_val = CAN_STATUS_COMPLETED;
            break;
        //---------------      
        case MOB_TX_COMPLETED:     
            // Status field of descriptor: 0x40 if Tx completed
            cmd->status = a_status;
            Can_mob_abort();        // Freed the MOB
            Can_clear_status_mob(); //   and reset MOb status
            rtn_val = CAN_STATUS_COMPLETED;
            break;
        //---------------      
        default:
            // Status field of descriptor: (bin)000b.scfa if MOb error
            cmd->status = a_status;
            Can_mob_abort();        // Freed the MOB
            Can_clear_status_mob(); //   and reset MOb status
            rtn_val = CAN_STATUS_ERROR;
            break;
             
    } // switch (a_status...
 
    return (rtn_val);
}

/************************************************************************/
/* CAN                                                                  */
/************************************************************************/

/* can_tx_sync
 * 
 * Transmit a message over CAN and wait for transmit to complete.
 * 
 * Arguments: 
 *		ID, the CAN ID to send with
 *		buffer, an array of things to send
 *		buf_size, the size of the buffer
 * 
 * Returns:
 *		void	
 */
void can_tx(uint16_t id, uint8_t *buffer, uint8_t buf_size) {
	
	// Message command object
	st_cmd_t can_msg;
	
	// Configure TX mailbox
	can_msg.pt_data = buffer;	// point message object to the data buffer
	can_msg.ctrl.ide = 0;		// CAN 2.0A
	can_msg.dlc = buf_size;		// Number of data bytes (8 max) requested from remote node
	can_msg.id.std = id;		// Message ID
	can_msg.cmd = CMD_TX_DATA;	// Transmit data command object

	// Wait for MOb to configure (Must re-configure MOb for every transaction) and send request
	while(can_cmd(&can_msg) != CAN_CMD_ACCEPTED);
	
	// Wait for Tx to complete
	while(can_get_status(&can_msg) == CAN_STATUS_NOT_COMPLETED);
}

/* can_rx_sync
 * 
 * Receive a message over CAN and wait for receive to complete.
 * 
 * Arguments:
 *		ID, the CAN ID to send with
 *		buffer, an array of things to send
 *		buf_size, the size of the buffer
 *
 * Returns:
 *		the received array
 */
void can_rx_sync(uint16_t ID, uint8_t *buffer, uint8_t buf_size) {
	// Message command object
	st_cmd_t can_msg;
	
	// Configure TX mailbox
	can_msg.pt_data = buffer;			// point message object to the data buffer
	can_msg.ctrl.ide = 0;				// CAN 2.0A
	can_msg.dlc = buf_size;				// Number of data bytes (8 max) requested from remote node
	can_msg.id.std = ID;				// Message ID
	can_msg.cmd = CMD_RX_DATA_MASKED;	// Receive data command object

	// Wait for MOb to configure (Must re-configure MOb for every transaction) and send request
	while(can_cmd(&can_msg) != CAN_CMD_ACCEPTED);
	// Wait for Tx to complete
	while(can_get_status(&can_msg) == CAN_STATUS_NOT_COMPLETED);
}
    
    
    
    
    
    
    
    
    
    
    
    
    
    

