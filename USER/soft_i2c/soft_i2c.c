#include "soft_i2c.h"
#include "delay.h"


//Nuvoton 库函数没有写1bit IO的函数，但STM32有，为了兼容STM32，使用条件编译
#if  GPIO_WRITEBIT
void GPIO_WriteBit(GPIO_T *Port, uint32_t Pin, BitAction BitVal)
{
	uint32_t pos = BitVal?Pin:~Pin;
	
	GPIO_ENABLE_DOUT_MASK(Port, ~Pin & 0x000000ff);
	Port->DOUT = pos & 0x000000ff;
	GPIO_DISABLE_DOUT_MASK(Port, ~Pin & 0x000000ff);
}
#endif

//初始化I2C
I2C_Err_TypeDef eSoft_I2C_Init(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t I2C_Address)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	eI2C_SetDeviceAddress(soft_I2C_Portx, I2C_Address);
	
	RCC_AHBPeriphClockCmd(soft_I2C_Portx->RCC_AHBPeriph_SCL, ENABLE);
	RCC_AHBPeriphClockCmd(soft_I2C_Portx->RCC_AHBPeriph_SDA, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = soft_I2C_Portx->soft_I2C_SCL_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(soft_I2C_Portx->soft_I2C_SCL_Port, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = soft_I2C_Portx->soft_I2C_SDA_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(soft_I2C_Portx->soft_I2C_SDA_Port, &GPIO_InitStructure);
	
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_SET);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);	
	return NoErr;
}
//产生I2C起始信号
void vSoft_I2C_Start(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	SET_SDA_OUT(soft_I2C_Portx);//sda线输出
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_SET);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(4);
 	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_RESET);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);//钳住I2C总线，准备发送或接收数据 
}	  
//产生I2C停止信号
void vSoft_I2C_Stop(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	SET_SDA_OUT(soft_I2C_Portx);//sda线输出
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_RESET);//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(4);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_SET);//发送I2C总线结束信号
	delay_us(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t uSoft_I2C_Wait_Ack(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	uint8_t ucErrTime=0;
	SET_SDA_IN(soft_I2C_Portx);      //SDA设置为输入  
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_SET);
	delay_us(1);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(1);
	while(SOFT_I2C_SDA_READ(soft_I2C_Portx))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			//不要问我I2C为什么没有wait ack，硬件就这么设计的，没有考虑这玩意，软件上只能忽视它，
			//如果你问我使用mcu资源上的I2C总线可不可以，我只能告诉你我不知道，你自己去尝试，
			//我反正为了适配各种非标准奇奇怪怪的问题，已经放弃使用硬件I2C了，还有SPI，还有串口，全部软件模拟。
			if(soft_I2C_Portx->WaitAckEnable)
				vSoft_I2C_Stop(soft_I2C_Portx);
			else
				delay_us(1);
			return 1;
		}
	}
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);//时钟输出0
	return 0;  
} 
//产生ACK应答
void vSoft_I2C_Ack(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
	SET_SDA_OUT(soft_I2C_Portx);//sda线输出
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_RESET);
	delay_us(2);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(2);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
}
//不产生ACK应答		    
void vSoft_I2C_NAck(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
	SET_SDA_OUT(soft_I2C_Portx);//sda线输出
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_SET);
	delay_us(2);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(2);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
}					 				     
//I2C发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void vSoft_I2C_Send_Byte(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t txd)
{                        
    uint8_t t;   
	SET_SDA_OUT(soft_I2C_Portx);//sda线输出
//	soft_I2C_Portx->soft_I2C_SDA_Port->MODER = 1<<((soft_I2C_Portx->soft_I2C_SDA_PinSourcex)<<1);
    GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
		GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, (BitAction)((txd&0x80)>>7));
		txd<<=1; 	  
		delay_us(2);   //这三个延时都是必须的
		GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
		delay_us(2);
		GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
		delay_us(2);
    }	 
}

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t uSoft_I2C_Read_Byte(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t ack)
{
	uint8_t i,receive=0;
	
	SET_SDA_IN(soft_I2C_Portx);//SDA设置为输入
    for(i=0;i<8;i++)
	{
        GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
        delay_us(2);
		GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
        receive<<=1;
        if(SOFT_I2C_SDA_READ(soft_I2C_Portx)){
			receive++;
		}
		delay_us(2);
    }
    if(!ack)
        vSoft_I2C_NAck(soft_I2C_Portx);//发送nACK
    else
        vSoft_I2C_Ack(soft_I2C_Portx); //发送ACK
    return receive;
}

/*****************************************************************************************************/
/****************************** I2C设备读写 **********************************************************/
I2C_Err_TypeDef eI2C_SetDeviceAddress(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t DeviceAddress)
{
	soft_I2C_Portx->soft_I2C_DeviceAddress = DeviceAddress<<1;
	return NoErr;
}

I2C_Err_TypeDef eI2C_DeviceWriteOneByte(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t WriteAddr, uint8_t DataToWrite)
{
    vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress);	    //发送写命令
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DeviceErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, WriteAddr);//发送地址
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return AddrErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, DataToWrite);     //发送字节
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DataErr;
	}
    vSoft_I2C_Stop(soft_I2C_Portx);//产生一个停止条件
	return NoErr;
}

I2C_Err_TypeDef eI2C_DeviceReadOneByte(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t ReadAddr, uint8_t *Data)
{
    vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress);	   //发送写命令
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DeviceErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, ReadAddr);//发送寄存器地址
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return AddrErr;
	}
	vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress+1);           //进入接收模式
    if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DataErr;
	}
	*Data = uSoft_I2C_Read_Byte(soft_I2C_Portx, 0);
    vSoft_I2C_Stop(soft_I2C_Portx);//产生一个停止条件
	return NoErr;
}

I2C_Err_TypeDef eI2C_DeviceWrite(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t WriteAddr, uint8_t *pBuffer, uint8_t NumToWrite)
{
	vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress);	    //发送写命令
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DeviceErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, WriteAddr);//发送地址
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return AddrErr;
	}
	while(NumToWrite--){
		vSoft_I2C_Send_Byte(soft_I2C_Portx, *pBuffer);     //发送字节
		if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
			if(soft_I2C_Portx->WaitAckEnable)
				return DataErr;
		}
		pBuffer++;
	}
	vSoft_I2C_Stop(soft_I2C_Portx);//产生一个停止条件
	return NoErr;
}

I2C_Err_TypeDef eI2C_DeviceRead(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t ReadAddr, uint8_t *pBuffer, uint8_t NumToRead)
{
	vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress);	   //发送写命令
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DeviceErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, ReadAddr);//发送寄存器地址
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return AddrErr;
	}
	vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress+1);           //进入接收模式
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DataErr;
	}
	while(NumToRead--){
		*pBuffer++= uSoft_I2C_Read_Byte(soft_I2C_Portx, NumToRead);//NumToRead!=0时发送NACK, else send ACK
	}
	vSoft_I2C_Stop(soft_I2C_Portx);//产生一个停止条件
	return NoErr;
}




