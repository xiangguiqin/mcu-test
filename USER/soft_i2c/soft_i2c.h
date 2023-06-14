#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H
#include "sys.h"

#define    GPIO_WRITEBIT                           (0U)

//IO方向设置
#define    SET_SDA_IN(soft_I2C_Portx)                             \
{                                                                 \
	uint32_t temp = soft_I2C_Portx->soft_I2C_SDA_Port->MODER;     \
	temp &= ~(3<<((soft_I2C_Portx->soft_I2C_SDA_PinSourcex)<<1)); \
	temp |= 0<<((soft_I2C_Portx->soft_I2C_SDA_PinSourcex)<<1);    \
	soft_I2C_Portx->soft_I2C_SDA_Port->MODER = temp;              \
}

#define    SET_SDA_OUT(soft_I2C_Portx)                            \
{                                                                 \
	uint32_t temp = soft_I2C_Portx->soft_I2C_SDA_Port->MODER;     \
	temp &= ~(3<<((soft_I2C_Portx->soft_I2C_SDA_PinSourcex)<<1)); \
	temp |= 1<<((soft_I2C_Portx->soft_I2C_SDA_PinSourcex)<<1);    \
	soft_I2C_Portx->soft_I2C_SDA_Port->MODER = temp;              \
}

#define    SOFT_I2C_SDA_READ(soft_I2C_Portx)       GPIO_ReadInputDataBit(soft_I2C_Portx->soft_I2C_SDA_Port, soft_I2C_Portx->soft_I2C_SDA_Pin)



typedef enum{
	NoErr         = 0x00,
	DeviceErr     = 0x01,
	AddrErr       = 0x02,
	DataErr       = 0x03
}I2C_Err_TypeDef;


typedef struct{
	uint32_t           RCC_AHBPeriph_SDA       ;
	uint32_t           RCC_AHBPeriph_SCL       ;
	
	GPIO_TypeDef      *soft_I2C_SDA_Port       ;
	GPIO_TypeDef      *soft_I2C_SCL_Port       ;
	uint16_t           soft_I2C_SDA_Pin        ;
	uint16_t           soft_I2C_SCL_Pin        ;
	uint8_t            soft_I2C_SDA_PinSourcex ;
	uint8_t            soft_I2C_SCL_PinSourcex ;
	uint8_t            soft_I2C_DeviceAddress  ;
	
	uint8_t            WaitAckEnable           ;
}soft_I2C_Port_Typedef;


#if  GPIO_WRITEBIT
typedef enum{
	Bit_RESET = 0,
	Bit_SET
}BitAction;
#endif

//IIC所有操作函数
#if  GPIO_WRITEBIT
void               GPIO_WriteBit               (GPIO_T *Port, uint32_t Pin, BitAction BitVal);
#endif             

I2C_Err_TypeDef    eSoft_I2C_Init              (soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t I2C_Address);//初始化IIC的IO口
void               vSoft_I2C_Start             (soft_I2C_Port_Typedef *soft_I2C_Portx);				//发送IIC开始信号
void               vSoft_I2C_Stop              (soft_I2C_Port_Typedef *soft_I2C_Portx);	  			//发送IIC停止信号
void               vSoft_I2C_Send_Byte         (soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t txd);	//IIC发送一个字节
uint8_t            uSoft_I2C_Read_Byte         (soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t ack);   //IIC读取一个字节
uint8_t            uSoft_I2C_Wait_Ack          (soft_I2C_Port_Typedef *soft_I2C_Portx); 				//IIC等待ACK信号
void               vSoft_I2C_Ack               (soft_I2C_Port_Typedef *soft_I2C_Portx);				//IIC发送ACK信号
void               vSoft_I2C_NAck              (soft_I2C_Port_Typedef *soft_I2C_Portx);				//IIC不发送ACK信号
		           
void               vSoft_I2C_Write_One_Byte    (uint8_t daddr, uint8_t addr, uint8_t data);
uint8_t            uSoft_I2C_Read_One_Byte     (uint8_t daddr, uint8_t addr);

/*****************************************************************************************************/
I2C_Err_TypeDef    eI2C_SetDeviceAddress       (soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t DeviceAddress);
I2C_Err_TypeDef    eI2C_DeviceWriteOneByte     (soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t WriteAddr, uint8_t DataToWrite);
I2C_Err_TypeDef    eI2C_DeviceReadOneByte      (soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t ReadAddr, uint8_t *pData);
I2C_Err_TypeDef    eI2C_DeviceWrite            (soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t WriteAddr, uint8_t *pBuffer, uint8_t NumToWrite);
I2C_Err_TypeDef    eI2C_DeviceRead             (soft_I2C_Port_Typedef *soft_I2C_Portx, uint8_t ReadAddr, uint8_t *pBuffer, uint8_t NumToRead);







#endif


