/*
 * stm32f1_xpt2046.c
 *
 *  Created on: Nov 17, 2016
 *      Author: louisz
 */
/*
 * This library allows to use the XPT2046 touch part of the TFT screen with any STM32F1 and is independent of ILI9341 library.
 *
 * The module uses the SPI bus and the pins are imposed (fitting power supply and GND must have been chosen aswell):
 *
 *  T_IRQ : PB3 : IRQ pin of the touch screen used to notify when screen is touched or released (currently not used)
 *  T_OUT : PA6 : Output from screen to SPI (MISO)
 *  T_IN  : PA7 : Input from SPI master (MOSI)
 *  T_CS  : PA11: Chip select for SPI
 *  T_CLK : PA5 : SPI Clock
 *  VCC   : 5V  : Power supply
 *  GND   : GND : Ground
 *
 *  How to use the XPT2046 touchscreen library:
 *
 *  - Make sure the USE_XPT2046 in config.h is correctly defined at 1
 *  - Initialise the library (init function)
 *  - Get the data from screen using getCoordinate or getAverageCoordinate (getAverageCoordinate is recommended for better precision)
 *
 */
#include "config.h"
#if USE_XPT2046
#include <stdarg.h>

#include "stm32f1_xpt2046.h"
#include "stm32f1xx_hal.h"
#include "stm32f1_spi.h"
#include "stm32f1_ili9341.h"
#include "stm32f1_gpio.h"
#include "macro_types.h"


// Définition du bit de départ (S n7)
#define CONTROL_BYTE_START									0b10000000
#define CONTROL_BYTE_NO_START								0b00000000

//Différents bits de selection des channel (A2-A0 bits 6-4)
// Selection byte pour la temp0 (controle température)
#define CONTROL_BYTE_CHANNEL_SELECT_TEMP0					0b00000000
// Other Temp input
#define CONTROL_BYTE_CHANNEL_SELECT_TEMP1					0b01110000
// Selection byte pour Y
#define CONTROL_BYTE_CHANNEL_SELECT_Y						0b00010000
// X
#define CONTROL_BYTE_CHANNEL_SELECT_X						0b01010000
// Cross panel measurements
#define CONTROL_BYTE_CHANNEL_SELECT_Z1						0b00110000
#define CONTROL_BYTE_CHANNEL_SELECT_Z2						0b01000000
// Selection batery monitor input byte
#define CONTROL_BYTE_CHANNEL_SELECT_VBAT					0b00100000
// Aux input
#define CONTROL_BYTE_CHANNEL_SELECT_AUX						0b01100000

//Choix du mode de conversion (bit MODE n3)
#define CONTROL_BYTE_MODE_12_BIT							0b00000000
#define CONTROL_BYTE_MODE_8_BIT								0b00001000

// Choix du mode Single Ended ou differential (bit SER/DFR n2)
#define CONTROL_BYTE_SD_DIFFERENTIAL						0b00000000
#define CONTROL_BYTE_SD_SINGLE_ENDED						0b00000100

// Power Down control mode select (bits PD1 et PD0 n1 et 0)
#define CONTROL_BYTE_POWER_DOWN_MODE_LOW_POWER_IRQ			0b00000000
#define CONTROL_BYTE_POWER_DOWN_MODE_REF_ON_ADC_OFF_NO_IRQ	0b00000001
#define CONTROL_BYTE_POWER_DOWN_MODE_REF_OFF_ADC_ON_IRQ		0b00000010
#define CONTROL_BYTE_POWER_DOWN_MODE_FULL_POWER_NO_IRQ		0b00000011

// Type control byte
typedef Uint8 controlByte_t;



#define XPT2046_CS_SET()			HAL_GPIO_TogglePin(PIN_CS_TOUCH)
#define XPT2046_CS_RESET()			HAL_GPIO_WritePin(PIN_CS_TOUCH,GPIO_PIN_RESET)

static Uint16 XPT2046_getReading(controlByte_t controlByte);
static void XPT2046_convertCoordinateScreenMode(Sint16 * pX, Sint16 * pY);

/**
 * @brief init function for XPT2046 lib
 */
void XPT2046_init(void){

	// Initialise SPI
	SPI_Init(XPT2046_SPI);
	BSP_GPIO_PinCfg(PIN_CS_TOUCH,GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_FREQ_HIGH);
	BSP_GPIO_PinCfg(PIN_IRQ_TOUCH,GPIO_MODE_INPUT,GPIO_PULLDOWN,GPIO_SPEED_FREQ_HIGH);
	XPT2046_CS_SET();

	XPT2046_getReading(CONTROL_BYTE_START
					   | CONTROL_BYTE_CHANNEL_SELECT_X
					   | CONTROL_BYTE_MODE_12_BIT
					   | CONTROL_BYTE_SD_DIFFERENTIAL
					   | CONTROL_BYTE_POWER_DOWN_MODE_LOW_POWER_IRQ);
}

/*
 * @brief Reconfigure SPI speed (currently not working)
 */
void XPT2046_setConfig(void){
	SPI_setBaudRate(XPT2046_SPI, SPI_BAUDRATEPRESCALER_256);
}

