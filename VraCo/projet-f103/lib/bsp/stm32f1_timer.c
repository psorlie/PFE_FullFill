/***
   * \file TIMER.c
   * \author S. Poiraud
   * \date 23 janvier 2014
   * \brief Contient les fonctions pour configurer, lancer et gerer les interruption du timer 2.
   */
#include "stm32f1_timer.h"
#include "stm32f1xx_hal.h"


#if USE_BSP_TIMER


static TIM_HandleTypeDef TIMER_HandleStructure[TIMER_ID_NB];	//Ce tableau contient les structures qui sont utilisées pour piloter chaque TIMER avec la librairie HAL.
static const TIM_TypeDef * instance_array[TIMER_ID_NB] = {TIM1, TIM2, TIM3, TIM4};
static const IRQn_Type nvic_irq_array[TIMER_ID_NB] = {TIM1_UP_IRQn, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn};
#define TIM_1_2_3_4_5_CLK    	800000								//Fréquence des timers en Hz.
#define NB_NSEC_PER_EVENT		(1000000000/TIM_1_2_3_4_5_CLK)		//Nombre de ns par évènement timer. Calculé à partir de la fréquence du timer.


/**
 * @brief	Initialisation et lancement du timer sélectionné.
 * 			Cette fonction lance de timer et le configure Il peut declencher une IT quand il y a debordement du timer.
 * @func 	void TIMER_run_us(void)
 * @param id du timer : cf timer_id_e
 * @param us temps en us code sur un 32bits non signe
 * @param enable_irq : TRUE : active les IT, FALSE ne les active pas. En cas d'activation des IT, l'utilisateur doit ecrire une fonction TIMERx_user_handler_it. Par defaut, ces fonctions ecrites dans ce fichier mais avec l'attribut weak (elles peuvent donc etre reecrites)
 * @post	Le timer et son horloge sont activés, ses interruptions autorisées, et son décompte lancé.
 */
void TIMER_run_us(timer_id_e timer_id, uint32_t us, bool_e enable_irq)
{
	// On active l'horloge du timer demandé.
	switch(timer_id)
	{
		case TIMER1_ID:
			__HAL_RCC_TIM1_CLK_ENABLE();
			break;
		case TIMER2_ID:
			__HAL_RCC_TIM2_CLK_ENABLE();
			break;
		case TIMER3_ID:
			__HAL_RCC_TIM3_CLK_ENABLE();
			break;
		case TIMER4_ID:
			__HAL_RCC_TIM4_CLK_ENABLE();
			break;
		default:
			break;

	}



	// Time base configuration
	TIMER_HandleStructure[timer_id].Instance = (TIM_TypeDef*)instance_array[timer_id]; //On donne le timer en instance à notre gestionnaire (Handle)

	//On détermine la fréquence des évènements comptés par le timer.
	uint32_t freq;
	if(timer_id == TIMER1_ID)
	{
		//Fréquence du TIMER1 est PCLK2 lorsque APB2 Prescaler vaut 1, sinon : PCLK2*2
		freq = HAL_RCC_GetPCLK2Freq();
		if((RCC->CFGR & RCC_CFGR_PPRE2) >> 11 != RCC_HCLK_DIV1)
			freq *= 2;
	}
	else
	{
		//Fréquence des TIMERS 2,3,4 est PCLK1 lorsque APB1 Prescaler vaut 1, sinon : PCLK1*2
		freq = HAL_RCC_GetPCLK1Freq();
		if((RCC->CFGR & RCC_CFGR_PPRE1) >> 8 != RCC_HCLK_DIV1)
			freq *= 2;
	}

	uint64_t nb_psec_per_event = 1000000000000/freq;
	uint64_t period = (((uint64_t)(us))*1000000)/nb_psec_per_event;

	if(period > 65536)
	{
		uint32_t prescaler = 1;
		while(period > 65536)
		{
			prescaler *= 2;
			period /= 2;
		}
		TIMER_HandleStructure[timer_id].Init.Prescaler 	= prescaler - 1;	//le prescaler du timer doit être enregistré avec un offset de -1.
		TIMER_HandleStructure[timer_id].Init.Period 	= period - 1;	//On compte de 0 à period-1
	}
	else
	{
		TIMER_HandleStructure[timer_id].Init.Prescaler 	= 0;
		TIMER_HandleStructure[timer_id].Init.Period 	= (uint32_t)period - 1;
	}

	TIMER_HandleStructure[timer_id].Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	TIMER_HandleStructure[timer_id].Init.CounterMode = TIM_COUNTERMODE_UP;

	// On applique les paramètres d'initialisation
	HAL_TIM_Base_Init(&TIMER_HandleStructure[timer_id]);

	if(enable_irq)
	{
		//acquittement des IT
		clear_it_status(timer_id);
		// On fixe les priorités des interruptions du timer PreemptionPriority = 0, SubPriority = 1 et on autorise les interruptions
		HAL_NVIC_SetPriority(nvic_irq_array[timer_id] , 0, 1);
		HAL_NVIC_EnableIRQ(nvic_irq_array[timer_id]);
	}

	// On autorise les interruptions
	HAL_TIM_Base_Start_IT(&TIMER_HandleStructure[timer_id]);

	// On lance le timer
	__HAL_TIM_ENABLE(&TIMER_HandleStructure[timer_id]);
}

uint16_t TIMER_read(timer_id_e timer_id)
{
	return __HAL_TIM_GET_COUNTER(&TIMER_HandleStructure[timer_id]);
}

