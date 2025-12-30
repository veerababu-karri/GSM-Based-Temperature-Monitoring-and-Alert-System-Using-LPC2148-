#include <lpc214x.h>
#include "lcd.h"
#include "RTC.h"
#include <stdio.h>

#define RTC_RST (1<<1)
#define RTC_EN 1

volatile unsigned char edit_mode = 0;
unsigned char field = 0;
char digit_buf[5];
unsigned char digit_cnt = 0;


void rtc_init(void)
{
			//VPBDIV=0x00;
			//PREINT=456;
			//PREFRAC=25024;
				
      CCR |= RTC_RST;    // Reset RTC
/*
      HOUR  = 10;
      MIN   = 15;
      SEC   = 0;
      DOM   = 17;
      MONTH = 12;
      YEAR  = 2025;
      DOW   = 4;
*/
      //CCR &= ~RTC_RST;   // Release reset
      CCR = RTC_EN|(1<<4);     // Start RTC
    
}

/* Get date & time as string */
void RTC_GetDateTime(char *buf)
{
    sprintf(buf,
            "%02d/%02d/%04d %02d:%02d:%02d",
            DOM, MONTH, YEAR,
            HOUR, MIN, SEC);
}
void rtc_display(void)
{
    //char day[7][4]={"SUN","MON","TUE","WED","THU","FRI","SAT"};

    lcd_cmd(0x80);
    lcd_data(HOUR/10+'0'); lcd_data(HOUR%10+'0'); lcd_data(':');
    lcd_data(MIN/10+'0');  lcd_data(MIN%10+'0');  lcd_data(':');
    lcd_data(SEC/10+'0');  lcd_data(SEC%10+'0');  lcd_data(' ');
    //lcd_print(day[DOW]);

    lcd_cmd(0xC0);
    lcd_data(DOM/10+'0'); lcd_data(DOM%10+'0'); lcd_data('/');
    lcd_data(MONTH/10+'0'); lcd_data(MONTH%10+'0'); lcd_data('/');
    //lcd_data(YEAR/1000+'0');
    //lcd_data((YEAR/100)%10+'0');
    lcd_data((YEAR/10)%10+'0');
    lcd_data(YEAR%10+'0');
}

int validate(unsigned char f, unsigned int v)
{
    switch(f)
    {
        case 0: return (v <= 23);
        case 1: return (v <= 59);
        case 2: return (v <= 59);
        case 3: return (v >= 1 && v <= 31);
        case 4: return (v >= 1 && v <= 12);
        case 5: return (v >= 2000);
        case 6: return (v <= 6);
    }
    return 0;
}

void rtc_write(unsigned char f, unsigned int v)
{
    CCR &= ~(1 << 0);   // Stop RTC

    switch(f)
    {
        case 0: HOUR  = v; break;
        case 1: MIN   = v; break;
        case 2: SEC   = v; break;
        case 3: DOM   = v; break;
        case 4: MONTH = v; break;
        case 5: YEAR  = v; break;
        case 6: DOW   = v; break;
    }

    CCR |= (1 << 0);    // Restart RTC
}


char *field_name[] =
{
    "SET HOUR:",
    "SET MIN:",
    "SET SEC:",
    "SET DATE:",
    "SET MONTH:",
    "SET YEAR:",
    "SET DAY:"
};


void rtc_edit_display(void)
{
    unsigned char i;

    lcd_cmd(0x01);          // Clear LCD
    lcd_cmd(0x80);
    lcd_print(field_name[field]);

    lcd_cmd(0xC0);
    for(i = 0; i < digit_cnt; i++)
        lcd_data(digit_buf[i]);
}


void rtc_edit(char key)
{
    unsigned int val = 0;
    unsigned char i;

    /* EXIT immediately if 'A' pressed */
    if(key == 'A')
    {
        edit_mode = 0;
        digit_cnt = 0;
        field = 0;
        lcd_cmd(0x01);   // Clear LCD
        return;
    }

    /* Numeric input */
    if(key >= '0' && key <= '9' && digit_cnt < 4)
    {
        digit_buf[digit_cnt++] = key;
    }

    /* Clear current entry */
    else if(key == '*')
    {
        digit_cnt = 0;
    }

    /* NEXT / ENTER */
    else if(key == '#')
    {
        /* Only update RTC if user entered digits */
        if(digit_cnt > 0)
        {
            for(i = 0; i < digit_cnt; i++)
                val = val * 10 + (digit_buf[i] - '0');

            if(validate(field, val))
                rtc_write(field, val);
        }
        /* else: skip field, keep old RTC value */

        digit_cnt = 0;
        field++;

        /* Exit after last field */
        if(field > 6)
        {
						
            edit_mode = 0;
            field = 0;
            lcd_cmd(0x01);   // Clear LCD
        }
    }
}
