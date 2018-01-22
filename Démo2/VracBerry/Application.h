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
