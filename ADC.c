#include <lpc214x.h>
#include "ADC.h"
#include "ADC_defines.h"
#include "delay.h"
#include "lcd.h"



void ADC_Init()
{
	PINSEL1 |= 0x01000000;
}


/*******************************************************
 * get_temperature()
 * Reads LM35 output through ADC and converts to ?C:
 * LM35 Output: 10mV/?C
 * ADC Voltage = (raw * 3.3V / 1023)
 * Temp = Voltage * 100
 *******************************************************/
float get_temperature()
{
    int raw = adc_value();                        // 10-bit ADC value
    float voltage = (raw * 3.3f) / 1023.0f;       // Convert to voltage
    float tempC = voltage * 100.0f;               // LM35 scaling
    return tempC;
}


/*******************************************************
 * adc_value()
 * Reads ADC0 channel 1 (AD0.1 = P0.28)
 * Steps:
 *  - Configure ADC
 *  - Start conversion
 *  - Wait for DONE flag
 *  - Extract 10-bit ADC result
 *******************************************************/
int adc_value()
{
    unsigned int adcdata;

    AD0CR = (1<<1)        |   // SEL: select ADC channel 1
            (49<<8)       |   // CLKDIV: ADC clock divider
            (1<<21);          // PDN: Enable ADC

    AD0CR |= (1<<24);         // Start conversion NOW

    while((AD0GDR & (1UL<<31)) == 0);  // Wait until DONE = 1

    adcdata = (AD0GDR >> 6) & 0x3FF; // Extract 10-bit result

    return adcdata;
}
