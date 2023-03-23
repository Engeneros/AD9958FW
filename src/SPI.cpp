#include <SPI.h>
#include <PrgDelay.h>

//void SPI1_IRQHandler (void)
//{
//	GPIOB->BRR =  1 << 9; 
//	SPI1->CR2 &= ~ (1 << 7);
//	while((SPI1->SR & (1 << 7)) != 0)
//	{
//	}
//	if(txSize > 0)
//	{
//		SPI1->DR = out.buff[--txSize];
//		SPI1->CR2 |= 1 << 7;
//	}
//	else
//		GPIOA->BSRR = 1 << 3;
//	GPIOB->BSRR =  1 << 9;
//}

SPIifc::SPIifc(GPIO_TypeDef* port, uint8_t pin) : portCS(port), pinCS(pin)
{
	RCC->APB2ENR |= 1 << 12;
	// -----CR1-----
	//bits: 0,1 CPHA = 1, CPOL = 0 - falling edge activ, idle clk state  = 0
	//bits: 3-5 = 1,1,1 - Fspi = Fpclk/265, 3 = Fpclk/16
	//bit 6  = 0 - spi disable
	//bit 11 = 0 - 8 bit data lenght 
	SPI1->CR1 = 0;
	SPI1->CR1 |= ((1 << 2) | 1 | (3 << 3) | ( 1 << 11)); //  second front PCCLK / 16,  16 bit
	
	SetPortToOutput(port, pin);
	SetPortToAlternateOut(GPIOA, 7);
	SetPortToAlternateOut(GPIOA, 5);
		
//	NVIC_EnableIRQ(SPI1_IRQn);
//	SPI1->CR2 &= ~(1 << 7);

	SPI1->CR1 |= 1 << 6; //spi enable
}

SPIifc::SPIifc(GPIO_TypeDef* port, uint8_t pin, SPI_IFC_TYPE ifcType) : portCS(port), pinCS(pin)
{
	RCC->APB2ENR |= 1 << 12;
	// -----CR1-----
	//bits: 0,1 CPHA = 1, CPOL = 0 - falling edge activ, idle clk state  = 0
	//bits: 3-5 = 1,1,1 - Fspi = Fpclk/265, 3 = Fpclk/16
	//bit 6  = 0 - spi disable
	//bit 11 = 0 - 8 bit data lenght 
	SPI1->CR1 = 0;
	if (ifcType == SPI_IFC_AD9958)
		SPI1->CR1 = ((1 << 2) | (3 << 3) | ( 1 << 11)); //  second front PCCLK / 16,  8 bit
	
	SetPortToOutput(port, pin);
	SetPortToAlternateOut(GPIOA, 7);
	SetPortToAlternateOut(GPIOA, 5);
		
//	NVIC_EnableIRQ(SPI1_IRQn);
//	SPI1->CR2 &= ~(1 << 7);

	SPI1->CR1 |= 1 << 6; //spi enable
}


void inline SPIifc::SetDataSize8()
{
	WaitReady();
	SPI1->CR1 &= ~ENABLE;
	SPI1->CR1 &= ~DATA_SIZE_16;
	SPI1->CR1 |= ENABLE;
}

void inline SPIifc::SetDataSize16()
{
	WaitReady();
	SPI1->CR1 &= ~ENABLE;
	SPI1->CR1 |= DATA_SIZE_16;
	SPI1->CR1 |= ENABLE;
}

void  inline SPIifc::WaitReady()
{
	uint16_t status;
	do 
	{
		status = SPI1->SR;
//		portCS->BSRR = 1 << pinCS;
//		portCS->BRR = 1 << pinCS;
	}
	while((status & BUSY) != 0);
}

void  inline SPIifc::WaitRxReady()
{
	uint16_t status;
	do 
	{
		status = SPI1->SR;
//		portCS->BSRR = 1 << pinCS;
//		DelayMs(2);
//		portCS->BRR = 1 << pinCS;
//		DelayMs(1);
	}
	while((status & RX_NOT_EMPTY) == 0);
}

//void SPIifc::SendData(uint8_t data)
//{
////	WaitReady();
////	portCS->BSRR = 1 << pinCS;
//	portCS->BRR = 1 << pinCS;
//	SPI1->DR = data;
////	WaitReady();
//	portCS->BSRR = 1 << pinCS;
//}

void SPIifc::SendData(uint32_t data)
{	
	WaitReady();	
	portCS->BRR = 1 << pinCS;
	SPI1->DR = data;
	WaitReady();
	portCS->BSRR = 1 << pinCS;
}


void SPIifc::TxRx(char* ioBuf, unsigned int bitNum)
{
	
}

//---------------2Ch 500 MSpS DDS generator--------------------
AD9958::AD9958(GPIO_TypeDef* cs_port, uint8_t cs_pin) : SPIifc (cs_port, cs_pin, SPI_IFC_AD9958)
{
	DelayMs(5);
}
//-------------------------------------------------------------
uint32_t AD9958::ReadReg(uint32_t regAddr, uint8_t szBytes)
{
	WaitReady();
	uint32_t ret = SPI1->DR;
	if (szBytes == 2)
	{
		SetDataSize16();
//		Select();
		SendData((regAddr  | R_BIT) << 8);
		WaitRxReady();
		ret = SPI1->DR;
	}
	return ret;
}
//-------------------------------------------------------------
void AD9958::Set3WireIfc()
{
	
	SetDataSize16();
//	Select();
	uint16_t msg = CH_SEL_RG_ADDR;
	msg <<= 8;
	msg |=0xC2;
	SendData(msg);
}
//-------------------------------------------------------------


