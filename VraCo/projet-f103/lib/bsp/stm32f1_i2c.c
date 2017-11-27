/**	
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * | 
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |  
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * | 
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#include "config.h"
#if USE_I2C
#include "stm32f1_i2c.h"
#include "stm32f1_gpio.h"


/* Private defines */
#define I2C_TRANSMITTER_MODE   0
#define I2C_RECEIVER_MODE      1
#define I2C_ACK_ENABLE         1
#define I2C_ACK_DISABLE        0

typedef enum
{
	I2C1_ID = 0,
	I2C2_ID,
	I2C_NB
}I2C_ID_e;

static I2C_HandleTypeDef  hi2c[I2C_NB];

/**
 * @brief  Initializes I2C
 * @param  *I2Cx: I2C used
 * @param  pinspack: Pins used. This parameter can have a value of @ref TM_I2C_PinsPack_t enumeration
 * @param  clockSpeed: Clock speed for SCL in Hertz
 * @retval None
 */
void I2C_Init(I2C_TypeDef* I2Cx, uint32_t clockSpeed)
{
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);


	if(id == I2C1_ID)
	{
		__HAL_RCC_AFIO_CLK_ENABLE();
		__HAL_RCC_I2C1_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_AFIO_REMAP_I2C1_ENABLE();		//I2C1 sur PB8 et PB9.
		BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_8, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM);
		BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_9, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM);
	}
	else
	{
		__HAL_RCC_AFIO_CLK_ENABLE();
		__HAL_RCC_I2C2_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		//I2C2 sur PB10 et PB11.
		BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_10, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM);
		BSP_GPIO_PinCfg(GPIOB, GPIO_PIN_11, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM);
	}

	hi2c[id].Instance = I2Cx;
	hi2c[id].Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c[id].Init.ClockSpeed = clockSpeed;
	hi2c[id].Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c[id].Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c[id].Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c[id].Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	hi2c[id].Init.OwnAddress1 = 0x00;
	hi2c[id].Init.OwnAddress2 = 0x00;
	hi2c[id].State = HAL_I2C_STATE_RESET;
	HAL_I2C_Init(&hi2c[id]);
}

/**
 * @brief  Reads single byte from slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to read from
 * @retval Data from slave
 */
uint8_t I2C_Read(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg)
{
	uint8_t received_data;
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);
	HAL_I2C_Mem_Read(&hi2c[id],address,reg,I2C_MEMADD_SIZE_8BIT,&received_data,1,100);
	return received_data;
}

/**
 * @brief  Reads multi bytes from slave
 * @param  *I2Cx: I2C used
 * @param  uint8_t address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  uint8_t reg: register to read from
 * @param  uint8_t *data: pointer to data array to store data from slave
 * @param  uint8_t count: how many bytes will be read
 * @retval None
 */
void I2C_ReadMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count)
{
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);
	HAL_I2C_Mem_Read(&hi2c[id],address,reg,I2C_MEMADD_SIZE_8BIT,data,count,100);
}

/**
 * @brief  Reads byte from slave without specify register address
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @retval Data from slave
 */
uint8_t I2C_ReadNoRegister(I2C_TypeDef* I2Cx, uint8_t address)
{
	uint8_t data;
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);
	HAL_I2C_Master_Receive(&hi2c[id],address,&data,1,100);
	return data;
}

/**
 * @brief  Reads multi bytes from slave without setting register from where to start read
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  *data: pointer to data array to store data from slave
 * @param  count: how many bytes will be read
 * @retval None
 */
void I2C_ReadMultiNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint16_t count)
{
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);
	HAL_I2C_Master_Receive(&hi2c[id],address,data,count,100);
}

/**
 * @brief  Writes single byte to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  data: data to be written
 * @retval None
 */
void I2C_Write(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t data)
{
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);
	HAL_I2C_Mem_Write(&hi2c[id],address,reg,I2C_MEMADD_SIZE_8BIT,&data,1,100);
}

/**
 * @brief  Writes multi bytes to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  *data: pointer to data array to write it to slave
 * @param  count: how many bytes will be written
 * @retval None
 */
void I2C_WriteMulti(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count)
{
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);
	HAL_I2C_Mem_Write(&hi2c[id],address,reg,I2C_MEMADD_SIZE_8BIT,data,count,100);
}

/**
 * @brief  Writes byte to slave without specify register address
 *
 *         Useful if you have I2C device to read like that:
 *            - I2C START
 *            - SEND DEVICE ADDRESS
 *            - SEND DATA BYTE
 *            - I2C STOP
 *
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  data: data byte which will be send to device
 * @retval None
 */
void I2C_WriteNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t data)
{
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);
	HAL_I2C_Master_Transmit(&hi2c[id],address,&data,1,100);
}

/**
 * @brief  Writes multi bytes to slave without setting register from where to start write
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  *data: pointer to data array to write data to slave
 * @param  count: how many bytes you want to write
 * @retval None
 */
void I2C_WriteMultiNoRegister(I2C_TypeDef* I2Cx, uint8_t address, uint8_t* data, uint16_t count)
{
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);
	HAL_I2C_Master_Transmit(&hi2c[id],address,data,count,100);
}

/**
 * @brief  Checks if device is connected to I2C bus
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @retval Device status:
 *            - 0: Device is not connected
 *            - > 0: Device is connected
 */
bool_e I2C_IsDeviceConnected(I2C_TypeDef* I2Cx, uint8_t address)
{
	assert(I2Cx == I2C1 || I2Cx == I2C2);
	I2C_ID_e id = ((I2Cx == I2C2)?I2C2_ID:I2C1_ID);
	return (HAL_I2C_IsDeviceReady(&hi2c[id],address,2,100) == HAL_OK)?TRUE:FALSE;
}

#endif

