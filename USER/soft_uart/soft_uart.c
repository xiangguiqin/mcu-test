/**********************************************************************************
 * @file      soft_uart.c
 * @version   V2.0
 * $Revision: 2 $
 * $author:   秦湘桂
 * $Date:     2021/11/26
 * @brief
 * @note:
 * 本例使用IO和TIMER模拟串口,建议最高波特率115200,并且本例下面所有使用的模拟串口波特率相同;
 * 使用 SoftUart_Typedef定义虚拟串口并初始化;
 * Rx功能模拟建议每模拟一个Rx需要开一个定时器，并在对应的定时器调用void SoftUartRxCallBack(void *SoftUartx);
 * Tx功能只需开一个定时器,需在TMR2_IRQHandler调用void SoftUartTxCallBack(void *SoftUartx)函数传入对应的
 * (使用 SoftUart_Typedef定义)SoftUartx;
 * 需在main中调用void SoftUartInit(void *SoftUartx)函数对相应的串口初始化;
 * 完成以上步骤即可使用void SoftUartSendData(SoftUart_Typedef *SoftUartx, uint8_t *p, uint8_t len)发送数据;

**********************************************************************************/

#include "soft_uart.h"

SoftUart_Typedef  SoftUart0 = {//使能Rx功能
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
	,.SoftUartWordLength     = 8+1	//(8数据位+1停止位)
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
	
	SoftUart->TxByteData = Data;		//要发送的数据写入发送缓冲
	SoftUart->TxBusy = 1;		//启动发送
	
	uint32_t u32Clk = TIMER_GetModuleClock(TIMER1);
	TIMER_SET_CMP_VALUE(TIMER2, u32Clk/Baud);
	TIMER_Start(TIMER2);
	while(SoftUart->TxBusy);	//等待发送完成
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

	GPIO_WriteBit(SoftUart->SoftUartTxPort, SoftUart->SoftUartTxPin, Bit_SET);//输出高电平
	
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
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE); //时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period    = 208; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = 2; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE); //使能指定的TIM3中断,允许更新中断
	
	//中断优先级NVIC设置
	/* Enable and set EXTI5 Interrupt */
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

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
		TIMER_Start(TIMER1);//Timer1 开始运行
		SoftUart->RxStart = 1;		//标志已收到起始位
		SoftUart->RxBitCnt = 9;		//初始化接收的数据位数(8个数据位+1个停止位)
		
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
	
	//====================== 模拟串口接收程序 ========================================
	if(SoftUart->RxStart)		//已收到起始位
	{
		if ((--SoftUart->RxBitCnt) == 0)		//接收完一帧数据
		{
			SoftUart->RxStart = 0;			//停止接收
			SoftUart->RxByteData = SoftUart->RxShiftReg;		//存储数据到缓冲区
//			SoftUart->RxTimeOut = 1;
			SoftUart->RxDone = 1;	//标志已收到一帧数据
			TIMER_Stop(TIMER1);	//Timer1 停止运行
			GPIO_EnableInt(SoftUart->SoftUartRxPort, SoftUart->SoftUartRxPinNum, GPIO_INT_FALLING);	//允许INT中断
		}
		else
		{
			SoftUart->RxShiftReg >>= 1;			  		//把接收的单b数据 暂存到 RxShiftReg(接收缓冲)
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
		ModbusReceive(&SoftUart0, &Modbus0);//将SoftUart0收到的数据存放到Modbus0中
		
//		SoftUartRxCallBack(&SoftUart1);		
//		ModbusReceive(&SoftUart1, &Modbus3);
		
//		SoftUartRxCallBack(&SoftUart2);		
//		ModbusReceive(&SoftUart2, &Modbus4);
	}
}


void SoftUartTxCallBack(void *SoftUartx)
{
	SoftUart_Typedef *SoftUart = (SoftUart_Typedef *)SoftUartx;
//	//====================== 模拟串口发送程序 ========================================
	if(SoftUart->TxBusy)					// 不发送, 退出
	{
		if(SoftUart->TxBitCnt == 0)			//发送计数器为0 表明单字节发送还没开始
		{
			SoftUart->SoftUartTxPort->DOUT &= ~SoftUart->SoftUartTxPin;//发送起始位  send start bit
			SoftUart->TxShiftReg = SoftUart->TxByteData;		//把缓冲的数据放到发送的buff
			SoftUart->TxBitCnt = SoftUart->SoftUartWordLength;			//发送数据位数 (8数据位+1停止位)
		}
		else						//发送计数器为非0 正在发送数据
		{
			if (--SoftUart->TxBitCnt == 0)	//发送计数器减为0 表明单字节发送结束
			{
				SoftUart->SoftUartTxPort->DOUT |= SoftUart->SoftUartTxPin;//送停止位数据
				SoftUart->TxBusy = 0;		//发送停止
				TIMER_Stop(TIMER2);	//Timer2 停止运行
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
* 数据上RS485之前有npn9013，则需先将所有数据取反(包括起始位和停止位)
*/
void ReverseDataTxCallBack(void *SoftUartx)
{
	SoftUart_Typedef *SoftUart = (SoftUart_Typedef *)SoftUartx;
//	//====================== 模拟串口发送程序 ========================================
	if(SoftUart->TxBusy)					// 不发送, 退出
	{
		if(SoftUart->TxBitCnt == 0)			//发送计数器为0 表明单字节发送还没开始
		{
			SoftUart->SoftUartTxPort->DOUT |= SoftUart->SoftUartTxPin;//发送起始位  send start bit
			SoftUart->TxShiftReg = ~(SoftUart->TxByteData);		//把缓冲的数据放到发送的buff
			SoftUart->TxBitCnt = SoftUart->SoftUartWordLength;			//发送数据位数 (8数据位+1停止位)
		}
		else						//发送计数器为非0 正在发送数据
		{
			if (--SoftUart->TxBitCnt == 0)	//发送计数器减为0 表明单字节发送结束
			{
				SoftUart->SoftUartTxPort->DOUT &= ~SoftUart->SoftUartTxPin;//送停止位数据
				SoftUart->TxBusy = 0;		//发送停止
				TIMER_Stop(TIMER2);	//Timer2 停止运行
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












