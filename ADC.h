#ifndef ADC_H
#define ADC_H


/*********************************************************
 *                ADC / TEMPERATURE FUNCTIONS
 *********************************************************/

// Reads ADC value (10-bit) from the selected channel
// (In your code: AD0.1 / AD0.7 depending on implementation)
int adc_value(void);

// Converts ADC reading to temperature using LM35 sensor
// LM35 ? 10mV/?C ? Temperature = (Voltage * 100)
float get_temperature(void);

void ADC_Init(void);
#define BUZZER_PIN  (1<<25)   // Example P0.25

#endif
