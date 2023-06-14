/**********************************************************************************
 * @file      soft_uart.c
 * @version   V2.0
 * $Revision: 2 $
 * $author:   �����
 * $Date:     2021/11/26
 * @brief
 * @note:
 * ����ʹ��IO��TIMERģ�⴮��,������߲�����115200,���ұ�����������ʹ�õ�ģ�⴮�ڲ�������ͬ;
 * ʹ�� SoftUart_Typedef�������⴮�ڲ���ʼ��;
 * Rx����ģ�⽨��ÿģ��һ��Rx��Ҫ��һ����ʱ�������ڶ�Ӧ�Ķ�ʱ������void SoftUartRxCallBack(void *SoftUartx);
 * Tx����ֻ�迪һ����ʱ��,����TMR2_IRQHandler����void SoftUartTxCallBack(void *SoftUartx)���������Ӧ��
 * (ʹ�� SoftUart_Typedef����)SoftUartx;
 * ����main�е���void SoftUartInit(void *SoftUartx)��������Ӧ�Ĵ��ڳ�ʼ��;
 * ������ϲ��輴��ʹ��void SoftUartSendData(SoftUart_Typedef *SoftUartx, uint8_t *p, uint8_t len)��������;

**********************************************************************************/

#include "soft_uart.h"

SoftUart_Typedef  SoftUart0 = {//ʹ��Rx����
	 .RCC_AHBPeriph_Tx       = RCC_AHBPeriph_GPIOB
	,.RCC_AHBPeriph_Rx       = RCC_AHBPeriph_GPIOB
	
	,.SoftUartTxPort         = GPIOB
	,.SoftUartRxPort         = GPIOB
	,.SoftUartTxPin          = GPIO_Pin_15
	,.SoftUartRxPin          = GPIO_Pin_5
	
	,.RxEXTI_PortSourcex     = EXTI_PortSourceGPIOB
	,.RxEXTI_PinSourcex      = 5
	,.EXTI_Linex             = EXTI_Line5
	,.EXTIx_IRQn             = EXTI4_15_IRQn
	
	,.SoftUartBaud           = 115200
	,.SoftUartWordLength     = 8+1	//(8����λ+1ֹͣλ)
	,.TxBitCnt               = 0
	,.RxBitCnt               = 0

	,.RxStart                = 0

	,.TxByteData             = 0
	,.RxByteData             = 0

	,.TxShiftReg             = 0
	,.RxShiftReg             = 0

	,.TxBusy                 = 0
	,.RxDone                 = 0

	,.RxEnable               = 1
};

void SoftUartSendByte(void *SoftUartx, uint8_t Data)
{
	SoftUart_Typedef *SoftUart = (SoftUart_Typedef *)SoftUartx;
	
	SoftUart->TxByteData = Data;		//Ҫ���͵�����д�뷢�ͻ���
	SoftUart->TxBusy = 1;		//��������
	
	uint32_t u32Clk = TIMER_GetModuleClock(TIMER1);
	TIMER_SET_CMP_VALUE(TIMER2, u32Clk/Baud);
	TIMER_Start(TIMER2);
	while(SoftUart->TxBusy);	//�ȴ��������
}

void SoftUartSendData(SoftUart_Typedef *SoftUartx, uint8_t *p, uint8_t len)
{
	uint8_t t;

	for(t=0; t<len; t++){
		SoftUartSendByte(SoftUartx, p[t]);

	}
}

//void SoftUartRiciveData(SoftUart_Typedef *SoftUartx, uint8_t *p, uint8_t *len)
//{
//	uint8_t t;
//	if(SoftUart0.RxDone){
//		SoftUart0.RxDone = 0;
//		
//		*p = SoftUart0.RxByteData;
//		(*len)++;
//	}
//}


