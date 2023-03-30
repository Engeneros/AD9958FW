#include <SPI.h>
#include <PrgDelay.h>
#include <UART1.h>

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
	
//	SetPortToOutput(port, pin);
	SetPortToAlternateOut(GPIOA, 7);
	SetPortToAlternateOut(GPIOA, 5);
		
//	NVIC_EnableIRQ(SPI1_IRQn);
//	SPI1->CR2 &= ~(1 << 7);

	SPI1->CR1 |= 1 << 6; //spi enable
}


void inline SPIifc::SetDataSize8()
{
	//WaitReady();
	SPI1->CR1 &= ~ENABLE;
	SPI1->CR1 &= ~DATA_SIZE_16;
	SPI1->CR1 |= ENABLE;
}

void inline SPIifc::SetDataSize16()
{
	//WaitReady();
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
//	WaitReady();	
	portCS->BRR = 1 << pinCS;
	SPI1->DR = data;
	WaitReady();
	portCS->BSRR = 1 << pinCS;
}


void SPIifc::TxRx(char* ioBuf, unsigned int bitNum)
{
	
}

////---------------2Ch 500 MSpS DDS generator--------------------
////							CSR  FR1   FR2   CFR	
static const uint8_t reg_szs[] = {1,  3,    2,    3, 
////							CFTW  CPOW  ACR  LSRR
                                 4,   2,    3,   2,   
////                            RDW	   FDW
								4,      4     };
AD9958::AD9958 (GPOut* cSel, GPOut* ioUpdate, GPOut* mReset, GPOut* syncIO) : SPIifc (cSel->GetPortGroup(), cSel->GatPinNum(), SPI_IFC_AD9958),
csPin(cSel), ioUpdt(ioUpdate), mRst(mReset), ioSync (syncIO)   
{
	DelayMs(5);
	*csPin = 1;
	*ioUpdt = 0;
	*syncIO = 0;
	*mRst = 1;
	DelayMs(5);
	*mRst = 0;
}
//-------------------------------------------------------------
uint32_t AD9958::ReadReg(uint32_t regAddr, uint8_t szBytes)
{
//	WaitReady();
	uint32_t ret = SPI1->DR;
//	for (int i = szBytes, i > 0; --i  )
//	{
//		Send
//	
//	}
	if (szBytes == 1)
	{
		SetDataSize16();
//		Select();
		SendData((regAddr  | R_BIT) << 8);
		ret = SPI1->DR;
	}
	return ret;
}

void AD9958::IOUpdate()
{
	*ioUpdt = 1;
	DelayUs(100);
	*ioUpdt = 0;
}
//-------------------------------------------------------------
void AD9958::Set3WireIfc()
{	
	SetDataSize16();
	static unsigned char chans = 0;
//	Select();
	uint16_t msg = CH_SEL_RG_ADDR;
	msg <<= 8;
	//msg |=0x32 | ((++chans & 3) << 6);
	msg |=0xF2;
	SendData(msg);
	IOUpdate();
}


void AD9958::WriteReg(uint32_t regAddr, uint32_t data, bool justNow)
{
	uint16_t msg = regAddr;
	uint8_t rgLn = reg_szs[regAddr];
	if(rgLn == 3)
	{
		uint32_t bMost = data >> 16;
		uint32_t bLeast = data & 0xFFFF;
		SetDataSize16();
		msg <<= 8;
		msg |= bMost;
		*csPin = 0;
		SPI1->DR = msg;
		WaitReady();
		SPI1->DR = bLeast;
		WaitReady();
		*csPin = 1;	
//		IOUpdate();
	}
//	else if(dataSz == 4)
//	{
//		SetDataSize8();
//		uint16_t msg = regAddr;
//		*csPin = 0;
//		for (int i = 5; i > 0; --i)
//		{
//			SPI1->DR = msg;
//			msg = data >> 24;
//			data <<= 8;
//			WaitReady();
//		}
//		*csPin = 1;
//		IOUpdate();
//	}
	else if((rgLn == 2) || (rgLn  == 4))
	{
		SetDataSize8();
		uint16_t msg = regAddr;
		*csPin = 0;
		for (int i = (rgLn + 1); i > 0; --i)
		{
			SPI1->DR = msg;
			msg = data >> (8 * (rgLn - 1)) & 0x000000ff;
			data <<= 8;
			WaitReady();
		}
		*csPin = 1;
//		IOUpdate();
	}
	if((justNow != false) && (rgLn > 1) && (rgLn < 5))
		IOUpdate();
}

void AD9958::PrepareToJump()
{
	valFR2 = ReadReg(FR2);
	WriteReg(FR2, valFR2 | 0x2000, false);
}

void AD9958::Jump()
{
	IOUpdate();
//	valFR2 &= 0xdfff;
//	WriteReg(FR2, valFR2);
}

void AD9958::DisableJamp()
{
	valFR2 &= 0xdfff;
	WriteReg(FR2, valFR2);
}

uint32_t AD9958::ReadReg(uint32_t regAddr)
{

	uint32_t ret = 0x1245, tmp = SPI1->DR;;
	uint16_t msg = regAddr | R_BIT;
	uint8_t rgLn = reg_szs[regAddr];
	if(rgLn == 3)
	{
		SetDataSize16();		
		msg <<= 8;
		*csPin = 0;
		SPI1->DR = msg;
		WaitReady();
		ret = SPI1->DR;
		SPI1->DR = 0;
		ret <<= 16;
		WaitReady();
		*csPin = 1;	
		tmp = SPI1->DR;
		ret |= tmp;
	}
//	else if(rgLn == 4)
//	{
//		SetDataSize8();
//		*csPin = 0;
//		for (int i = 5; i > 0; --i)
//		{
//			SPI1->DR = msg;
//			msg = 0;
//			ret <<= 8;
//			WaitReady();
//			tmp = SPI1->DR;
//			ret |= tmp & 0x000000FF;
//		}
//		*csPin = 1;
//	}
	else if((rgLn == 2) || (rgLn == 4))
	{
		SetDataSize8();
		*csPin = 0;
		for (int i = (rgLn + 1); i > 0; --i)
		{
			SPI1->DR = msg;
			msg = 0;
			ret <<= 8;
			WaitReady();
			tmp = SPI1->DR;
			ret |= tmp & 0x000000FF;
		}
		*csPin = 1;
	}
	return ret;
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
