#ifndef LCD_H     // Prevent multiple inclusion of this header file
#define LCD_H


void delay(unsigned int dly);

void lcd_cmd(char i);

void lcd_data(char i);

void lcd_print(char *p);

void lcd_init(void);

void int_display(int n);


void LCD_Custom_Char(unsigned char loc, unsigned char *msg);

void lcd_clear(void);


#endif // LCD_H

