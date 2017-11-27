/*
 * matrix_keyboard.c
 *
 *  Created on: 24 août 2015
 *      Author: S. Poiraud
 */
#include "config.h"
#if USE_MATRIX_KEYBOARD
#include "matrix_keyboard.h"
#include "systick.h"

#ifdef CONFIG_PULL_UP
	#define DEFAULT_STATE 		(TRUE)
#define CONFIG_PULL	GPIO_PULLUP
#else
	#ifdef CONFIG_PULL_DOWN
		#define DEFAULT_STATE 	(FALSE)
		#define CONFIG_PULL	GPIO_PULLDOWN
	#else
		#error "Vous devez définir CONFIG_PULL_UP ou CONFIG_PULL_DOWN"
	#endif
#endif

//Portage...
#define GPIO_SET_OUTPUT(port, pin)	HAL_GPIO_Init(port, &(GPIO_InitTypeDef){pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH})
#define GPIO_SET_INPUT(port, pin)	HAL_GPIO_Init(port, &(GPIO_InitTypeDef){pin, GPIO_MODE_INPUT, CONFIG_PULL, GPIO_SPEED_FREQ_HIGH})
#define GPIO_WRITE					HAL_GPIO_WritePin
#define GPIO_READ					HAL_GPIO_ReadPin


//Fonctions privées
static void KEYBOARD_write_bit_output(uint8_t bit);
static uint8_t  KEYBOARD_get_inputs(void);


//Disposition des touches sur le clavier. (attention, ne correspond pas forcément à la disposition physique dans le bon ordre !)
const char default_keyboard_keys[16] = {
								'D','#','0','*',
								'C','9','8','7',
								'B','6','5','4',
								'A','3','2','1' };

char * keyboard_keys;
static bool_e initialized = FALSE;

/*
@function	Initialise le module keyboard.
@post		Les ports concernés sont configurés en entrée ou en sortie.
*/
void KEYBOARD_init(const char * new_keyboard_keys)
{
	MATRIX_KEYBOARD_HAL_CLOCK_ENABLE();
	GPIO_SET_INPUT(PORT_INPUT_0,PIN_INPUT_0);
	GPIO_SET_INPUT(PORT_INPUT_1,PIN_INPUT_1);
	GPIO_SET_INPUT(PORT_INPUT_2,PIN_INPUT_2);
	GPIO_SET_INPUT(PORT_INPUT_3,PIN_INPUT_3);

	KEYBOARD_write_bit_output(DEFAULT_STATE);
	if(new_keyboard_keys)
		keyboard_keys = (char *)new_keyboard_keys;
	else
		keyboard_keys = (char *)default_keyboard_keys;
	initialized = TRUE;
}





//Les positions des touches sur ce clavier sont probablement fausses...
//	mais elles permettent de montrer l'utilisation de la fonction KEYBOARD_init(const char * new_keyboard_keys);
const char custom_keyboard[16] =  {
										'0','4','8','C',
										'1','5','9','D',
										'2','6','A','E',
										'3','7','B','F' };
const char custom_keyboard_12_touchs[16]  =  {
										'X','X','X','X',
										'#','9','6','3',
										'0','8','5','2',
										'*','7','4','1' };

static volatile uint32_t t = 0;

