#include <stm32f10x.h> 
#include <UART1.h>
#include <GPIO.h>
const uint32_t TX_DATA_REG_EMPTY = 1 << 7;
const uint32_t TX_DMA_EN = 1<< 7;
const uint32_t RX_DMA_EN = 1 << 6;
void UART1_ini( void )
{
		USART1->BRR = 0x271;
		USART1->CR1 = (1 << 13) | (1 << 3) | (1 << 2);//Enable UART1, Tx1, RX1
		//USART1->CR3 = TX_DMA_EN | RX_DMA_EN;
		SetPortToAlternateOut(GPIOA, 9);
		SetPortToInput(GPIOA, 10);
}



void UART1_SendByte (char data)
{
	while((USART1->SR & TX_DATA_REG_EMPTY) == 0)
	{
	}
	USART1->DR = data;
}


void UART1_SendString(char* str, unsigned int len)
{
	for (int i = 0; i < len; ++i)
		UART1_SendByte(str[i]);
}