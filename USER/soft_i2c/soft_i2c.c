#include "soft_i2c.h"
#include "delay.h"


//Nuvoton �⺯��û��д1bit IO�ĺ�������STM32�У�Ϊ�˼���STM32��ʹ����������
#if  GPIO_WRITEBIT
void GPIO_WriteBit(GPIO_T *Port, uint32_t Pin, BitAction BitVal)
{
	uint32_t pos = BitVal?Pin:~Pin;
	
	GPIO_ENABLE_DOUT_MASK(Port, ~Pin & 0x000000ff);
	Port->DOUT = pos & 0x000000ff;
	GPIO_DISABLE_DOUT_MASK(Port, ~Pin & 0x000000ff);
}
#endif

//��ʼ��I2C
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
//����I2C��ʼ�ź�
void vSoft_I2C_Start(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	SET_SDA_OUT(soft_I2C_Portx);//sda�����
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_SET);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(4);
 	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_RESET);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����I2Cֹͣ�ź�
void vSoft_I2C_Stop(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	SET_SDA_OUT(soft_I2C_Portx);//sda�����
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_RESET);//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(4);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_SET);//����I2C���߽����ź�
	delay_us(4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t uSoft_I2C_Wait_Ack(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	uint8_t ucErrTime=0;
	SET_SDA_IN(soft_I2C_Portx);      //SDA����Ϊ����  
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_SET);
	delay_us(1);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(1);
	while(SOFT_I2C_SDA_READ(soft_I2C_Portx))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			//��Ҫ����I2CΪʲôû��wait ack��Ӳ������ô��Ƶģ�û�п��������⣬�����ֻ�ܺ�������
			//���������ʹ��mcu��Դ�ϵ�I2C���߿ɲ����ԣ���ֻ�ܸ������Ҳ�֪�������Լ�ȥ���ԣ�
			//�ҷ���Ϊ��������ַǱ�׼����ֵֹ����⣬�Ѿ�����ʹ��Ӳ��I2C�ˣ�����SPI�����д��ڣ�ȫ�����ģ�⡣
			if(soft_I2C_Portx->WaitAckEnable)
				vSoft_I2C_Stop(soft_I2C_Portx);
			else
				delay_us(1);
			return 1;
		}
	}
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);//ʱ�����0
	return 0;  
} 
//����ACKӦ��
void vSoft_I2C_Ack(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
	SET_SDA_OUT(soft_I2C_Portx);//sda�����
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_RESET);
	delay_us(2);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(2);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
}
//������ACKӦ��		    
void vSoft_I2C_NAck(soft_I2C_Port_Typedef *soft_I2C_Portx)
{
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
	SET_SDA_OUT(soft_I2C_Portx);//sda�����
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, Bit_SET);
	delay_us(2);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
	delay_us(2);
	GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
}					 				     
//I2C����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void vSoft_I2C_Send_Byte(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t txd)
{                        
    uint8_t t;   
	SET_SDA_OUT(soft_I2C_Portx);//sda�����
//	soft_I2C_Portx->soft_I2C_SDA_Port->MODER = 1<<((soft_I2C_Portx->soft_I2C_SDA_PinSourcex)<<1);
    GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
		GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin, (BitAction)((txd&0x80)>>7));
		txd<<=1; 	  
		delay_us(2);   //��������ʱ���Ǳ����
		GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_SET);
		delay_us(2);
		GPIO_WriteBit(soft_I2C_Portx->soft_I2C_SCL_Port, soft_I2C_Portx->soft_I2C_SCL_Pin, Bit_RESET);
		delay_us(2);
    }	 
}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t uSoft_I2C_Read_Byte(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t ack)
{
	uint8_t i,receive=0;
	
	SET_SDA_IN(soft_I2C_Portx);//SDA����Ϊ����
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
        vSoft_I2C_NAck(soft_I2C_Portx);//����nACK
    else
        vSoft_I2C_Ack(soft_I2C_Portx); //����ACK
    return receive;
}

/*****************************************************************************************************/
/****************************** I2C�豸��д **********************************************************/
I2C_Err_TypeDef eI2C_SetDeviceAddress(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t DeviceAddress)
{
	soft_I2C_Portx->soft_I2C_DeviceAddress = DeviceAddress<<1;
	return NoErr;
}

I2C_Err_TypeDef eI2C_DeviceWriteOneByte(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t WriteAddr, uint8_t DataToWrite)
{
    vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress);	    //����д����
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DeviceErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, WriteAddr);//���͵�ַ
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return AddrErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, DataToWrite);     //�����ֽ�
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DataErr;
	}
    vSoft_I2C_Stop(soft_I2C_Portx);//����һ��ֹͣ����
	return NoErr;
}

I2C_Err_TypeDef eI2C_DeviceReadOneByte(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t ReadAddr, uint8_t *Data)
{
    vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress);	   //����д����
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DeviceErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, ReadAddr);//���ͼĴ�����ַ
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return AddrErr;
	}
	vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress+1);           //�������ģʽ
    if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DataErr;
	}
	*Data = uSoft_I2C_Read_Byte(soft_I2C_Portx, 0);
    vSoft_I2C_Stop(soft_I2C_Portx);//����һ��ֹͣ����
	return NoErr;
}

I2C_Err_TypeDef eI2C_DeviceWrite(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t WriteAddr, uint8_t *pBuffer, uint8_t NumToWrite)
{
	vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress);	    //����д����
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DeviceErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, WriteAddr);//���͵�ַ
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return AddrErr;
	}
	while(NumToWrite--){
		vSoft_I2C_Send_Byte(soft_I2C_Portx, *pBuffer);     //�����ֽ�
		if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
			if(soft_I2C_Portx->WaitAckEnable)
				return DataErr;
		}
		pBuffer++;
	}
	vSoft_I2C_Stop(soft_I2C_Portx);//����һ��ֹͣ����
	return NoErr;
}

I2C_Err_TypeDef eI2C_DeviceRead(soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t ReadAddr, uint8_t *pBuffer, uint8_t NumToRead)
{
	vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress);	   //����д����
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DeviceErr;
	}
	vSoft_I2C_Send_Byte(soft_I2C_Portx, ReadAddr);//���ͼĴ�����ַ
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return AddrErr;
	}
	vSoft_I2C_Start(soft_I2C_Portx);
	vSoft_I2C_Send_Byte(soft_I2C_Portx, soft_I2C_Portx->soft_I2C_DeviceAddress+1);           //�������ģʽ
	if(uSoft_I2C_Wait_Ack(soft_I2C_Portx) != 0){
		if(soft_I2C_Portx->WaitAckEnable)
			return DataErr;
	}
	while(NumToRead--){
		*pBuffer++= uSoft_I2C_Read_Byte(soft_I2C_Portx, NumToRead);//NumToRead!=0ʱ����NACK, else send ACK
	}
	vSoft_I2C_Stop(soft_I2C_Portx);//����һ��ֹͣ����
	return NoErr;
}




