#ifndef UART1_H
#define  UART1_H

void UART1_ini( void );

void UART1_SendByte (char data); 

void UART1_SendString(char* str, unsigned int len);

#endif