void SoftUartInit(void *SoftUartx)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	
	SoftUart_Typedef *SoftUart = (SoftUart_Typedef *)SoftUartx;
	
	RCC_AHBPeriphClockCmd(SoftUart->RCC_AHBPeriph_Tx, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin   = SoftUart->SoftUartTxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(SoftUart->SoftUartTxPort, &GPIO_InitStructure);

	GPIO_WriteBit(SoftUart->SoftUartTxPort, SoftUart->SoftUartTxPin, Bit_SET);//����ߵ�ƽ
	
	if(SoftUart->RxEnable){
		
		RCC_AHBPeriphClockCmd(SoftUart->RCC_AHBPeriph_Rx, ENABLE);
		
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Pin   = SoftUart->SoftUartRxPin;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_Init(SoftUart->SoftUartTxPort, &GPIO_InitStructure);
		
		/* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		/* Connect EXTI5 Line to PB5 pin */
		SYSCFG_EXTILineConfig(SoftUart->RxEXTI_PortSourcex, SoftUart->RxEXTI_PinSourcex);
		
		/* Configure EXTI5 line */
		EXTI_InitStructure.EXTI_Line = SoftUart->EXTI_Linex;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		/* Enable and set EXTI5 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = SoftUart->EXTIx_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	
	SoftUartTimerInit();
}


void SoftUartTimerInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE); //ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period    = 208; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = 2; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�
	
	//�ж����ȼ�NVIC����
	/* Enable and set EXTI5 Interrupt */
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM16, DISABLE);
	
	
	
	
	
	/* Unlock protected registers */
    SYS_UnlockReg();
	
	/* Enable TIMER1 module clock */
    CLK_EnableModuleClock(TMR1_MODULE);
	/* Select TIMER1 module clock source */
	CLK->CLKSEL1 |= (CLK->CLKSEL1 & (~CLK_CLKSEL1_TMR1_S_Msk)) | CLK_CLKSEL1_TMR1_S_HIRC;
	
	/* Lock protected registers */
    SYS_LockReg();
	
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, Baud);
    TIMER_EnableInt(TIMER1);
	
	/* Enable Timer0 ~ Timer3 NVIC */
    NVIC_EnableIRQ(TMR1_IRQn);
//	TIMER_Start(TIMER1);
	TIMER_Stop(TIMER1);
	
	
	/* Unlock protected registers */
    SYS_UnlockReg();
	
	/* Enable TIMER3 module clock */
    CLK_EnableModuleClock(TMR2_MODULE);
	/* Select TIMER3 module clock source */
	CLK->CLKSEL1 |= (CLK->CLKSEL1 & (~CLK_CLKSEL1_TMR2_S_Msk)) | CLK_CLKSEL1_TMR2_S_HIRC;
	
	/* Lock protected registers */
    SYS_LockReg();
	
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, Baud);
    TIMER_EnableInt(TIMER2);
	
	/* Enable Timer0 ~ Timer3 NVIC */
    NVIC_EnableIRQ(TMR2_IRQn);
//	TIMER_Start(TIMER1);
	TIMER_Stop(TIMER2);
	
}

void SoftUartRxStartCallBack(SoftUart_Typedef *SoftUart)
{
//	SoftUart_Typedef *SoftUart = (SoftUart_Typedef *)SoftUartx;

	if(GPIO_GET_INT_FLAG(SoftUart->SoftUartRxPort, SoftUart->SoftUartRxPin))
    {
        GPIO_CLR_INT_FLAG(SoftUart->SoftUartRxPort, SoftUart->SoftUartRxPin);
        GPIO_DisableInt(SoftUart->SoftUartRxPort, SoftUart->SoftUartRxPinNum);
		
		TIMER_Stop(TIMER1);//Timer1
		TIMER_ClearIntFlag(TIMER1);
//		uint32_t u32Clk = TIMER_GetModuleClock(TIMER1);
		TIMER_SET_CMP_VALUE(TIMER1, TCMPR_BAUD_VALUE);//22118400/115200 = 192
		TIMER_Start(TIMER1);//Timer1 ��ʼ����
		SoftUart->RxStart = 1;		//��־���յ���ʼλ
		SoftUart->RxBitCnt = 9;		//��ʼ�����յ�����λ��(8������λ+1��ֹͣλ)
		
	}

}
void GPIOP2P3P4_IRQHandler(void)
{
	
	SoftUartRxStartCallBack(&SoftUart0);
	
//	SoftUartRxStartCallBack(&SoftUart1);
	
	
}


void SoftUartRxCallBack(SoftUart_Typedef *SoftUart)
{
//	SoftUart_Typedef *SoftUart = (SoftUart_Typedef *)SoftUartx;
	
	//====================== ģ�⴮�ڽ��ճ��� ========================================
	if(SoftUart->RxStart)		//���յ���ʼλ
	{
		if ((--SoftUart->RxBitCnt) == 0)		//������һ֡����
		{
			SoftUart->RxStart = 0;			//ֹͣ����
			SoftUart->RxByteData = SoftUart->RxShiftReg;		//�洢���ݵ�������
//			SoftUart->RxTimeOut = 1;
			SoftUart->RxDone = 1;	//��־���յ�һ֡����
			TIMER_Stop(TIMER1);	//Timer1 ֹͣ����
			GPIO_EnableInt(SoftUart->SoftUartRxPort, SoftUart->SoftUartRxPinNum, GPIO_INT_FALLING);	//����INT�ж�
		}
		else
		{
			SoftUart->RxShiftReg >>= 1;			  		//�ѽ��յĵ�b���� �ݴ浽 RxShiftReg(���ջ���)
			if((SoftUart->SoftUartRxPort->PIN) & (SoftUart->SoftUartRxPin))
				SoftUart->RxShiftReg |= 0x80;  	//shift RX data to RX buffer
		}
	}
}



