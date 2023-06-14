#include "sys.h"
#if SYSTEM_SUPPORT_OS
#include "rtthread.h"
#endif
#include "main.h"
#include "fpga2mcu_uart.h"


rt_event_t xOsEvent;	//�¼������ƿ�



void fpga2mcu_uart_init(uint32_t bound)
{
	//GPIO�˿�����
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

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������1
	
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
///	USART_ClearFlag(USART2,USART_FLAG_IDLE);
}

uint8_t RcvCnt = 0;
uint8_t RcvData[2] = {0};

extern uint8_t DownScaler;
extern uint8_t uVideoLock;
extern uint8_t uUvcActive;
void USART2_IRQHandler(void)                	//����1�жϷ������
{
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	rt_interrupt_enter();
#endif
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		RcvData[RcvCnt++] =USART_ReceiveData(USART2);	//��ȡ���յ�������
		
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
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
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
