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
	
//	SetPortToOutput(GPIOC, 13);
	GPOut c13(GPIOC, 13);
	SetPortToOutput(GPIOB, 8);
	SetPortToOutput(GPIOB, 9);

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
//	AD9958(GPOut* cSel, GPOut* ioUpdate, GPOut* mReset, GPOut* syncIO);	
	AD9958 dds(&csel, &ioUpd, &mstRst, &syncIO);
	int cnt = 0;
	DelayMs(1000);
	dds.Set3WireIfc();
	DelayMs(100);
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
	
	tmp = 1000000.0 * (double)( 0xffffffff) / 500000000.0;
	ftW = (uint32_t) tmp;
	dds.WriteReg (4, ftW);
	DelayMs(100);
	
	while(1)
	{
	//	GPIOC->BSRR = 1 << 13;
		if ((++cntdd % 20) == 0)
		{
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
			
			dds.PrepareToJump();
			dataRd = dds.ReadReg(0, FR2);
			sprintf(outStr, "%d - reg2Bfr=%x%c%c", ++cnt, dataRd, 13, 10);
			UART1_SendString(outStr, strlen(outStr));
			DelayMs(10);
			
			dds.Jump();
			dataRd = dds.ReadReg(0, FR2);
			sprintf(outStr, "%d - reg2aftJmp=%x%c%c", ++cnt, dataRd, 13, 10);
			UART1_SendString(outStr, strlen(outStr));
			DelayMs(10);
			
			dds.DisableJamp();
			dataRd = dds.ReadReg(0, FR2);
			sprintf(outStr, "%d - reg2Bfr=%x%c%c", ++cnt, dataRd, 13, 10);
			UART1_SendString(outStr, strlen(outStr));
			DelayMs(10);
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
		c13 = 1;
//		pa0_ioUpd = 1;
//		pa2_syncIO = 1;
//		pa4_mstRst = 0;
////		pa3_cs =1;
//	DelayUs(5);
//	dataRd = dds.ReadReg(0, 1);
//	sprintf(outStr, "%d - reg0=%x%c%c", ++cnt, dataRd, 13, 10);
//	UART1_SendString(outStr, strlen(outStr));
		DelayMs(1000);
//	DelayMs(200);
		c13 = 0;
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