#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

extern volatile unsigned char menu_mode;

extern int pin;
void Menu_Show(void);
void Menu_Handle(void);
void EINT0_Init(void);


#endif
