#include <stdint.h>
#include "main.h"

#include "wiegand.h"


enum gState {IDLE, TRANSMITT} globalState;

enum bState {START_TX_BIT, END_TX_BIT} bitState;

TIM_HandleTypeDef *wiegandTim = NULL;//таймер, используемый для формирования времен

static uint32_t dataTx; //данные для передачи

static uint32_t bitsTx; //сколько бит передавать

volatile static int32_t curBit = 0;//номер текущего бита


void WiegandInit(TIM_HandleTypeDef *htim)
{
	wiegandTim = htim;
}

void WiegandTransmit(uint32_t data, uint32_t bits)
{
	globalState = TRANSMITT;
	bitState = START_TX_BIT;
	dataTx = data;
	bitsTx = bits;
	curBit = bits-1;
	//Запускаем таймер
	wiegandTim->Instance->ARR = WIEGAND_BIT_TIME;
	HAL_TIM_Base_Start_IT(wiegandTim);
}

uint32_t GetBit()
{
	if ((dataTx & (1 << (curBit--)))!=0) return 1;
	else return 0;
}

void WiegandProcess()
{
	switch (globalState)
	{
		case IDLE: break;
		case TRANSMITT:
		{
			switch (bitState)
			{
				case START_TX_BIT:
				{
					//если был передан последний бит завершаем передачу
					if (curBit < 0)
					{
						globalState = IDLE;
						HAL_TIM_Base_Stop_IT(wiegandTim);
						break;
					}

					if (GetBit()!=0)
					{
						HAL_GPIO_WritePin(WIEGAND_D1_GPIO_Port, WIEGAND_D1_Pin, GPIO_PIN_RESET);
					}
					else
					{
						HAL_GPIO_WritePin(WIEGAND_D0_GPIO_Port, WIEGAND_D0_Pin, GPIO_PIN_RESET);
					}

					bitState = END_TX_BIT;

					//Запускаем таймер на время передачи бита
					wiegandTim->Instance->ARR = WIEGAND_BIT_TIME - 1;

					break;
				}

				case END_TX_BIT:
				{
					//Без разбора устанавливаем обе линии в 1
					HAL_GPIO_WritePin(WIEGAND_D0_GPIO_Port, WIEGAND_D0_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(WIEGAND_D1_GPIO_Port, WIEGAND_D1_Pin, GPIO_PIN_SET);

					bitState = START_TX_BIT;

					//Запускаем таймер на период минус время передачи бита
					wiegandTim->Instance->ARR = WIEGAND_PERIOD - WIEGAND_BIT_TIME - 1;
					break;
				}

				default:break;
			}
			break;
		}
		default: break;
	}

}



