#include <stm32f10x.h> 
#include <PrgDelay.h>
#include <GPIO.h>
#include <SPI.h>
#include <UART1.h>
#include <stdio.h>
#include <string.h>


int adddrrrr = 0;
int main(void)
{
	RCC->APB2ENR |= 0x1D;// GPIO A & B & C  & AFIO  clock enable

	RCC->APB2ENR |= 1 << 14; //UART1 clock enable
//		RCC->APB1ENR |= 1 | 4; //TIM2 & TIM4  clock enable
//	Timer2ini();
//	Timer4ini();
	UART1_ini();
	//Отключаем JTAG, SWD подключён   
//	AFIO->MAPR &= (unsigned int) ~(0x00000007 << 24);//
//	AFIO->MAPR |= (unsigned int) (0x00000002 << 24);
	
	SetPortToOutput(GPIOC, 13);
	SetPortToOutput(GPIOB, 8);
	SetPortToOutput(GPIOB, 9);

//	SetPortToInput(GPIOB, 3);
	uint16_t spiData = 0x5555;
	double voltage = 0.0;
	uint32_t mVlt = 0, dataRd = 0;
//	SPIifc spi1(GPIOA, 3);
//	AD5761 ad5761(GPIOA, 3);
	AD9958 dds(GPIOA, 3);
	char outStr[64];
	while(1)
	{
//		spi1.SendData(++spiData);
		GPIOC->BSRR = 1 << 13;
		DelayMs(1000);
//		spi1.SendData(++spiData);
//		ad5761.SendData(dacData);
//		dacData += 0x00010001;
//		dacData ^= 0x00004100;
//		ad5761.SetVoltage(voltage % 5000);
//		for (int i = 0; i < 100; ++i)
		//mVlt = dataRd;
//		ad5761 +=  mVlt;
//		ad5761 <uint32_t>-= voltage;
//		ad5761 -= mVlt;
	//	ad5761.SendData(0x3AAAA);
		//dataRd = ad5761.ReadRegData(0xB);
		dds.Set3WireIfc();
		dataRd = dds.ReadReg(adddrrrr, 2);
		sprintf(outStr, "wr=%d ; rd=%d", mVlt, dataRd);
		UART1_SendString(outStr, strlen(outStr));
//		voltage = (voltage < 4500.0)? voltage + 150.0 : 0.0;
		mVlt += 100;
		mVlt %= 4500;
//		voltage = mVlt;
		GPIOC->BRR = 1 << 13;
		DelayMs(200);
		++adddrrrr;
		if(adddrrrr > 0x20)
			adddrrrr = 0;
		
	}
	return 0;
}