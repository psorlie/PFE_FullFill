/*
 * stm32f1_pwm.h
 *
 *  Created on: 23 mai 2016
 *      Author: Nirgal
 */

#ifndef BSP_STM32F1_PWM_H_
#define BSP_STM32F1_PWM_H_
#include "config.h"
#if USE_PWM
	#include "stm32f1_timer.h"

	void PWM_test(void);

	void PWM_run(timer_id_e timer_id, uint16_t TIM_CHANNEL_x, bool_e negative_chanel, uint32_t period_us, uint32_t duty, bool_e remap);

	void PWM_configure_pin(timer_id_e timer_id, uint16_t TIM_CHANNEL_x, bool_e negative_chanel, bool_e enable, bool_e remap);


	void PWM_set_duty(timer_id_e timer_id, uint16_t TIM_CHANNEL_x, uint16_t duty);
#endif
#endif /* BSP_STM32F1_PWM_H_ */
