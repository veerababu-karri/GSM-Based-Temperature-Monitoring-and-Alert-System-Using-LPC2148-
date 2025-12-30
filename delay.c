#include <lpc214x.h>   

void delay_us(int DELY_us)
{
	DELY_us *= 12;        // Multiply to obtain approximate ?s delay count
	while(DELY_us--);    // Loop executes until counter reaches zero
}

void delay_ms(int DELY_ms)
{
	DELY_ms *= 12000;     // Loop scaling for ~1ms timing
	while(DELY_ms--);     // Busy loop execution
}

void delay_s(int DELY_s)
{
	DELY_s *= 12000000;   // Scale loop counter for second timing
	while(DELY_s--);      // Loop delays execution for given time
}
