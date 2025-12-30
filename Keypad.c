#include "keypad.h"
#include "lcd.h"
#include <lpc214x.h>
#include "delay.h"
#include "interrupt.h"

/*unsigned char keypad[4][4] = {
    {'7','4','1','*'},
    {'8','5','2','0'},
    {'9','6','3','#'},
    {'A','B','C','D'}
};   with respect to hardware*/

/*unsigned char keypad[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
}; // with consideration to new keypad*/


unsigned int keypad[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};


int arr_to_int(char *p)
{
    int num = 0, dig = 0;

    while(*p)
    {
        dig = *p - '0';       // Convert ASCII digit to integer
        num = num * 10 + dig; // Accumulate number
        p++;                  // Move to next character
    }
    return num;
}	


unsigned char Read_Keypad(void)
{
    unsigned int row, col;

    /*-------------------------------------------------------
     Configure ROW pins as OUTPUT and COL pins as INPUT
    -------------------------------------------------------*/
    IO1DIR |= ROW_PINS;     // Rows ? Output
    IO1DIR &= ~COL_PINS;    // Columns ? Input

    /*-------------------------------------------------------
     Ensure all PORT1 pins act as GPIO
    -------------------------------------------------------*/
    PINSEL2 = 0x00000000;

    /*-------------------------------------------------------
     Enable pull-up on column pins by writing HIGH
     - Columns remain HIGH when no key is pressed
    -------------------------------------------------------*/
    IO1SET = COL_PINS;     

    while(1)
    {
        /*---------------------------------------------------
         Scan each row one by one
        ---------------------------------------------------*/
        for(row = 0; row < 4; row++)
        {
            IO1SET = ROW_PINS;                 // Set all rows HIGH
            IO1CLR = (1 << (24 + row));        // Pull current row LOW

            /*-----------------------------------------------
             Check each column for LOW signal
            -----------------------------------------------*/
            for(col = 0; col < 4; col++)
            {
                if(!(IO1PIN & (1 << (28 + col))))
                {
                    /*---------------------------------------
                     Key press detected
                     LOW indicates closed switch
                    ---------------------------------------*/
                    delay_ms(20);              // Debounce delay

                    /* Wait until key is released */
                    while(!(IO1PIN & (1 << (28 + col))));

                    /* Return corresponding key value */
                    return keypad[row][col];
                }
            }
        }
    }
}
void Keypad_ReadString(char *buf, int max_len)
{
    char key;
    int index = 0;

    buf[0] = '\0';

    while (1)
    {
        key = Read_Keypad();

        /* ---- DIGIT PRESSED ---- */
        if (key >= '0' && key <= '9')
        {
            if (index < (max_len - 1))
            {
                buf[index++] = key;
                buf[index] = '\0';
								if(!pin)
									lcd_data(key);      // echo on LCD
								else
									lcd_data('*');
						}
        }

        /* ---- CLEAR INPUT (*) ---- */
        else if (key == '*')
        {
            index = 0;
            buf[0] = '\0';

            lcd_cmd(0xC0);
            lcd_print("                ");  // clear row
            lcd_cmd(0xC0);
        }

        /* ---- BACKSPACE (A) ---- */
        else if (key == 'A')
        {
            if (index > 0)
            {
                index--;
                buf[index] = '\0';

                lcd_cmd(0xC0 + index);
                lcd_data(' ');
                lcd_cmd(0xC0 + index);
            }
        }

        /* ---- ENTER (#) ---- */
        else if (key == '#')
        {
            return;   // input complete
        }

        delay_ms(150);   // debounce + user readability
    }
}
