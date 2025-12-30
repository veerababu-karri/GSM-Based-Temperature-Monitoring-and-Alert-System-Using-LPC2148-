#include "eeprom.h"
#include "lcd.h"
#include "gsm.h"
#include "uart.h"
#include "adc.h"
#include "delay.h"
#include <lpc214x.h>
#include "interrupt.h"
#include "keypad.h"
#include "string.h"
#include "stdio.h"
#include "RTC.h"
#include <stdio.h>
#include <stdlib.h>


volatile unsigned char menu_mode;
char buf[15];
int pin;

/* -------- EINT0 ISR -------- */
void EINT0_ISR(void) __irq
{
    menu_mode = 1;
		edit_mode = 1;
    EXTINT = (1<<0);
    VICVectAddr = 0;
}


void EINT0_Init()
{
    // --- Configure P0.16 as EINT0 ---
    PINSEL1 &= ~(3 << 0);
    PINSEL1 |=  (1 << 0);   // EINT0 function

    // --- Configure EINT0 for falling edge ---
    EXTMODE  |= (1 << 0);    // Edge sensitive
    EXTPOLAR &= ~(1 << 0);   // Falling edge

    // --- Configure VIC ---
    VICIntSelect &= ~(1 << 14);        // MUST SET: EINT0 as IRQ
    VICIntEnable |= (1 << 14);         // Enable EINT0 interrupt

    VICVectCntl0 = (1 << 5) | 14;      // Enable slot 0, assign EINT0
    VICVectAddr0 = (unsigned)EINT0_ISR;  // ISR address
}


int Validpass()
{
		pin=1;
		EEPROM_ReadString(PASSKEY, pass);
		lcd_clear();
		lcd_print("Enter PIN:");
		lcd_cmd(0xC0);
		Keypad_ReadString(buf,5);
		if (strcmp(buf,pass) == 0)
		{
			lcd_clear();
			lcd_print("Valid Pass");
			delay_ms(1000);
			pin=0;
			return 1;
		}
		lcd_clear();
		lcd_print("Invalid Pass");
		delay_ms(1000);
		pin=0;
		return 0;
}
/* -------- MENU DISPLAY -------- */
void Menu_Show(void)
{
    lcd_clear();
    lcd_print("1:SP");
    lcd_cmd(0xC0);
		lcd_print("2:Phn");
		lcd_cmd(0x86);
		lcd_print("3:RTC");
		lcd_cmd(0xC6);
		lcd_print("4:EXIT");
    //delay_ms(2000);

}

/* -------- MENU HANDLER -------- */
void Menu_Handle(void)
{
		char key;
    int value;
		menu_mode = 0;
		if(!Validpass())
			return;

    

    while (1)
    {
        Menu_Show();
        key = Read_Keypad();

        /* ---- UPDATE SET POINT ---- */
        if (key == '1')
        {
            lcd_clear();
            lcd_print("Enter Set:");
            lcd_cmd(0xC0);

            Keypad_ReadString(buf, 3);   // e.g. 35
            value = atoi(buf);

            EEPROM_WriteByte(EEPROM_TEMP_ADDR, value);

            lcd_clear();
            lcd_print("SET UPDATED");
            delay_ms(1500);
        }

        /* ---- UPDATE PHONE NUMBER ---- */
        else if (key == '2')
        {
            lcd_clear();
            lcd_print("Enter Mobile:");
            lcd_cmd(0xC0);

            Keypad_ReadString(buf, 11);  // 10-digit number
            EEPROM_WriteString(EEPROM_MOB_ADDR, buf);

            lcd_clear();
            lcd_print("MOB UPDATED");
            delay_ms(1500);
        }
				
				else if(key == '3')
				{
						char k;
						while(1)
						{
								if(edit_mode==1)
								{	
						
										rtc_edit_display();
										k = Read_Keypad();
            
										if(k)
										{

												rtc_edit(k);

												delay_ms(250);
	
										}
									}
								else
									break;
							}
					}
				

        /* ---- EXIT MENU ---- */
        else if (key == '4')
        {
            lcd_clear();
            lcd_print("EXIT MENU");
            delay_ms(1000);
            break;
        }
    }
}
