/*
 * Application.h
 *
 *  Created on: 19 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_APPLICATION_H_
#define SRC_APPLICATION_H_

#include "DispenserManager.h"
#include "Dispenser.h"

/**
 * @brief is called when the user ask the current value of a dispenser and it gets its return.
 * Should also be called when this dispenser is updated (physically or virtually like a new product name
 * but it's not :/
 *
 * @param[in] Dispenser* pointer of the asked dispenser
 */
void Application_send_update(Dispenser*);

/**
 * @brief is called when a new dispenser is lit and we receive its first msg.
 * should create a new button and ask for a a new product name on the IHM : the new name isn't implemented
 *
 * @param[in] Dispenser* pointer of the new dispenser
 */
void Application_send_new(Dispenser*);

/**
 * @brief is called when the user ask the detail about a specified dispenser and we return it.
 *
 * @param[in] Dispenser* pointer of the specified dispenser
 */
void Application_send_detail(Dispenser*);

/**************************************************************
 * Still not implemented - some cleaning in the header is due *
 **************************************************************/
//TODO : clean the header, some functions are probably useless
extern void Application_display_all_dispenser(Dispenser*);

extern void Application_update_dispenser(Dispenser*);

extern void Application_detailed_dispenser(Dispenser*);

// extern void Application_set_network_configuration(Network*);

extern void Application_warn_dirty_dispenser(Dispenser_Id);

extern void Application_warn_empty_dispenser(Dispenser_Id);

extern void Application_warn_empty_battery(Dispenser_Id);

extern void Application_warn_dispenser_is_lost(Dispenser_Id);

extern void Application_warn_dispenser_is_broken(Dispenser_Id);

extern void Application_add_new_dispenser(Dispenser*);

extern void Application_warn_configuration_accepted();

extern void Application_send_current_configuration(uint32_t, uint32_t, uint16_t, uint16_t);

extern void	Application_warn_configuration_refused();

#endif /* SRC_APPLICATION_H_ */