void TMR1_IRQHandler(void)
{
	if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);
		
		SoftUartRxCallBack(&SoftUart0);
		ModbusReceive(&SoftUart0, &Modbus0);//��SoftUart0�յ������ݴ�ŵ�Modbus0��
		
//		SoftUartRxCallBack(&SoftUart1);		
//		ModbusReceive(&SoftUart1, &Modbus3);
		
//		SoftUartRxCallBack(&SoftUart2);		
//		ModbusReceive(&SoftUart2, &Modbus4);
	}
}


void SoftUartTxCallBack(void *SoftUartx)
{
	SoftUart_Typedef *SoftUart = (SoftUart_Typedef *)SoftUartx;
//	//====================== ģ�⴮�ڷ��ͳ��� ========================================
	if(SoftUart->TxBusy)					// ������, �˳�
	{
		if(SoftUart->TxBitCnt == 0)			//���ͼ�����Ϊ0 �������ֽڷ��ͻ�û��ʼ
		{
			SoftUart->SoftUartTxPort->DOUT &= ~SoftUart->SoftUartTxPin;//������ʼλ  send start bit
			SoftUart->TxShiftReg = SoftUart->TxByteData;		//�ѻ�������ݷŵ����͵�buff
			SoftUart->TxBitCnt = SoftUart->SoftUartWordLength;			//��������λ�� (8����λ+1ֹͣλ)
		}
		else						//���ͼ�����Ϊ��0 ���ڷ�������
		{
			if (--SoftUart->TxBitCnt == 0)	//���ͼ�������Ϊ0 �������ֽڷ��ͽ���
			{
				SoftUart->SoftUartTxPort->DOUT |= SoftUart->SoftUartTxPin;//��ֹͣλ����
				SoftUart->TxBusy = 0;		//����ֹͣ
				TIMER_Stop(TIMER2);	//Timer2 ֹͣ����
			}
			else
			{
				if(SoftUart->TxShiftReg&0x01)//write hight
					SoftUart->SoftUartTxPort->DOUT |= SoftUart->SoftUartTxPin;
				else
					SoftUart->SoftUartTxPort->DOUT &= (uint32_t)~SoftUart->SoftUartTxPin;//write low
				
				SoftUart->TxShiftReg >>= 1;
			}
		}
	}
}

/* 
* data reverse 
* ������RS485֮ǰ��npn9013�������Ƚ���������ȡ��(������ʼλ��ֹͣλ)
*/
void ReverseDataTxCallBack(void *SoftUartx)
{
	SoftUart_Typedef *SoftUart = (SoftUart_Typedef *)SoftUartx;
//	//====================== ģ�⴮�ڷ��ͳ��� ========================================
	if(SoftUart->TxBusy)					// ������, �˳�
	{
		if(SoftUart->TxBitCnt == 0)			//���ͼ�����Ϊ0 �������ֽڷ��ͻ�û��ʼ
		{
			SoftUart->SoftUartTxPort->DOUT |= SoftUart->SoftUartTxPin;//������ʼλ  send start bit
			SoftUart->TxShiftReg = ~(SoftUart->TxByteData);		//�ѻ�������ݷŵ����͵�buff
			SoftUart->TxBitCnt = SoftUart->SoftUartWordLength;			//��������λ�� (8����λ+1ֹͣλ)
		}
		else						//���ͼ�����Ϊ��0 ���ڷ�������
		{
			if (--SoftUart->TxBitCnt == 0)	//���ͼ�������Ϊ0 �������ֽڷ��ͽ���
			{
				SoftUart->SoftUartTxPort->DOUT &= ~SoftUart->SoftUartTxPin;//��ֹͣλ����
				SoftUart->TxBusy = 0;		//����ֹͣ
				TIMER_Stop(TIMER2);	//Timer2 ֹͣ����
			}
			else
			{
				if(SoftUart->TxShiftReg&0x01)//write hight
					SoftUart->SoftUartTxPort->DOUT |= SoftUart->SoftUartTxPin;
				else
					SoftUart->SoftUartTxPort->DOUT &= (uint32_t)~SoftUart->SoftUartTxPin;//write low
				
				SoftUart->TxShiftReg >>= 1;
			}
		}
	}
}




void TMR2_IRQHandler(void)
{
	if(TIMER_GetIntFlag(TIMER2) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER2);
		
		SoftUartTxCallBack(&SoftUart0);
		
		SoftUartTxCallBack(&SoftUart1);

//		ReverseDataTxCallBack(&SoftUart2);
//		SoftUartTxCallBack(&SoftUart2);
		
	}
}












