#ifndef WIEGANT_H
#define WIEGANT_H

#define WIEGAND_PERIOD 		2000
#define WIEGAND_BIT_TIME 	200

void WiegandInit(TIM_HandleTypeDef *htim);
void WiegandTransmit(uint32_t data, uint32_t bits);
void WiegandProcess(void);

#endif
