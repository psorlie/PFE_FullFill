/*
 *	Club Robot ESEO 2009 - 2010
 *	Chomp
 *
 *	Fichier : QS_sys.c
 *	Package : Qualite Soft
 *	Description : Configuration du STM32 - Horloges - Démarrage
 *	Auteur : Gwenn
 *	Version 20100421
 */

#ifndef QS_SYS_H
	#define QS_SYS_H
	#include "stm32f1xx_hal.h"
	#include "stm32f1_uart.h"

	void SYS_NVIC_PriorityGrouping(void);
	void SYS_ClockConfig(void);
	void SYS_set_std_usart(uart_id_e in, uart_id_e out, uart_id_e err);



#endif /* ndef QS_SYS_H */