//Cette fonction doit être appelée dans la boucle de tâche de fond
running_e DEMO_matrix_keyboard_process_main (bool_e ask_for_finish)
{
	typedef enum
	{
		INIT = 0,
		RUN
	}state_e;
	static state_e state = INIT;
	static char previous_key = 0;
	running_e ret;
	char current_key;

	ret = IN_PROGRESS;
	switch(state)
	{
		case INIT:
			Systick_add_callback_function(DEMO_matrix_keyboard_process_1ms);

			//A modifier en fonction du clavier utilisé : par défaut, personnalisé ou personnalisé 12 touches
			//KEYBOARD_init(NULL);						//Initialisation du clavier avec le clavier par défaut
			//KEYBOARD_init(custom_keyboard);			//Initialisation du clavier avec un clavier personnalisé
			KEYBOARD_init(custom_keyboard_12_touchs);	//Initialisation du clavier avec un clavier personnalisé 12 touches

			printf("To run this demo, you should plug a matrix keyboard on the right ports. See matrix_keyboard.h\n");
			printf("Warning: the defaults ports used by this drivers are not compatible with the presence of SD card or Motors!\n");
			state = RUN;
			break;
		case RUN:
			if(!t)	//A chaque fois que t vaut 0 (toutes les 10ms)...
			{
				t = 10;							//[ms] On recharge le chronomètre t pour 10ms...
				current_key = KEYBOARD_get_key();
				if(current_key != previous_key)
				{
					switch(current_key)
					{
						case NO_KEY:
							break;
						case MANY_KEYS:
							printf("Many keys pressed\n");
							break;
						default:
							printf("%c pressed\n", current_key);
							break;
					}
				}
				previous_key = current_key;
			}
			if(ask_for_finish)
			{
				state = INIT;
				Systick_remove_callback_function(DEMO_matrix_keyboard_process_1ms);
				ret = END_OK;
			}
			break;
		default:
			break;
	}
	return ret;
}

//Cette fonction doit être appelée toutes les ms.
void DEMO_matrix_keyboard_process_1ms(void)
{
	if(t)		//Si le chronomètre est "chargé", on décompte... (comme un minuteur de cuisine !)
		t--;
}







/*
@function	Vérifie qu'il y a appui sur une touche ou non.
@return		TRUE si une touche est appuyée, FALSE sinon.
*/
bool_e KEYBOARD_is_pressed(void)
{
	uint8_t ret;
	if(!initialized)
		return FALSE;

	KEYBOARD_write_bit_output(!DEFAULT_STATE);

	//Delay

	//Si l'un des ports n'est pas dans l'état par défaut, c'est qu'une touche est pressée.
	ret = (	(GPIO_READ(PORT_INPUT_0,PIN_INPUT_0) != DEFAULT_STATE) ||
			(GPIO_READ(PORT_INPUT_1,PIN_INPUT_1) != DEFAULT_STATE) ||
			(GPIO_READ(PORT_INPUT_2,PIN_INPUT_2) != DEFAULT_STATE) ||
			(GPIO_READ(PORT_INPUT_3,PIN_INPUT_3) != DEFAULT_STATE));

	KEYBOARD_write_bit_output(DEFAULT_STATE);

	return ret;
}





