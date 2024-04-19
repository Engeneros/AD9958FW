#ifndef GPIO_H
#define GPIO_H
#include <stm32f10x.h>  

void SetPortToInput( GPIO_TypeDef* port, unsigned char pin );

void SetPortToOutput( GPIO_TypeDef* port, unsigned char pin );

void SetPortToHigh( GPIO_TypeDef* port, unsigned char pin );

void SetPortToLow( GPIO_TypeDef* port, unsigned char pin );

void SetPortToAlternateOut( GPIO_TypeDef* port, unsigned char pin );

class GPOut 
{
public:
	GPOut (GPIO_TypeDef* port, unsigned char pin);
	void ToLow();
	void ToHigh();
	void Toggle();
	virtual void operator = (int val);
	virtual	void operator = (bool val);
	virtual GPOut& operator ! ();
	
	unsigned char GatPinNum() {return pNum;}
	GPIO_TypeDef* GetPortGroup() {return pGroup;}
	virtual ~GPOut(){}
private:
	GPIO_TypeDef* pGroup;
	unsigned char pNum;
	uint32_t pinSRMask, pinTglMask;
};
#endif


