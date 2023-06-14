#ifndef __SOFT_UART_H__#define __SOFT_UART_H__#include "sys.h"#define	     Baud	                         (115200UL)		    //���岨����#define      TCMPR_BAUD_VALUE                (192U)		//22118400/115200 = 192#define      SOFT_UART_READ(SoftUartx)       ((SoftUartx->SoftUartRxPort->PIN) & (SoftUartx->SoftUartRxPin))typedef struct{	uint32_t           RCC_AHBPeriph_Tx     ;	uint32_t           RCC_AHBPeriph_Rx     ;		GPIO_TypeDef      *SoftUartTxPort       ;	GPIO_TypeDef      *SoftUartRxPort       ;	uint32_t           SoftUartTxPin        ;	uint32_t           SoftUartRxPin        ;	uint8_t            RxEXTI_PortSourcex   ;	uint8_t            RxEXTI_PinSourcex    ;	uint32_t           EXTI_Linex           ;	IRQn_Type          EXTIx_IRQn           ;		uint32_t           SoftUartBaud         ;	uint16_t           SoftUartWordLength   ;	uint8_t            TxBitCnt             ;	uint8_t            RxBitCnt             ;	uint8_t            RxStart              ;	uint8_t            TxByteData           ;	uint8_t            RxByteData           ;	uint8_t            TxShiftReg           ;	uint8_t            RxShiftReg           ;		uint8_t            TxBusy               ;	uint8_t	           RxDone               ;	uint8_t            RxEnable             ;}SoftUart_Typedef;extern SoftUart_Typedef  SoftUart0;extern SoftUart_Typedef  SoftUart1;extern SoftUart_Typedef  SoftUart2;void SoftUartSendByte(void *SoftUartx, uint8_t Data);void SoftUartSendData(SoftUart_Typedef *SoftUartx, uint8_t *p, uint8_t len);void SoftUartInit(void *SoftUartx);void SoftUartTimerInit(void);void SoftUartRxStartCallBack(SoftUart_Typedef *SoftUartx);void SoftUartRxCallBack(SoftUart_Typedef *SoftUartx);void SoftUartTxCallBack(void *SoftUartx);void ReverseDataTxCallBack(void *SoftUartx);#endif