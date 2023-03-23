#include <stm32f10x.h> 
#include <PrgDelay.h>
#include <GPIO.h>

int main(void)
{
	RCC->APB2ENR |= 0x1D;// GPIO A & B & C  & AFIO  clock enable
	RCC->APB2ENR |= 1 << 14; //UART1 clock enable
	RCC->APB1ENR |= 1 | 4; //TIM2 & TIM4  clock enable
	Timer2ini();
//	Timer4ini();
//	UART1_ini();
	//Отключаем JTAG, SWD подключён   
	AFIO->MAPR &= (unsigned int) ~(0x00000007 << 24);//
	AFIO->MAPR |= (unsigned int) (0x00000002 << 24);
	
	SetPortToOutput(GPIOC, 13);
	

	SetPortToOutput(GPIOB, 8);
	SetPortToOutput(GPIOB, 9);

	SetPortToInput(GPIOB, 3);
	
	while(1)
	{
		GPIO
		
	}
	return 0;
}


//		#include <stm32f10x.h>   
//		//#include <UART1.h>
//		//#include <time.h>
//		#include <PrgDelay.h>
//		#include <GPIO.h>
//		#include <stdio.h>
//		#include <string.h>>

//		//const char ENTER[] = {13, 10, 0};

//		//void SPI_Set_8_dataSize()
//		//{

//		//}
//		//void SPI_Set_16_dataSize()
//		//{

//		//}

//		//void SPI_SendByte(uint8_t data)
//		//{

//		//}

//		//void SPI_SendWord(uint16_t data)
//		//{

//		//}

//		//void SPI_Send(uint32_t data)
//		//{
//		//	
//		//}

//		//SPI_ini()
//		//{


//		//}


//		int main (void)
//		{	
//			RCC->APB2ENR |= 0x1D;// GPIO A & B & C  & AFIO  clock enable
//			RCC->APB2ENR |= 1 << 14; //UART1 clock enable
//			RCC->APB1ENR |= 1 | 4; //TIM2 & TIM4  clock enable
//		//	Timer2ini();
//		//	Timer4ini();
//		//	UART1_ini();
//			//Отключаем JTAG, SWD подключён   
//			AFIO->MAPR &= (unsigned int) ~(0x00000007 << 24);//
//			AFIO->MAPR |= (unsigned int) (0x00000002 << 24);
//			
//			SetPortToOutput(GPIOC, 13);
//			SetPortToOutput(GPIOB, 8);
//			SetPortToOutput(GPIOB, 9);
//			SetPortToInput(GPIOB, 3);
//			
//			while(1)
//			{
//				GPIOB->BSRR = 1 << 8;
//				wait();
//				GPIOB->BRR = 1 << 8;
//				wait();
//			}
//		}