/*
@function	Renvoi le code ASCII de la touche pressée. En correspondance avec le tableau de codes ASCII.
@post		Cette fonction intègre un anti-rebond
@pre		Il est conseillé d'appeler cette fonction périodiquement (10ms par exemple)
@return		Retourne le caractère ASCII si UNE touche est pressée. Sinon, renvoie 0.
*/
char KEYBOARD_get_key(void)
{
	uint8_t i;
	uint8_t pressed = NO_KEY;
	uint8_t first_pressed_i = NO_KEY;
	uint8_t first_pressed_j = NO_KEY;
	if(!initialized)
		return FALSE;

	for(i=0;i<4;i++)
	{
		KEYBOARD_write_bit_output(DEFAULT_STATE);

		switch(i)
		{
			case 0:
				GPIO_SET_OUTPUT(PORT_OUTPUT_0,PIN_OUTPUT_0);
				GPIO_WRITE(PORT_OUTPUT_0,PIN_OUTPUT_0,!DEFAULT_STATE);
				break;
			case 1:
				GPIO_SET_OUTPUT(PORT_OUTPUT_1,PIN_OUTPUT_1);
				GPIO_WRITE(PORT_OUTPUT_1,PIN_OUTPUT_1,!DEFAULT_STATE);
				break;
			case 2:
				GPIO_SET_OUTPUT(PORT_OUTPUT_2,PIN_OUTPUT_2);
				GPIO_WRITE(PORT_OUTPUT_2,PIN_OUTPUT_2,!DEFAULT_STATE);
				break;
			case 3:
				GPIO_SET_OUTPUT(PORT_OUTPUT_3,PIN_OUTPUT_3);
				GPIO_WRITE(PORT_OUTPUT_3,PIN_OUTPUT_3,!DEFAULT_STATE);
				break;
			default:
				return 0;
				break;	//Ne doit pas se produire.
		}


		//Acquisition entrées.
		pressed = KEYBOARD_get_inputs();
		if(pressed == MANY_KEYS)
			return MANY_KEYS;
		if(pressed != NO_KEY)	//Si une touche est appuyée.
		{
			if(first_pressed_j != NO_KEY)	//si une touche a déjà été enregistrée
				return NO_KEY;	//On renvoie 0.
			//Sinon on enregistre la touche pressée (ligne et colonne !).
			first_pressed_j = pressed;	//Une touche est appuyée, on la sauvegarde.
			first_pressed_i = i;
		}

	}

	//Fin du balayage.
	if(first_pressed_j == NO_KEY)	//Si aucune touche appuyée.
		return NO_KEY;					//On renvoie 0.

	return keyboard_keys[first_pressed_i*4+first_pressed_j];
}

/*
@function 	Fonction privée. Retourne la lecture des entrées si UNE entrée est à l'état pressée. Sinon, renvoie NO_KEY.
*/
static uint8_t  KEYBOARD_get_inputs(void)
{
	uint8_t inputs;
	inputs = 	((GPIO_READ(PORT_INPUT_0,PIN_INPUT_0) != DEFAULT_STATE)?1:0)|
				((GPIO_READ(PORT_INPUT_1,PIN_INPUT_1) != DEFAULT_STATE)?2:0)|
				((GPIO_READ(PORT_INPUT_2,PIN_INPUT_2) != DEFAULT_STATE)?4:0)|
				((GPIO_READ(PORT_INPUT_3,PIN_INPUT_3) != DEFAULT_STATE)?8:0);
	switch(inputs)
	{
		case 0b0000:	return NO_KEY;		break;
		case 0b0001:	return 0;			break;
		case 0b0010:	return 1;			break;
		case 0b0100:	return 2;			break;
		case 0b1000:	return 3;			break;
		default:		return MANY_KEYS;	break;	//Plusieurs touches sont appuyées.
	}
}

static void KEYBOARD_write_bit_output(uint8_t bit)
{
	if(bit == DEFAULT_STATE)
	{
		GPIO_SET_INPUT(PORT_OUTPUT_0,PIN_OUTPUT_0);
		GPIO_SET_INPUT(PORT_OUTPUT_1,PIN_OUTPUT_1);
		GPIO_SET_INPUT(PORT_OUTPUT_2,PIN_OUTPUT_2);
		GPIO_SET_INPUT(PORT_OUTPUT_3,PIN_OUTPUT_3);
	}
	else
	{
		GPIO_WRITE(PORT_OUTPUT_0,PIN_OUTPUT_0,bit);
		GPIO_WRITE(PORT_OUTPUT_1,PIN_OUTPUT_1,bit);
		GPIO_WRITE(PORT_OUTPUT_2,PIN_OUTPUT_2,bit);
		GPIO_WRITE(PORT_OUTPUT_3,PIN_OUTPUT_3,bit);
		GPIO_SET_OUTPUT(PORT_OUTPUT_0,PIN_OUTPUT_0);
		GPIO_SET_OUTPUT(PORT_OUTPUT_1,PIN_OUTPUT_1);
		GPIO_SET_OUTPUT(PORT_OUTPUT_2,PIN_OUTPUT_2);
		GPIO_SET_OUTPUT(PORT_OUTPUT_3,PIN_OUTPUT_3);
	}
}
#endif