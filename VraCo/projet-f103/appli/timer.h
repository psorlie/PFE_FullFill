/*
 * timerVraco.h
 *
 *  Created on: 23 juin 2016
 *      Author: Nirgal
 */

#ifndef TIMER_H_M3_
#define TIMER_H_M3_

void TIMER3_init_for_1ms(void);
void TIMER3_run(void);
void TIMER3_stop(void);
//extern void TIMER3_user_handler_it_1ms(void);
void TIMER3_user_handler_it_1ms(void);
void TIM3_IRQHandler(void);

#endif /* TIMER_H_M3_ */
