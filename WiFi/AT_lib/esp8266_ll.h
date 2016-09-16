/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website http://esp8266at.com
 * \license MIT
 * \brief   Low level, platform dependant, part for communicate with ESP module and platform.
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef ESP8266_LL_H
#define ESP8266_LL_H 110

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f4xx_hal.h>

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/*    Edit file name to esp8266_ll.h and edit values for your platform    */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/


/* Include ESP layer */
#include "esp8266.h"



UART_HandleTypeDef huart;

/* ESP defines */
#define ESP_RTS_HIGH         1
#define ESP_RTS_LOW          0

/**
 * \brief  Initializes U(S)ART peripheral for ESP8266 communication
 * \note   This function is called from ESP stack
 * \param  baudrate: baudrate for USART you have to use when initializing USART peripheral
 * \retval Initialization status:
 *           - 0: Initialization OK
 *           - > 0: Initialization failed
 */
uint8_t ESP8266_LL_USARTInit(uint32_t baudrate);

/**
 * \brief  Sends data from ESP stack to ESP8266 module using USART
 * \param  *data: Pointer to data array which should be sent
 * \param  count: Number of data bytes to sent to module
 * \retval Sent status:
 *           - 0: Sent OK
 *           - > 0: Sent error
 */
uint8_t ESP8266_LL_USARTSend(uint8_t* data, uint16_t count);

/**
 * \brief  Sets software RTS pin on microcontroller
 * \param  dir: Pin direction
 * \retval None
 */
void ESP8266_LL_SetRTS(uint8_t dir);

/**
 * \brief  Initializes reset pin on platform
 * \note   Function is called from ESP stack module when needed
 * \note   Declared as macro 
 */
#define ESP8266_RESET_INIT        do {                  \
	GPIO_InitTypeDef GPIO_InitStruct;                   \
                                                        \
	GPIO_InitStruct.Pin = GPIO_PIN_5;                   \
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;         \
	GPIO_InitStruct.Pull = GPIO_PULLUP;                 \
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;            \
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);             \
} while (0)
	
/**
 * \brief  Sets reset pin LOW
 * \note   Function is called from ESP stack module when needed
 * \note   Declared as macro 
 */
#define ESP8266_RESET_LOW         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)

/**
 * \brief  Sets reset pin HIGH
 * \note   Function is called from ESP stack module when needed
 * \note   Declared as macro 
 */
#define ESP8266_RESET_HIGH        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
