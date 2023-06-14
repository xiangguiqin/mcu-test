#include "soft_i2c.h"
#include "aw2013.h"
#include "delay.h"
//#include <easyflash.h>
#include <stdio.h>
#include <string.h>
//#include "user_mb_app.h"
#if SYSTEM_SUPPORT_OS
#include "rtthread.h"
#endif
#include "main.h"
#include "fpga2mcu_uart.h"

soft_I2C_Port_Typedef soft_I2C_Port_aw2013 = {
	 .RCC_AHBPeriph_SDA       = RCC_AHBPeriph_GPIOF
	,.RCC_AHBPeriph_SCL       = RCC_AHBPeriph_GPIOF
	
	,.soft_I2C_SDA_Port       = GPIOF
	,.soft_I2C_SCL_Port       = GPIOF
	,.soft_I2C_SDA_Pin        = GPIO_Pin_7
	,.soft_I2C_SCL_Pin        = GPIO_Pin_6
	,.soft_I2C_SDA_PinSourcex = GPIO_PinSource7
	,.soft_I2C_SCL_PinSourcex = GPIO_PinSource6
	,.WaitAckEnable           = 1
};

const RegTypedef Aw2013Config[17] = {
	{0x01, 0X01},
	{0x30, 0X07},
	{0x31, 0x03},
	{0x32, 0x03},
	{0x33, 0x03},
	{0x34, 0Xff},
	{0x35, 0X00},
	{0x36, 0x00},
	{0x37, 0x22},
	{0x3A, 0x22},
	{0x3D, 0x22},
	{0x38, 0X00},
	{0x3B, 0X00},
	{0x3E, 0X00},
	{0x39, 0X20},
	{0x3C, 0X20},
	{0x3F, 0X20},
};


void aw2013_init(void)
{
	uint8_t device_reset = 0;
//	I2C_Err_TypeDef Err;

	eSoft_I2C_Init(&soft_I2C_Port_aw2013, AW2013_I2C_ADDR);
	
	eI2C_DeviceWriteOneByte(&soft_I2C_Port_aw2013, AW2013_SOFT_RESET_REG, AW2013_SOFT_RESET_VALUE);
	
	while(device_reset != 0x33)
	{
		eI2C_DeviceReadOneByte(&soft_I2C_Port_aw2013, AW2013_SOFT_RESET_REG, &device_reset);
		delay_ms(5);
	}
	
	
	for(uint8_t i=0; i<17; i++){
		eI2C_DeviceWriteOneByte(&soft_I2C_Port_aw2013, Aw2013Config[i].address, Aw2013Config[i].value);
	}

}

uint8_t uVideoLock;
uint8_t uUvcActive;
void aw2013_process(void)
{
	
	static uint8_t R=0,G=0,B=0;
	static uint8_t buf[3] = {0};
	
	rt_err_t rt_err_event;
	rt_uint32_t RecvedEvent;
	extern rt_event_t xOsEvent;
	
	rt_err_event = rt_event_recv( xOsEvent
				                 ,EV_UART_UVCACTIVE | EV_UART_VIDEOlOCK
				                 ,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR
				                 ,RT_WAITING_NO//,RT_WAITING_FOREVER
				                 ,&RecvedEvent
				                );
	if(rt_err_event == RT_EOK){
		if(RecvedEvent == EV_UART_UVCACTIVE){
//			uUvcActive = get_uart_data();
		}else if(RecvedEvent == EV_UART_VIDEOlOCK){
//			uVideoLock = get_uart_data();
		}
		
		if(uUvcActive && uVideoLock){
			R = 0;
			G = 255;
			B = 0;
		}else if((!uUvcActive) && uVideoLock){
			R = 255;
			G = 0;
			B = 0;
		}else if(uUvcActive && (!uVideoLock)){
			R = 0;
			G = 0;
			B = 255;
		}else if((!uUvcActive) && (!uVideoLock)){
			R = 255;
			G = 0;
			B = 0;
		}
		
		
		rt_enter_critical();
		if((!uUvcActive) && (!uVideoLock)){//³£ÁÁ
			uint8_t lcfg[3] = {0x03, 0x03, 0x03};
			eI2C_DeviceWrite(&soft_I2C_Port_aw2013, AW2013_LCFG0_REG, lcfg, 3);
		}else{//ºôÎüÊ½
			uint8_t lcfg[3] = {0x33, 0x33, 0x33};
			eI2C_DeviceWrite(&soft_I2C_Port_aw2013, AW2013_LCFG0_REG, lcfg, 3);
		}
		rt_exit_critical();
		
		buf[0] = R;
		buf[1] = G;
		buf[2] = B;
		rt_enter_critical();
		eI2C_DeviceWrite(&soft_I2C_Port_aw2013, AW2013_PWM0_REG, buf, 3);
		rt_exit_critical();
		
		rt_thread_mdelay(5);
		
		
		
	}
	
}	

