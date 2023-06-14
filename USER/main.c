/**
  ******************************************************************************
  * @file    			main.c
  * @author  			FMD AE
  * @brief   		
  * @version 			V1.0.0           
  * @data		 		2021-07-01
	******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "main.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "aw2013.h"
#if SYSTEM_SUPPORT_OS
#include "rtthread.h"
#endif
#include "fpga2mcu_uart.h"

/******************************************************************************/
rt_thread_t av_main_tid = RT_NULL;
static rt_thread_t test_tid = RT_NULL;

extern uint8_t LogicOutputSel;
/* Private functions ---------------------------------------------------------*/
static void MX_GPIO_Init(void);


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
RCC_ClocksTypeDef RCC_Clocks;
uint8_t CLKSource;
int main(void)
{
//	delay_init();
//	uart_init(115200);	 //串口初始化为115200
//	LED_Init();
	
	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	
	
	fpga2mcu_uart_init(115200);
	

	
	
	RCC_GetClocksFreq(&RCC_Clocks);
	CLKSource = RCC_GetSYSCLKSource();
	//if(LogicOutputSel == 1)
	//{
		GPIO_SetBits(DUT_RST_GPIO_Port, DUT_RST_Pin);
		rt_thread_mdelay(100);
		GPIO_ResetBits(DUT_RST_GPIO_Port, DUT_RST_Pin);
		rt_thread_mdelay(100);
		GPIO_SetBits(DUT_RST_GPIO_Port, DUT_RST_Pin);
	//}
	//else
	//{
		rt_thread_mdelay(100);
	//}
	
	rt_enter_critical();
	av_main_tid = rt_thread_create( "av_main_thread"
									,GsvMain_thread_entry
									,RT_NULL
									,AV_MAIN_THREAD_STACK_SIZE
									,AV_MAIN_THREAD_PRIORITY
									,AV_MAIN_THREAD_TIMESLICE
								  );
	rt_exit_critical();
	if(av_main_tid != RT_NULL){
		rt_thread_startup(av_main_tid);
	}
	
	rt_enter_critical();
	test_tid = rt_thread_create( "test_thread"
						        ,test_thread_entry
						        ,RT_NULL
						        ,TEST_THREAD_STACK_SIZE
						        ,TEST_THREAD_PRIORITY
						        ,TEST_THREAD_TIMESLICE
							   );
	rt_exit_critical();
	if(test_tid != RT_NULL)
		rt_thread_startup(test_tid);
	
	/* Infinite loop */
	while (1)
	{
		rt_thread_mdelay(500);
	}
}


static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;
	
	/* GPIO Ports Clock Enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | \
	                      RCC_AHBPeriph_GPIOB | \
	                      RCC_AHBPeriph_GPIOF, ENABLE);
	
	/*Configure GPIO pin Output Level */
	GPIO_SetBits(GPIOA, DUT_RST_Pin|LED0_Pin|LED1_Pin|LED2_Pin);
	
	/*Configure GPIO pin Output Level */
	GPIO_SetBits(GPIOB, I2C_SCL_Pin|I2C_SDA_Pin);
	
	/*Configure GPIO pin : DUT_RST_Pin */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = DUT_RST_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(DUT_RST_GPIO_Port, &GPIO_InitStruct);
	
	/*Configure GPIO pins : LED0_Pin LED1_Pin LED2_Pin */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = LED0_Pin|LED1_Pin|LED2_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/*Configure GPIO pin : KEY0_Pin */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = KEY0_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(KEY0_GPIO_Port, &GPIO_InitStruct);
	
	/*Configure GPIO pins : AVMUTE_Pin INT_Pin */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = AVMUTE_Pin|INT_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*Configure GPIO pins : I2C_SCL_Pin I2C_SDA_Pin */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = I2C_SCL_Pin|I2C_SDA_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*Configure GPIO pins : downscaler */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = iDownScalerPin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(iDownScalerPort, &GPIO_InitStruct);
	
	/**/
//	HAL_I2CEx_EnableFastModePlus(SYSCFG_CFGR1_I2C_FMP_PB8);
	
	/**/
//	HAL_I2CEx_EnableFastModePlus(SYSCFG_CFGR1_I2C_FMP_PB9);
	
}
#include "av_config.h"
extern AvPort gsv2k11Ports[];
static void test_thread_entry(void *parameter)
{
//	rt_enter_critical();
	aw2013_init();
//	rt_exit_critical();
	while(1)
	{
		rt_enter_critical();
		
//		fpga2mcu_uart_process();
		aw2013_process();
		
		rt_exit_critical();

		rt_thread_mdelay(5);
//		delay_ms(500);
	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT FMD *****END OF FILE****/
