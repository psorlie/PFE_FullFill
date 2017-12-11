/*
 * Application.h
 *
 *  Created on: 19 nov. 2017
 *      Author: sorlie
 */

#ifndef SRC_APPLICATION_H_
#define SRC_APPLICATION_H_

#include "DispenserManager.h"

extern void Application_display_all_dispenser(Dispenser*);

extern void Application_update_dispenser(Dispenser*);

extern void Application_detailed_dispenser(Dispenser*);

// extern void Application_set_network_configuration(Network*);

// extern void Application_set_dispenser_configuration();

extern void Application_warn_dirty_dispenser(Dispenser_Id);

extern void Application_warn_empty_dispenser(Dispenser_Id);

extern void Application_warn_empty_battery(Dispenser_Id);

extern void Application_warn_dispenser_is_broken(Dispenser_Id);

#endif /* SRC_APPLICATION_H_ */
