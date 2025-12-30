#include <lpc214x.h>     
#include <string.h>      

#include "UART.h"


volatile char rx_buffer[100];
char rx_line[100];
volatile int rx_index = 0;
volatile int rx_ready = 0;
volatile GSM_Response_t gsm_resp = GSM_NONE;


void UART0_ISR(void) __irq

{
    char ch;   // Temporary variable to store received character

    
    if (U0IIR & 0x04)
    {
        ch = U0RBR;

        /* ---------------- SMS PROMPT HANDLING ---------------- */
        if (ch == '>')
        {
            /* '>' indicates GSM is waiting for SMS text (AT+CMGS) */

            rx_buffer[0] = '>';
            rx_buffer[1] = '\0';   // Null terminate string

            gsm_resp = GSM_PROMPT; // Inform main code
            rx_ready = 1;          // Response ready
            rx_index = 0;          // Reset buffer index
        }

        /* ---------------- END OF LINE HANDLING ---------------- */
        else if (ch == '\r' || ch == '\n')
        {
            /* GSM responses end with CR/LF */

            if (rx_index > 0)
            {
                /* Terminate string */
                rx_buffer[rx_index] = '\0';

                /* Copy from volatile buffer to normal buffer
                   WHY?
                   - Standard C string functions do not work reliably
                     on volatile memory */
                strcpy(rx_line, (char *)rx_buffer);

                /* ---------------- MODEM RESET / NOT READY ---------------- */
                if (strstr(rx_line, "PBSTART") ||
                    strstr(rx_line, "MODEM") ||
                    strstr(rx_line, "SMS Ready") ||
                    strstr(rx_line, "Call Ready"))
                {
                    /* GSM module restarted or not ready */
                    gsm_resp = GSM_MODEM_RESET;
                    rx_ready = 1;
                }

                /* ---------------- STANDARD GSM RESPONSES ---------------- */
                else if (strstr(rx_line, "OK"))
                    gsm_resp = GSM_OK;

                else if (strstr(rx_line, "ERROR"))
                    gsm_resp = GSM_ERROR;

                else if (strstr(rx_line, "+CMGS"))
                    gsm_resp = GSM_CMGS;   // SMS sent confirmation

                else if (strstr(rx_line, "+CMTI"))
                    gsm_resp = GSM_CMTI;   // New SMS received

                else if (strstr(rx_line, "RING"))
                {
                    gsm_resp = GSM_RING;

                    /* Automatically reject call
                       Alternative:
                       - ATH ? hang up
                       - ATA ? answer call */
                    UART0_TxString("ATH0\r\n");
                }
                else
                {
                    /* Unknown or unhandled response */
                    gsm_resp = GSM_NONE;
                }

                rx_ready = 1;   // Signal main loop
                rx_index = 0;   // Prepare for next message
            }
        }

        /* ---------------- NORMAL CHARACTER ---------------- */
        else
        {
            /* Store received character into buffer */
            rx_buffer[rx_index++] = ch;

            /* Prevent buffer overflow */
            if (rx_index >= 99)
                rx_index = 0;
        }
    }

    /* Mandatory: Tell VIC that ISR execution is complete */
    VICVectAddr = 0;
}


/* ---------------- UART0 INITIALIZATION ---------------- */

void UART0_Init(void)
{
    /* Configure P0.0 as TXD0 and P0.1 as RXD0 */
    PINSEL0 |= 0x00000005;

    /* Enable access to divisor latches */
    U0LCR = 0x83;   // 8-bit data, 1 stop bit, no parity, DLAB = 1

    /* Baud rate configuration:
       9600 baud @ 15 MHz PCLK
       Formula: Baud = PCLK / (16 ? DLL)
       DLL = 97 */
    U0DLL = 97;
    U0DLM = 0;

    /* Disable divisor latch access */
    U0LCR = 0x03;

    /* Enable RX interrupt only
       Alternative:
       - 0x03 ? RX + THRE interrupts */
    U0IER = 0x01;

    /* Enable UART0 interrupt in VIC (Interrupt number 6) */
    VICIntEnable |= (1<<6);

    /* Assign UART0 interrupt to VIC slot 1 */
    VICVectCntl1 = (1<<5) | 6;   // Enable slot + UART0

    /* Set ISR address */
    VICVectAddr1 = (unsigned)UART0_ISR;
}


/* ---------------- UART TRANSMIT CHARACTER ---------------- */

void UART0_TxChar(char ch)
{
    /* Wait until Transmit Holding Register is empty */
    while(!(U0LSR & (1<<5)));

    /* Send character */
    U0THR = ch;
}


/* ---------------- UART TRANSMIT STRING ---------------- */

void UART0_TxString(char *str)
{
    /* Send characters one by one until null terminator */
    while(*str)
        UART0_TxChar(*str++);
}
