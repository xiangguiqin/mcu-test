/**
  ******************************************************************************
  * File Name          : main.hpp
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2020 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys.h"
#include "rtthread.h"
#include "fpga2mcu_uart.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define DUT_RST_Pin GPIO_Pin_0
#define DUT_RST_GPIO_Port GPIOA
#define LED0_Pin GPIO_Pin_3
#define LED0_GPIO_Port GPIOA
#define LED1_Pin GPIO_Pin_4
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_Pin_5
#define LED2_GPIO_Port GPIOA
#define KEY0_Pin GPIO_Pin_6
#define KEY0_GPIO_Port GPIOA
#define AVMUTE_Pin GPIO_Pin_5
#define AVMUTE_GPIO_Port GPIOB
#define INT_Pin GPIO_Pin_6
#define INT_GPIO_Port GPIOB
#define I2C_SCL_Pin GPIO_Pin_8
#define I2C_SCL_GPIO_Port GPIOB
#define I2C_SDA_Pin GPIO_Pin_9
#define I2C_SDA_GPIO_Port GPIOB

#define iDownScalerPin GPIO_Pin_5
#define iDownScalerPort GPIOB

#define setbit(x, y)          (x|=(1<<y))
#define clrbit(x, y)          (x&=~(1<<y))
#define reversebit(x, y)      (x^=(1<<y))
#define getbit(x, y)          ((x) >> (y)&1)

typedef enum{
	EV_UART_UPDATE      = 1<<0,
    EV_UART_DOWNSCALER  = 1<<2,
	EV_UART_UVCACTIVE   = 1<<3,
	EV_UART_VIDEOlOCK   = 1<<4,
	EV_UPDATE           = 1<<5,
} eUartEventType;
/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

//任务优先级
#define AV_MAIN_THREAD_PRIORITY			3
//任务堆栈大小	
#define AV_MAIN_THREAD_STACK_SIZE 		2048
#define AV_MAIN_THREAD_TIMESLICE		50
//任务句柄
extern rt_thread_t av_main_tid;
//任务函数
extern void GsvMain_thread_entry(void *parameter);


#define TEST_THREAD_PRIORITY			4
#define TEST_THREAD_STACK_SIZE			200
#define TEST_THREAD_TIMESLICE			10
extern rt_thread_t test_tid;
extern void test_thread_entry(void *parameter);


/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
