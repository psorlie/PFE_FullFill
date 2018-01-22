/*
 * observer.c
 *
 *  Created on: 21 déc. 2017
 *      Author: pierr
 */


#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "adc.h"
#include "comp.h"
#include "usart.h"
#include "gpio.h"
#include "sensor.h"
#include "observer.h"
#include "sensor.h"
#include "lptim.h"
#include "postman.h"
#include "bat_level.h"



void SystemClock_Config(void);
void Error_Handler(void);

//TODO : rajouter les librairies statiques (en <>) dans le .c plutot
//TODO : rajouter les déclarations de fonction statiques avant leurs définitions
static uint8_t nb_msg;

static ObserverState myObserverState = MEASURING_S;
//static pthread_t observerThread;
static ObserverEvent myEvent = MEASURE_OVER_E;
static uint32_t delay_repeat_msg = DEFAULT_DELAY_REPEAT_MSG;
static uint8_t myAck = DEFAULT_ACK;
static uint8_t myCode = DEFAULT_CODE;
static char* myMessage;
static uint8_t myId = DEFAULT_ID;
static uint8_t mySample = DEFAULT_SAMPLE;
static uint8_t myBatLevel = DEFAULT_BAT_LVL;
static uint8_t myFailedCounter = 0;
static uint32_t myListeningDelay = DEFAULT_LISTENING_DELAY;
static void observer_performAction(ObserverAction);
static void observer_check_nb_message_send();
static void observer_ask_meas();
static void analyse_id(uint8_t);
static void observer_update_id(char*);
static void observer_update_param(char*);


static ObserverTransition myObserverStateMachine[NB_STATE][NB_EVENT] = {

		[NAP_S][RTC_PERIODIC_ALARM_E] = {MEASURING_S, RTC_PERIODIC_ALARM_A},

		[MEASURING_S][MEASURE_OVER_E] = {CHECK_WORTH_S, EVALUATE_MEAS_A},

		[CHECK_WORTH_S][IS_WORTH_E] = {LISTEN_ACK_S, SEND_AND_WAIT_A},
		[CHECK_WORTH_S][IS_NOT_WORTH_E] = {NAP_S, RESTART_RTC_PERIODIC_ALARM_A},

		[LISTEN_ACK_S][LISTEN_E] = {ANALYSE_ACK_S, LISTEN_A},
		[LISTEN_ACK_S][WATCHDOG_LAUNCHED_E] = {CHECK_NB_REPEAT_S, CHECK_NB_MSG_ALREADY_SENT_A},

		[ANALYSE_ACK_S][ACK_DIFF_CURRENT_ID_E] = {LISTEN_ACK_S, REPEAT_NOT_MY_ID_A},
		[ANALYSE_ACK_S][ACK_EQUAL_CURRENT_ID_E] = {CHECK_NEW_PARAM_S, ANALYSE_TEXT_A},

		[CHECK_NB_REPEAT_S][NB_MSG_INF_MAX_E] = {LISTEN_ACK_S, SEND_AND_WAIT_A},
		[CHECK_NB_REPEAT_S][NB_MSG_SUP_MAX_E] = {NAP_S, RESTART_RTC_PERIODIC_ALARM_A},

		[CHECK_NEW_PARAM_S][TEXT_REPEAT_E] = {LISTEN_ACK_S, SEND_AND_WAIT_A},
		[CHECK_NEW_PARAM_S][TEXT_EMPTY_E] = {NAP_S, TEXT_EMPTY_A},
		[CHECK_NEW_PARAM_S][TEXT_UPDATE_E] = {NAP_S, TEXT_UPDATE_A},
		[CHECK_NEW_PARAM_S][BROKEN_E] = {DEATH_S, BROKEN_A},

		[CHECK_MSG_BEFORE_SLEEP_S][BOOL_MSG_TODAY_YES_E] = {SLEEP_S},
		[CHECK_MSG_BEFORE_SLEEP_S][BROKEN_E] = {SLEEP_S, SEND_OLD_VAL_A},

		[SLEEP_S][CURFEW_END_E] = {MEASURING_S, CURFEW_END_A},

		[NAP_S][CURFEW_BEGINNING_E] = {CHECK_MSG_BEFORE_SLEEP_S, CURFEW_BEGINNING_A},
		[MEASURING_S][CURFEW_BEGINNING_E] = {CHECK_MSG_BEFORE_SLEEP_S, CURFEW_BEGINNING_A},
		[CHECK_WORTH_S][CURFEW_BEGINNING_E] = {CHECK_MSG_BEFORE_SLEEP_S, CURFEW_BEGINNING_A},
		[LISTEN_ACK_S][CURFEW_BEGINNING_E] = {CHECK_MSG_BEFORE_SLEEP_S, CURFEW_BEGINNING_A},
		[ANALYSE_ACK_S][CURFEW_BEGINNING_E] = {CHECK_MSG_BEFORE_SLEEP_S, CURFEW_BEGINNING_A},
		[CHECK_NB_REPEAT_S][CURFEW_BEGINNING_E] = {CHECK_MSG_BEFORE_SLEEP_S, CURFEW_BEGINNING_A},
		[CHECK_NEW_PARAM_S][CURFEW_BEGINNING_E] = {CHECK_MSG_BEFORE_SLEEP_S, CURFEW_BEGINNING_A},

};