/*
 * @brief function to get coordinate on a chosen coordinate mode.
 * @param *pX : pointer to get the X coordinate
 * @param *pY : pointer to get the Y coordinate
 * @param coordinateMode : either XPT2046_COORDINATE_RAW or XPT2046_COORDINATE_SCREEN_RELATIVE
 * @return a boolean that shows whether or not the screen has been touched (and the coordinate returned is valid)
 */
bool_e XPT2046_getCoordinates(Sint16 * pX, Sint16 * pY, XPT2046_coordinateMode_e coordinateMode){
	Uint8 i, j;
	Sint16 allX[7] , allY[7];

	for (i=0; i < 7 ; i++){

		allY[i] = XPT2046_getReading(CONTROL_BYTE_START
										| CONTROL_BYTE_CHANNEL_SELECT_Y
										| CONTROL_BYTE_MODE_12_BIT
										| CONTROL_BYTE_SD_DIFFERENTIAL
										| CONTROL_BYTE_POWER_DOWN_MODE_LOW_POWER_IRQ);

		allX[i] = XPT2046_getReading(CONTROL_BYTE_START
										| CONTROL_BYTE_CHANNEL_SELECT_X
										| CONTROL_BYTE_MODE_12_BIT
										| CONTROL_BYTE_SD_DIFFERENTIAL
										| CONTROL_BYTE_POWER_DOWN_MODE_LOW_POWER_IRQ);
	}

	for (i=0; i < 4 ; i++){
		for (j=i; j < 7 ; j++) {
			Sint16 temp = allX[i];
			if(temp > allX[j]){
				allX[i] = allX[j];
				allX[j] = temp;
			}
			temp = allY[i];
			if(temp > allY[j]){
				allY[i] = allY[j];
				allY[j] = temp;
			}
		}
	}

	if(coordinateMode == XPT2046_COORDINATE_SCREEN_RELATIVE)
		XPT2046_convertCoordinateScreenMode(&(allX[3]), &(allY[3]));

	*pX = allX[3];
	*pY = allY[3];

#ifdef XPT2046_USE_PIN_IRQ_TO_CHECK_TOUCH
	if(!HAL_GPIO_ReadPin(PIN_IRQ_TOUCH))
		return TRUE;
	else
		return FALSE;
#else
	if(allX[3] == 4095 || allY[3] == 4095 || allX[3] == 0 || allY[3] == 0)
		return FALSE;
	else
		return TRUE;
#endif
}

/*
 * @brief function to get average coordinate on a chosen coordinate mode on a chosen number of values.
 * @param *pX : pointer to get the X coordinate
 * @param *pY : pointer to get the Y coordinate
 * @param nSamples : number of samples used for each returned values
 * @param coordinateMode : either XPT2046_COORDINATE_RAW or XPT2046_COORDINATE_SCREEN_RELATIVE
 * @return a boolean that shows whether or not the screen has been touched (and the coordinate returned is valid)
 */
bool_e XPT2046_getAverageCoordinates(Sint16 * pX, Sint16 * pY, Uint8 nSamples, XPT2046_coordinateMode_e coordinateMode){
	Uint8 nRead = 0;
	Sint32 xAcc = 0 , yAcc = 0;
	Sint16 x , y;

	while(nRead < nSamples){
		if(!XPT2046_getCoordinates(&x , &y, coordinateMode)){
			break;
		}
		xAcc += x;
		yAcc += y;
		nRead ++;
	}

	if(nRead == 0){
		return FALSE;
	}
	*pX = xAcc / nRead;
	*pY = yAcc / nRead;
	return TRUE;
}


/*
 * @brief private function used to read the SPI
 * @param controlByte controlByte used to read
 * @return data read from screen touch
 */
static Uint16 XPT2046_getReading(controlByte_t controlByte){

	Uint16 ret;

	XPT2046_CS_RESET();
	SPI_WriteNoRegister(XPT2046_SPI,controlByte);

	ret = SPI_ReadNoRegister(XPT2046_SPI);
	ret = (ret << 5) | (SPI_ReadNoRegister(XPT2046_SPI) >> 3);

	XPT2046_CS_SET();

	return ret;
}

/*
 * @brief private function used to convert coordinates from Raw to ScreenMode
 * @param *pX X coordinate of the data
 * @param *pY Y coordinate of the data
 */
static void XPT2046_convertCoordinateScreenMode(Sint16 * pX, Sint16 * pY){
	ILI931_Options_t screenOption = ILI9341_getOptions();
	Sint16 tempX = *pX, tempY = *pY;

	switch(screenOption.orientation){
	case ILI9341_Orientation_Portrait_1 :
		*pX = tempX * screenOption.width / 4096;
		*pY = tempY * screenOption.height / 4096;
		break;

	case ILI9341_Orientation_Portrait_2 :
		*pX = (4096 - tempX) * screenOption.width / 4096;
		*pY = (4096 - tempY) * screenOption.height / 4096;
		break;

	case ILI9341_Orientation_Landscape_1 :
		*pX = tempY * screenOption.width / 4096;
		*pY = tempX * screenOption.height / 4096;
		break;

	case ILI9341_Orientation_Landscape_2 :
		*pX = (4096 - tempY) * screenOption.width / 4096;
		*pY = (4096 - tempX) * screenOption.height / 4096;
		break;
	}
}

#endif

