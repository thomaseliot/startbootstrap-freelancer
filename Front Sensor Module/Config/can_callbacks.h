/*
 * can_callbacks.h
 *
 * Callbacks used by the AC Fan Controller for CAN messages.
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#ifndef CAN_CALLBACKS_H_
#define CAN_CALLBACKS_H_

#include "cmr_constants.h"
#include "node_tasks.h"
#include "frtos_can.h"
#include "can_ids.h"
#include "can_structs.h"

// Callback functions, bound to a receive mailbox
void heartbeatCallback(CAN_packet packet);

#endif /* CAN_CALLBACKS_H_ */