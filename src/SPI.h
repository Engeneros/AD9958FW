#ifndef SPI_H
#define SPI_H

#include <stm32f10x.h>
#include <GPIO.h>

enum SPI_IFC_TYPE
{
	SPI_IFC_AD9958 = 0
};

class SPIifc
{
public:
	SPIifc(GPIO_TypeDef* port, uint8_t pin);
	SPIifc(GPIO_TypeDef* port, uint8_t pin, SPI_IFC_TYPE ifcType);
	virtual void SendData(uint32_t data);
	virtual void TxRx(char* ioBuf, unsigned int bitNum);
//	void SendData(uint8_t data);
//	void SendData(uint32_t data);
	virtual ~SPIifc(){}
protected:
	static const uint16_t ENABLE = 1 << 6;
	static const uint16_t DATA_SIZE_16 = 1 << 11;
	static const uint16_t TX_BUFF_EMPTY = 2;
	static const uint16_t BUSY = 1 << 7;
	static const uint16_t RX_NOT_EMPTY = 1;
	void Select()   {portCS->BRR = 1 << pinCS;}
	void Release()  {portCS->BSRR = 1 << pinCS;}
	void inline WaitReady();
	void inline WaitRxReady();	
	void inline SetDataSize8();
	void inline SetDataSize16();
	void inline SetDataSize(uint8_t size) { (size == 16)? SetDataSize16() : SetDataSize8(); } 
private:
	GPIO_TypeDef* portCS;
	uint8_t pinCS;
};

//2Ch 500 MSpS DDS generator 
class AD9958 : public SPIifc
{
public:
	AD9958(GPIO_TypeDef* cs_port, uint8_t cs_pin);

	uint32_t ReadReg(uint32_t regAddr, uint8_t szBytes); 

	//Configure SDIO_2  (pin #52) as SDO (MISO)
	void Set3WireIfc();
	virtual ~AD9958() {}
private:
	static const uint8_t R_BIT = 1 << 7;
	static const uint8_t CH_SEL_RG_ADDR = 0;
};

class AD5761 : public SPIifc
{
public:
	AD5761 (GPIO_TypeDef* port, uint8_t pin);// : SPIifc (port, pin){}
	virtual void SendData(uint32_t data);
	virtual uint32_t ReadRegData(uint32_t regAddr);
	void SetVoltage(uint32_t mVolts);
	void SetVoltage(double mVolts);
	AD5761& operator = (double mVolts);
	AD5761& operator = (uint32_t mVolts);
// AD5761& operator += (AD5761& left, double mVolts);
	AD5761& operator += (const int32_t mVolts);
//	AD5761& operator -= (AD5761& left, double mVolts);
//	AD5761& operator -= (AD5761& left, uint32_t mVolts);
//	template<typename Type>
//	AD5761& operator = (<Type> mVolts);
	template <class Type>
	AD5761& operator -= (Type dx); 
	virtual ~AD5761(){}			
protected:
	uint32_t qwantsNow;
	static const uint32_t CONTROL_RG_ADDR = 0x00040000;
	static const uint32_t DATA_RG_ADDR = 0x00030000;
	static const uint32_t RESET_RG_ADDR = 0x000F0000;
	void WriteDacData(uint32_t data);
};


#endif