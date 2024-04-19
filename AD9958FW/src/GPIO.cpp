//#include <stm32f10x.h>
#include <GPIO.h>
#include <PrgDelay.h>

void EXTI3_IRQHandler(void)
{
	int i = 20;
//	EXTI->IMR &= ~8; // is not masked
	for (i = 20; i > 0; --i)
	{
			GPIOB->ODR ^= 1 << 5;
			wait();
	}  
//	EXTI->IMR |= 8; // is not masked
	EXTI->PR = 8;
}

void SetPortToInput( GPIO_TypeDef* port, unsigned char pin )
{
	if (pin < 8)
	{
		port->CRL &= (unsigned int) ~(0xF << (pin * 4));
		port->CRL |= (unsigned int) 4 << (pin *4);
	}
	else 
	{
		pin &= 7;
		port->CRH &= (unsigned int) ~(0xF << (pin * 4));
		port->CRH |= (unsigned int) 4 << (pin * 4); 
	}
}

void SetPortToOutput( GPIO_TypeDef* port, unsigned char pin )
{
	if (pin < 8)
	{
		port->CRL &= (unsigned int) ~(0xF << (pin * 4));
		port->CRL |= (unsigned int) 3 << (pin *4);
	}
	else 
	{
		pin &= 7;
		port->CRH &= (unsigned int) ~(0xF << (pin * 4));
		port->CRH |= (unsigned int) 3 << (pin * 4); 
	}
}

void SetPortToAlternateOut( GPIO_TypeDef* port, unsigned char pin )
{
	if (pin < 8)
	{
		port->CRL &= (unsigned int) ~(0xF << (pin * 4));
		port->CRL |= (unsigned int) 0xB << (pin *4);
	}
	else 
	{
		pin &= 7;
		port->CRH &= (unsigned int) ~(0xF << (pin * 4));
		port->CRH |= (unsigned int) 0xB << (pin * 4); 
	}
}

void SetPortToHigh( GPIO_TypeDef* port, unsigned char pin )
{
	port->BRR = 1 << pin;
}

void SetPortToLow( GPIO_TypeDef* port, unsigned char pin )
{
	port->BSRR = 1 << pin; 
}

GPOut::GPOut(GPIO_TypeDef* port, unsigned char pin) : pGroup(port), pNum(pin)
{
	SetPortToOutput(pGroup, pNum);
	pinTglMask = pinSRMask = 1 << pNum;
	pinTglMask |= pinSRMask << 16;
}
void GPOut::ToLow()
{
	pGroup->BRR = pinSRMask;
}

void GPOut::Toggle()
{
	uint32_t state = pGroup->ODR & pinSRMask;
//	state ^= pinSRMask;
	(state == 0)? ToHigh() : ToLow(); 
	
//	pGroup->BSRR = pinTglMask;
}

GPOut& GPOut::operator !()
{
	pGroup->BSRR = pinTglMask;
	return *this;
}

void GPOut::ToHigh()
{
	pGroup->BSRR = pinSRMask;
}

void GPOut::operator = (int val)
{
	(val == 0)? pGroup->BRR = pinSRMask : pGroup->BSRR = pinSRMask;
}


void GPOut::operator = (bool val)
{
	(val == false)? pGroup->BRR = pinSRMask : pGroup->BSRR = pinSRMask;
}

