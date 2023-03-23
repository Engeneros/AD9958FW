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