/**
 * @brief	accesseur sur le handler.
 * @func 	void TIMER_get_phandler(void)
 *///Récupération de la structure du timer
TIM_HandleTypeDef * TIMER_get_phandler(timer_id_e timer_id)
{
	return &TIMER_HandleStructure[timer_id];
}

//L'attribut weak indique à l'éditeur de liens, lors de la compilation, que cette fonction sera ignorée s'il en existe une autre portant le même nom. Elle sera choisie par défaut d'autre fonction homonyme.
//Ainsi, si l'utilisateur définie sa propre TIMER1_user_handler_it_1ms(), elle sera appelée
//Sinon, aucun message d'erreur n'indiquera que cette fonction n'existe pas !
__weak void TIMER1_user_handler_it(void)
{

}

__weak void TIMER2_user_handler_it(void)
{

}

__weak void TIMER3_user_handler_it(void)
{

}

__weak void TIMER4_user_handler_it(void)
{

}



/**
 * @brief 	Routine d'interruption appelée AUTOMATIQUEMENT lorsque le timer 1 arrive a écheance.
 * @func 	void TIM1_IRQHandler(void)
 * @pre		Cette fonction NE DOIT PAS être appelée directement par l'utilisateur...
 * @post	Acquittement du flag d'interruption, et appel de la fonction de l'utilisateur : TIMER1_user_handler_it_1ms()
 * @note	Nous n'avons PAS le choix du nom de cette fonction, c'est comme ça qu'elle est nommée dans le fichier startup.s !
 */
void TIM1_UP_IRQHandler(void){
	if(__HAL_TIM_GET_IT_SOURCE(&TIMER_HandleStructure[TIMER1_ID], TIM_IT_UPDATE) != RESET) 	//Si le flag est levé...
	{
		__HAL_TIM_CLEAR_IT(&TIMER_HandleStructure[TIMER1_ID], TIM_IT_UPDATE);				//...On l'acquitte...
		TIMER1_user_handler_it();									//...Et on appelle la fonction qui nous intéresse
	}
}

void TIM2_IRQHandler(void){
	if(__HAL_TIM_GET_IT_SOURCE(&TIMER_HandleStructure[TIMER2_ID], TIM_IT_UPDATE) != RESET) 	//Si le flag est levé...
	{
		__HAL_TIM_CLEAR_IT(&TIMER_HandleStructure[TIMER2_ID], TIM_IT_UPDATE);				//...On l'acquitte...
		TIMER2_user_handler_it();									//...Et on appelle la fonction qui nous intéresse
	}
}

void TIM3_IRQHandler(void){
	if(__HAL_TIM_GET_IT_SOURCE(&TIMER_HandleStructure[TIMER3_ID], TIM_IT_UPDATE) != RESET) 	//Si le flag est levé...
	{
		__HAL_TIM_CLEAR_IT(&TIMER_HandleStructure[TIMER3_ID], TIM_IT_UPDATE);				//...On l'acquitte...
		TIMER3_user_handler_it();									//...Et on appelle la fonction qui nous intéresse
	}
}

void TIM4_IRQHandler(void){
	if(__HAL_TIM_GET_IT_SOURCE(&TIMER_HandleStructure[TIMER4_ID], TIM_IT_UPDATE) != RESET) 	//Si le flag est levé...
	{
		__HAL_TIM_CLEAR_IT(&TIMER_HandleStructure[TIMER4_ID], TIM_IT_UPDATE);				//...On l'acquitte...
		TIMER4_user_handler_it();									//...Et on appelle la fonction qui nous intéresse
	}
}

/**
 * @brief	acquite les IT sur le timer sélectionné.
 * 			
 * @func 	void clear_it_status(void)
 * @pre 	Le timer a ete initialise
 * @post	Le timer est acquité
 */
void clear_it_status(timer_id_e timer_id){
	switch(timer_id)
	{
		case TIMER1_ID:
			__HAL_TIM_CLEAR_IT(&TIMER_HandleStructure[TIMER1_ID], TIM_IT_UPDATE);
			break;
		case TIMER2_ID:
			__HAL_TIM_CLEAR_IT(&TIMER_HandleStructure[TIMER2_ID], TIM_IT_UPDATE);
			break;
		case TIMER3_ID:
			__HAL_TIM_CLEAR_IT(&TIMER_HandleStructure[TIMER3_ID], TIM_IT_UPDATE);
			break;
		case TIMER4_ID:
			__HAL_TIM_CLEAR_IT(&TIMER_HandleStructure[TIMER4_ID], TIM_IT_UPDATE);				//...On l'acquitte...
			break;
		default:
			break;

	}
}

/**
 * @brief	stop le timer selectionne.
 * 			
 * @func 	void stop_timer(void)
 * @pre 	Le timer a ete initialise
 * @post	Le timer est desactivés
 */
void stop_timer(timer_id_e timer_id){
	switch(timer_id)
	{
		case TIMER1_ID:
			__HAL_TIM_DISABLE(&TIMER_HandleStructure[TIMER1_ID]);
			break;
		case TIMER2_ID:
			__HAL_TIM_DISABLE(&TIMER_HandleStructure[TIMER2_ID]);
			break;
		case TIMER3_ID:
			__HAL_TIM_DISABLE(&TIMER_HandleStructure[TIMER3_ID]);
			break;
		case TIMER4_ID:
			__HAL_TIM_DISABLE(&TIMER_HandleStructure[TIMER4_ID]);				//...On l'acquitte...
			break;
		default:
			break;
	}

}

#endif

