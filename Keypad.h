#ifndef KEYPAD_H
#define KEYPAD_H

unsigned char Read_Keypad(void);

int arr_to_int(char *p);


#define ROW_PINS        (0x0F000000)   // P1.24?P1.27

#define COL_PINS        (0xF0000000)   // P1.28?P1.31
void Keypad_ReadString(char *buf, int max_len);

#endif

