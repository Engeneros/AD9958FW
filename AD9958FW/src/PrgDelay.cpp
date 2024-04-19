#include <PrgDelay.h>

void DelayMs(unsigned int mSec)
{
	unsigned int nTics;
	for( nTics = mSec * 1000; nTics > 0; nTics--)
	{
	}	
}
void DelayUs(unsigned int uSec)
{
	unsigned int nTics;
	for( nTics = uSec; nTics > 0; nTics--)
	{
	}	
}

void wait2 (void)  {
  int  d;

  for (d = 0; d < 4000000; d++);     
}

void wait (void)  {
  int  d;

  for (d = 0; d < 2000000; d++);     
}

void wait12 (void)  {
  int  d;

  for (d = 0; d < 1000000; d++);     
}

void wait14 (void)  {
  int  d;

  for (d = 0; d < 500000; d++);     
}

void wait18 (void)  {
  int  d;

  for (d = 0; d < 250000; d++);     
}
