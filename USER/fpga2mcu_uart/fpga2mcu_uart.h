#ifndef __FPGA2MCU_UART_H__
#define __FPGA2MCU_UART_H__

#include "sys.h"














void fpga2mcu_uart_init(uint32_t bound);


void fpga2mcu_uart_process(void);

uint8_t get_uart_data(void);









#endif



