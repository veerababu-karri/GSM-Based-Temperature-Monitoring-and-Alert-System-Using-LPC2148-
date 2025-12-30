#ifndef LCD_DEFINES_H        // Prevents duplicate inclusion of this header file
#define LCD_DEFINES_H        // Marks the file as included


#define RS 5      // Register Select pin connected to P0.5 
                  // (RS = 0 ? Command mode, RS = 1 ? Data mode)

#define E 6       // Enable pin connected to P0.6 
                  // (High-to-low pulse on E latches data/command into LCD)


#define CLR_LCD     0x01    // Clear display and reset cursor to home position
#define SET         0x38    // Function set: 8-bit mode, 2-line display, 5x8 font
#define ENTRY       0x06    // Entry mode: increment cursor, no display shift
#define DISPLAY_ON  0x0E    // Turn display ON with cursor ON and blinking OFF
#define FIRST_ROW   0x80    // Move cursor to beginning of first row (DDRAM address 0x00)


#define DATA_PINS 16  // LCD 8-bit data lines (D0?D7) are mapped to P1.16?P1.23


#endif // LCD_DEFINES_H

