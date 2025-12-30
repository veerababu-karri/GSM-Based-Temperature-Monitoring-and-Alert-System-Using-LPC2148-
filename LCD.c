#include <lpc214x.h>        // Header defining registers for LPC2148 microcontroller
#include "lcd_defines.h"    // Contains user-defined pin mappings and LCD command macros
#include "delay.h"      // Custom delay functions used for LCD timing compliance


void lcd_cmd(char i)
{
	// Clear old data on LCD data pins to avoid mixing previous values
	IOCLR1 = 0xFF << DATA_PINS;

	// Select command register (RS=0 ? instruction mode)
	IOCLR0 = 1 << RS;

	// Load command onto data pins
	IOSET1 = i << DATA_PINS;

	// Generate Enable Pulse (E=1 ? LCD reads data, then E=0 ? latch)
	IOSET0 = 1 << E;
	delay_ms(1);            // Allow LCD to process instruction timing
	IOCLR0 = 1 << E;
}

void lcd_data(char i)
{
	// Clear previous data output
	IOCLR1 = 0xFF << DATA_PINS;

	// Select data register (RS=1 ? write data to DDRAM)
	IOSET0 = 1 << RS;

	// Send new character to LCD data pins
	IOSET1 = i << DATA_PINS;

	// Trigger enable pulse
	IOSET0 = 1 << E;
	delay_ms(1);
	IOCLR0 = 1 << E;
}


void lcd_print(char *p)
{
	while (*p)
   		lcd_data(*p++);   // Send character then move pointer to next
}


void lcd_init()
{
    /* GPIO direction */
    IODIR1 |= (0xFF << DATA_PINS);
    IODIR0 |= (1 << RS) | (1 << E);

    /* Power-on delay */
    delay_ms(40);

    lcd_cmd(SET);
    delay_ms(2);

    lcd_cmd(ENTRY);
    delay_ms(2);

    lcd_cmd(DISPLAY_ON);
    delay_ms(2);

    lcd_cmd(CLR_LCD);
    delay_ms(2);

    lcd_cmd(FIRST_ROW);
    delay_ms(2);
}


void int_display(int n)
{
	char ch[10];     // Buffer to store reversed digits
	int count = 0;
	int negative_flag = 0;

	// If number is zero, print directly and return
	if (n == 0)
	{
		lcd_data('0');
		return;
	}

	// Check for negative input
	else if (n < 0)
	{
		n *= -1;        // Convert to positive for further processing
		negative_flag = 1;
	}

	// Extract digits one by one (reverse order)
	while (n != 0)
	{
		int rem = n % 10;
		ch[count] = rem + 48; // Convert integer -> ASCII using +48 ('0')
		count++;
		n /= 10;
	}

	// Append minus sign if originally negative
	if (negative_flag == 1)
		ch[count] = '-';
	else
		count--;  // Adjust pointer for printing

	// Print digits in correct order (reverse loop)
	for (; count >= 0; count--)
	{
		lcd_data(ch[count]);
	}
}

void LCD_Custom_Char (unsigned char loc, unsigned char *msg)
{
	unsigned char i;

	// LCD supports only 8 user-defined characters (indexes: 0-7)
	if (loc < 8)
	{
		// Set CGRAM address starting point (each char occupies 8 bytes)
		lcd_cmd(0x40 + (loc * 8));

		// Write 8 bytes data to form the pixel pattern
		for (i = 0; i < 8; i++)
			lcd_data(msg[i]);
	}
}


void lcd_clear(void)
{
    lcd_cmd(0x01);
		lcd_cmd(0x80);
}