///////////////////////

static void* Observer_run();

//static void observer_performAction(ObserverAction);

static void observer_examinate_sample(uint8_t);




/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure the main internal regulator output voltage
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1|RCC_PERIPHCLK_LPTIM1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.LptimClockSelection = RCC_LPTIM1CLKSOURCE_PCLK;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler */
}
///////////////////////

/*static void Observer_mq_send(ObserverMqMsg message) {
     int check;
     ObserverMqMsgAdapter msg_adapter;
     mqd_t mail_queue;
     msg_adapter.msg = message;
     mail_queue = mq_open(MQ_NETWORK_CONFIGURATION_NAME, O_WRONLY);
     if(mail_queue <= -1) {
         perror("[Observer] - Couldn\'t open the network configuration MQ\n");
         strcpy(msg_adapter.buffer, "\0\0");
     } else {
         check = mq_send(mail_queue, msg_adapter.buffer, sizeof(msg_adapter.buffer), 0);
         if (check <= -1) {
             perror("[Observer] - Couldn\'t write in the MQ\n");
         }
     }
     check = mq_close(mail_queue);
}*/

/*static ObserverMqMsg Observer_mq_receive() {
     int check;
     mqd_t mail_queue;
     ObserverMqMsgAdapter msg_adapter;
     mail_queue = mq_open(MQ_NETWORK_CONFIGURATION_NAME, O_RDONLY);
     if(mail_queue == -1) {
         perror("[Observer] - Couldn\'t open the network configuration MQ\n");
         strcpy(msg_adapter.buffer,"\0\0");
     } else {
         check = mq_receive(mail_queue, msg_adapter.buffer, MAX_MQ_MSG_SIZE, 0);
         if(check == -1) {
             perror("[Observer] - MQ is empty\n");
         }
     }
     check = mq_close(mail_queue);
     return msg_adapter.msg;
}*/

void observer_init() {

	  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	  HAL_Init();

	  /* Configure the system clock */
	  SystemClock_Config();

	  /* Initialize all configured peripherals */
	  MX_GPIO_Init();
	  MX_ADC_Init();
	  MX_COMP1_Init();
	  MX_LPUART1_UART_Init();
	  MX_LPTIM1_Init();
	  HAL_Delay(30);

	  Observer_run();
}