AD5761::AD5761 (GPIO_TypeDef* port, uint8_t pin) : SPIifc (port, pin)
{
	DelayMs(5);
	SendData(RESET_RG_ADDR);
	DelayMs(5);
	SendData(CONTROL_RG_ADDR | 0x23);	//0..5V, +5% ovf, Reference on, stright binary code.
	DelayMs(5);	
}

void AD5761::SendData(uint32_t data)
{
	SetDataSize8();
//	uint16_t cr1 = SPI1->CR1;
//	uint16_t cr1D = cr1 & ~ENABLE;
//	cr1 &= ~DATA_SIZE_16;
//	uint16_t cr1E = cr1 | ENABLE;
//	WaitReady();
//	SPI1->CR1 = cr1D;
//	SPI1->CR1 = cr1;
//	SPI1->CR1 = cr1E;	
	Select();	
	SPI1->DR = (uint16_t) (data >> 16);
	SetDataSize16();
//	cr1 = SPI1->CR1;
//	cr1D = cr1 & ~ENABLE;
//	cr1 |= DATA_SIZE_16;
//	cr1E = cr1 | ENABLE;
//	WaitReady();
	
//	uint16_t status;
//	do 
//		status = SPI1->SR;
//	while((status & BUSY) != 0);
	
//	
//	SPI1->CR1 = cr1D;
//	SPI1->CR1 = cr1;
//	SPI1->CR1 = cr1E;
	
	SPI1->DR = (uint16_t) (data & 0xffFF);
	WaitReady();
	Release();	
} 

uint32_t AD5761::ReadRegData(uint32_t regAddr)
{
	WaitReady();
	uint16_t ret = SPI1->DR;
	SendData(regAddr << 16);
	WaitRxReady();
	ret = SPI1->DR;
	SendData(0);
	ret = SPI1->DR;
	return ret;
}


void AD5761::WriteDacData(uint32_t data)
{
	qwantsNow = data & 0xffff;
	SendData(DATA_RG_ADDR | qwantsNow);
}

void AD5761::SetVoltage(uint32_t mVolts)
{ 
	WriteDacData ( (mVolts << 16) / 5000 );	
}

void AD5761::SetVoltage(double mVolts)
{ 
	mVolts *= 65535.0;
	uint32_t dacData = (uint32_t) mVolts;
	dacData /= 5000;
//	uint32_t dacData = (uint32_t) ( ((uint32_t)  (mVolts * 65535.0))/5000);
	WriteDacData ( dacData );	
}

//template <typename Type>
//AD5761& AD5761::operator= (Type mVolts)
//{
//	SetVoltage(mVolts);
//	return *this;
//}


AD5761& AD5761::operator= (uint32_t mVolts)
{
	SetVoltage(mVolts);
	return *this;
}

AD5761& AD5761::operator= (double mVolts)
{
	SetVoltage(mVolts);
	return *this;
}



AD5761& AD5761::operator+= (const int32_t mVolts)
{
	int32_t qDelta = (mVolts * 0x10000 / 5000);// + qwantsNow;
	int32_t qNow = (int32_t) qwantsNow;
	if (qDelta >= 0 )
		qNow += qDelta;
	else
	{
		qDelta = -qDelta;
		if(qDelta < qNow)
			qNow -= qDelta;
		else
			qNow = 0;			
	}
		
	WriteDacData( (uint32_t) qNow);
	return *this;
}

//AD5761& AD5761::operator += (AD5761& left, double mVolts)
//{
//	mVolts *= 65535.0;
//	uint32_t dacData = (uint32_t) mVolts;
//	uint32_t val = (dacData << 16) / 5000 + qwantsNow;
//	WriteDacData(val);
//	return *this;
//}

//AD5761& AD5761::operator -= (AD5761& left, uint32_t mVolts)
//{
//	uint32_t val =  (mVolts << 16) / 5000;
//	(qwantsNow > val)? 	WriteDacData(qwantsNow - val) : WriteDacData(0);
//	return *this;
//}

//AD5761& AD5761::operator -= (AD5761& left, double mVolts)
//{
//	mVolts *= 65535.0;
//	uint32_t dacData = (uint32_t) mVolts;
//	uint32_t val = (dacData << 16) / 5000;
//	(qwantsNow > val)? WriteDacData(qwantsNow - val) : WriteDacData(0);
//	return *this;
//}

//	Asynchronous operation: I am returning from the function while the data is being transmitted over the bus.
template <class Type>
AD5761& AD5761::operator-= (Type dx)
{
	double delta  = (65535.0 * dx) / 5000.0;
	double valPrev = (double) qwantsNow;
	valPrev -= delta;
	uint32_t val = (valPrev < 1.0)? 0 : (valPrev >  65534.0)? 65535 : (uint32_t) valPrev;
	WriteDacData( (uint32_t) val);	
	return *this;
}
