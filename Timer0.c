#include <lpc214x.h>
#include "timer0.h"

/* LED pin */
#define LED_PIN  (1<<20)

/* -------- TIMER0 ISR -------- */
void TIMER0_ISR(void) __irq
{
    IO0PIN ^= LED_PIN;   // Toggle LED

    T0IR = 1;            // Clear MR0 interrupt
    VICVectAddr = 0;    // End of ISR
}

/* -------- TIMER0 INIT -------- */
void Timer0_Init(void)
{
    /* Configure LED pin */
    IODIR0 |= LED_PIN;
    IOCLR0  = LED_PIN;

    /* Timer configuration */
    T0PR  = 15000 - 1;   // Prescaler: 1 ms tick
    T0MR0 = 500;         // 500 ms

    T0MCR = 3;           // Interrupt + Reset on MR0

    /* Enable Timer0 interrupt */
    VICIntEnable |= (1<<4);      // Timer0 interrupt
    VICVectCntl2  = (1<<5) | 4;  // Slot 2 (lower than EINT0, UART)
    VICVectAddr2  = (unsigned)TIMER0_ISR;

    /* Start Timer0 */
    T0TCR = 1;
}
