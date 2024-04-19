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
	
	GPOut c13(GPIOC, 13);
    GPOut b8(GPIOB, 8);

	GPOut b9(GPIOB, 9);

//	SetPortToInput(GPIOB, 3);
	uint16_t spiData = 0x5555;
	double voltage = 0.0;
	uint32_t mVlt = 0, dataRd = 0;
//	SPIifc spi1(GPIOA, 3);
//	AD5761 ad5761(GPIOA, 3);
/////////////	AD9958 dds(GPIOA, 3);
	char outStr[64];
//
	GPOut ioUpd (GPIOA, 0);
	GPOut mstRst(GPIOB, 5);
	GPOut syncIO(GPIOA, 2);
	GPOut csel(GPIOA, 3);

	AD9958 dds(&csel, &ioUpd, &mstRst, &syncIO);
	int cnt = 0;
	DelayMs(1000);
	
// We separate MISO and MOSI (3wire interface). SDIO_0 to MOSI; SDIO_2 -to MISO
	dds.Set3WireIfc();
	DelayMs(100);
//	
//Write to reg1. Datasheet p39 	
//bit23 - 1 = the high range (system clock above 255 MHz)
//bit 18-22 frequency multiplier 25MHzx20 = 500MHz 
	dds.WriteReg (1, (1 << 23) | (20 << 18) );
	
	DelayMs(100);
	dataRd = dds.ReadReg(1);
	DelayMs(100);
	sprintf(outStr, "%d - reg1=%x%c%c", ++cnt, dataRd, 13, 10);
	UART1_SendString(outStr, strlen(outStr));
	DelayUs(5);
	dataRd = dds.ReadReg(0, 1);
	sprintf(outStr, "%d - reg0=%x%c%c", ++cnt, dataRd, 13, 10);
	UART1_SendString(outStr, strlen(outStr));
	DelayMs(200);
	
//?? reg2 = 0x00C0
//perhaps in order to watch the synchronization with an oscilloscope
	dds.WriteReg (2, 3 << 6 );

	DelayMs(100);	
	dataRd = dds.ReadReg(2);
	DelayMs(100);
	sprintf(outStr, "%d - reg2=%x%c%c", ++cnt, dataRd, 13, 10);
	UART1_SendString(outStr, strlen(outStr));
	DelayUs(5);


	double freq = 100.0, tmp;
	

	uint32_t ftW;
	int cntdd  = 0;
//Calculation reg4 (Frequency Tuning Word - FTW) value for frequency = 1000000.0 Hz (1MHz)  Datasheet p18	
	tmp = 1000000.0 * (double)( 0xffffffff) / 500000000.0;
	ftW = (uint32_t) tmp;
//Set Freq = 1 MHz	
	dds.WriteReg (4, ftW);
	
	
	DelayMs(100);
	while(true)
	{
		b9 = 0;
		if ((++cnt & 0x40000) != 0)
		{
			cnt = 0;
			c13.Toggle();
		}
		b9 = 1;
//		if( (cnt & 0x20000) != 0)
//			c13 = 0;
	}
	
	while(1)
	{
	//	GPIOC->BSRR = 1 << 13;
		if ((++cntdd % 20) == 0)
		{
			c13 = 0;
			DelayMs(100);
			dataRd = dds.ReadReg(1);
			DelayMs(100);
			sprintf(outStr, "%d - reg1=%x%c%c", ++cnt, dataRd, 13, 10);
			UART1_SendString(outStr, strlen(outStr));
			DelayMs(5);
			dataRd = dds.ReadReg(0, 1);
			sprintf(outStr, "%d - reg0=%x%c%c", ++cnt, dataRd, 13, 10);
			UART1_SendString(outStr, strlen(outStr));
			DelayMs(10);
			
			b9 = 0;
			dds.PrepareToJump();
			dataRd = dds.ReadReg(FR2);
			sprintf(outStr, "%d - reg2Bfr=%x%c%c", ++cnt, dataRd, 13, 10);
			UART1_SendString(outStr, strlen(outStr));
			DelayMs(10);
			
			b8 = 0;
			dds.Jump();
			dataRd = dds.ReadReg(FR2);
			sprintf(outStr, "%d - reg2aftJmp=%x%c%c", ++cnt, dataRd, 13, 10);
			UART1_SendString(outStr, strlen(outStr));
			DelayMs(10);
			b9 = 1; 
			dds.DisableJamp();
			b8 = 1;
			dataRd = dds.ReadReg(FR2);
			sprintf(outStr, "%d - reg2Dsbl=%x%c%c", ++cnt, dataRd, 13, 10);
			UART1_SendString(outStr, strlen(outStr));
			DelayMs(10);
			c13 = 1;
		}			
//		tmp = freq * (double)( 0xffffffff)/ 500000000.0;
//		ftW = (uint32_t) tmp;
//		freq *= 1.002;
//		if ( freq > 20000000.0)
//			freq = 100.0;
//		dds.WriteReg (4, ftW);
//		DelayMs(100);
//		dataRd = dds.ReadReg(4);
//		DelayMs(100);
//		sprintf(outStr, "%d - reg4=%x%c%c", ++cnt, dataRd, 13, 10);
//		UART1_SendString(outStr, strlen(outStr));
		
//		sprintf(outStr, "wr=%d ; rd=%d", mVlt, dataRd);
//		UART1_SendString(outStr, strlen(outStr));
	//	GPIOC->BRR = 1 << 13;
		
//		pa0_ioUpd = 1;
//		pa2_syncIO = 1;
//		pa4_mstRst = 0;
////		pa3_cs =1;
//	DelayUs(5);
//	dataRd = dds.ReadReg(0, 1);
//	sprintf(outStr, "%d - reg0=%x%c%c", ++cnt, dataRd, 13, 10);
//	UART1_SendString(outStr, strlen(outStr));
		DelayMs(100);
//	DelayMs(200);
		
//		pa0_ioUpd = 0;
//		pa2_syncIO = 0;
//		DelayMs(100);
//		pa2_syncIO = 1;
//		DelayMs(100);
//		pa2_syncIO = 0;
//		pa4_mstRst = 1;
//		pa3_cs = 0;
		
	}
	return 0;
}