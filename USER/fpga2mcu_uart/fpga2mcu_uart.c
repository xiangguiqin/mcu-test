#include "sys.h"
#if SYSTEM_SUPPORT_OS
#include "rtthread.h"
#endif
#include "main.h"
#include "fpga2mcu_uart.h"


rt_event_t xOsEvent;	//事件集控制块



void fpga2mcu_uart_init(uint32_t bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	
	xOsEvent = rt_event_create("xOsEvent", RT_IPC_FLAG_FIFO);
	
	
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	//GPIOA.15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource14,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource15,GPIO_AF_1);

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口1
	
	USART_Cmd(USART2, ENABLE);                    //使能串口1 
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
///	USART_ClearFlag(USART2,USART_FLAG_IDLE);
}

uint8_t RcvCnt = 0;
uint8_t RcvData[2] = {0};

extern uint8_t DownScaler;
extern uint8_t uVideoLock;
extern uint8_t uUvcActive;
void USART2_IRQHandler(void)                	//串口1中断服务程序
{
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	rt_interrupt_enter();
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		RcvData[RcvCnt++] =USART_ReceiveData(USART2);	//读取接收到的数据
		
		if(RcvCnt >= 2){
			RcvCnt = 0;
			switch(RcvData[0]){
				case 0x01:
					uUvcActive = get_uart_data();
					rt_event_send(xOsEvent, EV_UART_UVCACTIVE);
					break;
				case 0x02:
					uVideoLock = get_uart_data();
					rt_event_send(xOsEvent, EV_UART_VIDEOlOCK);
					break;
				case 0x03:
					DownScaler = get_uart_data();
					rt_event_send(xOsEvent, EV_UART_DOWNSCALER);
					break;
			}
//			rt_event_send(&xOsEvent, EV_UART_UPDATE);
		}
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	rt_interrupt_leave();
#endif
} 


void fpga2mcu_uart_process(void)
{
	rt_err_t rt_err_event;
	rt_uint32_t RecvedEvent;
	
	rt_err_event = rt_event_recv( xOsEvent
				                 ,EV_UART_UPDATE
				                 ,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR
				                 ,RT_WAITING_NO//,RT_WAITING_FOREVER
				                 ,&RecvedEvent
				                );
	if(rt_err_event == RT_EOK){
		if(RecvedEvent == EV_UART_UPDATE)
		{
			switch(RcvData[0])
			{
				case 0x01:
					rt_event_send(xOsEvent, EV_UART_UVCACTIVE);
					break;
				case 0x02:
					rt_event_send(xOsEvent, EV_UART_VIDEOlOCK);
					break;
				case 0x03:
					rt_event_send(xOsEvent, EV_UART_DOWNSCALER);
					break;
			}
		}
	}

}


uint8_t get_uart_data(void)
{
	return RcvData[1];
}
