#ifndef __AW2013_H__
#define __AW2013_H__

#include "sys.h"
#include "soft_i2c.h"




typedef struct{
	uint8_t address; /*address is output register*/
	uint8_t value; /*The channel number is input */

}RegTypedef;



#define      AW2013_I2C_ADDR    	    0X45


#define      AW2013_SOFT_RESET_REG    	0X00
#define      AW2013_SOFT_RESET_VALUE    0X55
#define      AW2013_LCFG0_REG			0x31
#define      AW2013_LCFG1_REG			0x32
#define      AW2013_LCFG2_REG			0x33
#define      AW2013_PWM0_REG			0x34
#define      AW2013_PWM1_REG			0x35
#define      AW2013_PWM2_REG			0x36

extern soft_I2C_Port_Typedef soft_I2C_Port_aw2013;



///////////////////////////////////////////////////////////////////////////

void aw2013_init(void);
void aw2013_process(void);


#endif

