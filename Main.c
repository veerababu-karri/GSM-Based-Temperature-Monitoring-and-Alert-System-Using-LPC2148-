#include "eeprom.h"
#include "lcd.h"
#include "gsm.h"
#include "uart.h"
#include "adc.h"
#include "delay.h"
#include <lpc214x.h>
#include "interrupt.h"
#include "keypad.h"
#include "rtc.h"
#include "timer0.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

unsigned char degree_symbol[8] = {
        0x06,   //  ** 
        0x09,   // *  *
        0x09,   // *  *
        0x06,   //  ** 
        0x00,
        0x00,
        0x00,
        0x00
};
char sms[460];
unsigned char temp;

int main(void)
{
    int current_temp;
    int set_point;
    int alert_sent = 0;   // prevents repeated SMS flooding

    /* ---------------- INIT HARDWARE ---------------- */
    IODIR0 |= BUZZER_PIN|(7<<20);       // Buzzer as output
    IOCLR0 = BUZZER_PIN;        // Buzzer OFF
		IOSET0=(5<<21);
	
    lcd_init();
		Timer0_Init();
    UART0_Init();
		EINT0_Init(); 
    ADC_Init();
    I2C_Init();
		rtc_init();
    lcd_clear();
    lcd_print("SYSTEM START");
    delay_ms(1500);

    /* ---------------- GSM INIT ---------------- */
    GSM_Init();
		EEPROM_WriteString(EEPROM_MOB_ADDR, "9112449698");
		EEPROM_WriteString(PASSKEY, "1122");

    /* ---------------- MAIN LOOP ---------------- */
    while (1)
    {
			
				/* ---- MENU MODE (EINT0) ---- */
                /* ---- MENU MODE (EINT0) ---- */
        if (menu_mode)
        {
            Menu_Handle();

            /* Reload updated values after menu exit */
            if (EEPROM_ReadByte(EEPROM_TEMP_ADDR, &temp))
									set_point = temp;
            EEPROM_ReadString(EEPROM_MOB_ADDR, phone_read);

            continue;
        }

        /* ---- MODEM RESET DETECT ---- */
        if (gsm_resp == GSM_MODEM_RESET)
        {
            lcd_clear();
            lcd_print("MODEM RESET");
            delay_ms(1500);
            GSM_Init();
            gsm_resp = GSM_NONE;
        }
				
				
				EEPROM_ReadString(0x0000, phone_read);
        /* ---- READ CURRENT TEMPERATURE ---- */
        current_temp = get_temperature();

        /* ---- READ SET POINT FROM EEPROM ---- */
        if (EEPROM_ReadByte(EEPROM_TEMP_ADDR, &temp))
									set_point = temp;

        /* ---- DISPLAY ON LCD ---- */
        lcd_clear();
				rtc_display();
				lcd_cmd(0x8A); 
        lcd_print("T:");
        int_display(current_temp);
				lcd_cmd(0x8F);       // Move cursor to column 9 on row 1
				lcd_data('C');       // Print C

				// Load custom degree symbol into CGRAM location 0
				LCD_Custom_Char(0, degree_symbol);

				lcd_cmd(0x8E);       // Move cursor to column 8 (before C)
				lcd_data(0);         // Print custom character from CGRAM


        lcd_cmd(0xCA);
        lcd_print("S:");
        int_display(set_point);
				lcd_cmd(0xCF);       // Move cursor to column 9 on row 1
				lcd_data('C');       // Print C

				// Load custom degree symbol into CGRAM location 0
				LCD_Custom_Char(0, degree_symbol);

				lcd_cmd(0xCE);       // Move cursor to column 8 (before C)
				lcd_data(0);         // Print custom character from CGRAM

        delay_ms(500);

        /* ---- TEMPERATURE ALERT LOGIC ---- */
        if (current_temp > set_point)
        {
            IOSET0 = BUZZER_PIN;    // Buzzer ON

            if (!alert_sent)
            {
								char rtc_buf[30];

								RTC_GetDateTime(rtc_buf);
								
								snprintf(sms, sizeof(sms),
													"Alert: The current temperature(%d) has exceeded the configured set point (%d) @ %s",
													current_temp,set_point,rtc_buf);
										

								GSM_SendSMS(phone_read, sms);

                alert_sent = 1;
            }
        }
        else
        {
            IOCLR0 = BUZZER_PIN;    // Buzzer OFF
            alert_sent = 0;         // reset alert when temp normal
        }

        /* ---- CHECK INCOMING SMS ---- */
        if (gsm_resp == GSM_CMTI)
        {
            sms_index = GSM_ExtractSMSIndex(rx_line);

            if (sms_index >= 0)
            {
                GSM_ReadSMS(sms_index);
            }

            gsm_resp = GSM_NONE;
            rx_ready = 0;
        }
				

        delay_ms(300);
    }
}
