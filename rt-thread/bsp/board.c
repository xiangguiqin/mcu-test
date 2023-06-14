/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-24                  the first version
 */

#include <rthw.h>
#include <rtthread.h>
#include "sys.h"
//#include <cm_backtrace.h>

#define PLL_CLOCK       48000000
//#define _SYSTICK_PRI    (*(rt_uint8_t  *)(0xE000ED23UL))

extern uint32_t SystemCoreClock;

//static uint32_t _SysTick_Config(rt_uint32_t ticks)
//{
//    if ((ticks - 1) > 0xFFFFFF)
//    {
//        return 1;
//    }

//    SysTick->LOAD = ticks - 1;
//    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
//    _SYSTICK_PRI = 0xFF;
//    SysTick->VAL  = 0;
//    SysTick->CTRL = 0x07;

//    return 0;
//}

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
/*
 * Please modify RT_HEAP_SIZE if you enable RT_USING_HEAP
 * the RT_HEAP_SIZE max value = (sram size - ZI size), 1024 means 1024 bytes
 */
#define RT_HEAP_SIZE (10*1024)
static rt_uint8_t rt_heap[RT_HEAP_SIZE];

RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

void rt_os_tick_callback(void)
{
    rt_interrupt_enter();
    
    rt_tick_increase();

    rt_interrupt_leave();
}

void SysTick_Handler(void)
{
	rt_os_tick_callback();
}

extern void delay_init(void);
#ifdef RT_USING_CONSOLE
static int rt_uart_init(uint32_t bound);
#endif
/**
 * This function will initial your board.
 */
void rt_hw_board_init(void)
{
	/*
	 * #error "TODO 1: OS Tick Configuration."
     * 
     * TODO 1: OS Tick Configuration
     * Enable the hardware timer and call the rt_os_tick_callback function
     * periodically with the frequency RT_TICK_PER_SECOND. 
     */
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	delay_init();
#ifdef RT_USING_CONSOLE
	rt_uart_init(115200);
#endif
	/* CmBacktrace initialize */
//    cm_backtrace_init("CmBacktrace", "V1.0.0", "V1.0.0");
	
	
	
	
    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

#ifdef RT_USING_CONSOLE

static int rt_uart_init(uint32_t bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//GPIOA.9 10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART1, ENABLE);                    //使能串口1 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
///	USART_ClearFlag(USART1,USART_FLAG_IDLE);

	return 0;
}
//INIT_BOARD_EXPORT(rt_uart_init);



void rt_hw_console_output(const char *str)
{
	rt_enter_critical();
	
	while(*str != '\0')
	{
		
		if(*str == '\n')
		{
			while((USART1->ISR&0X40)==0){}//循环发送,直到发送完毕
			USART1->TDR = (uint8_t)'\n';
		}
		
		while((USART1->ISR&0X40)==0){}//循环发送,直到发送完毕
		USART1->TDR = (uint8_t)*str++;
	}
	
	rt_exit_critical();
}

#endif


#if 0
char rt_hw_console_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    int ch = -1;
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET){
        //USART_ClearITPendingBit(USART_DEBUG,  USART_FLAG_RXNE);
        ch = USART_ReceiveData(USART1) & 0xFF;
    }else{
        if(USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET){
            USART_ClearFlag(USART1,  USART_FLAG_ORE);
        }
        rt_thread_mdelay(10);
    }

    return ch;
}
#endif