static void observer_performAction(ObserverAction action) {
     switch(action) {
     case NOPE_A:
    		 break;
     case EVALUATE_MEAS_A:
    	 mySample = sensor_measure();
    	 observer_examinate_sample(mySample);
    	 break;
     case IS_WORTH_A:
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case LISTEN_A:
    	 analyse_id(myAck);
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case REPEAT_NOT_MY_ID_A:
    	 wait_and_free();
    	 postman_send(myId, mySample,myBatLevel);
    	 postman_receive(myListeningDelay);
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case CHECK_NB_MSG_ALREADY_SENT_A:
    	 observer_check_nb_message_send();
    	 ////perror("[Observer] - Unstable state machine\n");
    	 break;
     case SEND_AND_WAIT_A:
    	 postman_send(myId, mySample,myBatLevel);
    	 postman_receive(myListeningDelay);
    	 //perror("[Observer] - Sending and waiting \n");
    	 break;
     case ANALYSE_TEXT_A:
    	 analyse_param();
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case BROKEN_A:
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case TEXT_UPDATE_A:
    	 //myNewParam
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case TEXT_EMPTY_A:
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case CURFEW_BEGINNING_A:
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case CURFEW_END_A:
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case SEND_OLD_VAL_A:
    	 //perror("[Observer] - Unstable state machine\n");
    	 break;
     case RESTART_RTC_PERIODIC_ALARM_A:
    	 timer_start();
    	 break;
     case RTC_PERIODIC_ALARM_A:
    	 observer_ask_meas();
    	 break;
     default:
    	 break;
         //perror("[Observer] - Unstable state machine\n");
     }
}
static void* Observer_run() {
     ObserverAction action;
     while(myObserverState != DEATH_S) {
    	 	 action = myObserverStateMachine[myObserverState][myEvent].futurAction;
             myObserverState = myObserverStateMachine[myObserverState][myEvent].futurState;
             observer_performAction(action);
     }
     return (void*)0;
}
static void observer_examinate_sample(uint8_t sample) {
	uint8_t old_measure = sensor_get_old_measure();
	if(sample != old_measure){
		myEvent = IS_WORTH_E;
		mySample = sample;
	}else{
		myEvent =  IS_NOT_WORTH_E;
	}
}

void observer_wake_up() {
	myEvent = RTC_PERIODIC_ALARM_E;
}

void observer_has_received_nothing() {
	myEvent = WATCHDOG_LAUNCHED_E;
}

void observer_has_received(char* ptr) {
	myMessage = ptr;
	myEvent = LISTEN_E;
}

static void observer_check_nb_message_send()
{
	if (myFailedCounter  >= MAX_FAILED) {
		myEvent = NB_MSG_SUP_MAX_E;
		myFailedCounter = 0;
	} else {
		myEvent = NB_MSG_INF_MAX_E;
		myFailedCounter++;
	}
}
static void observer_ask_meas(){
	myEvent = MEASURE_OVER_E;
}
static void analyse_id(uint8_t nbr){
	char value;
	value = *myMessage;
	if(value == nbr){
		myEvent = ACK_EQUAL_CURRENT_ID_E;
	}else{
		myEvent = ACK_DIFF_CURRENT_ID_E;
	}
}

void wait_and_free(){
	free(myMessage);
	HAL_Delay(delay_repeat_msg);
	myEvent = ACK_DIFF_CURRENT_ID_E;
}

void analyse_param(){
	char* current_id = NULL;
	char* code = NULL;
	char* other_param = NULL;
	sscanf(myMessage, "[%s/%s/%s]", current_id, code, other_param);
	switch (myCode){
		case EMPTY_CODE:
			myEvent = TEXT_EMPTY_E;
		break;
		case UPDATE_CODE:
			myEvent = TEXT_UPDATE_E;
			if(strlen(other_param) <=1)
			{
				observer_update_id(other_param);
			}else {
				observer_update_param(other_param);
			}
		break;
		case BROKEN_CODE:
			myEvent = BROKEN_E;
		break;
		case REPEAT_CODE:
			myEvent = TEXT_REPEAT_E;
		break;
		default:
		break;
	}
}

static void observer_update_id(char* other_param){
	char value;
	value = *other_param;
	myId = (uint8_t) value;
	myEvent = TEXT_UPDATE_E;
}


static void observer_update_param(char* other_param){

}
