#include "gsm.h"
#include "uart.h"
#include "lcd.h"
#include "delay.h"
#include "eeprom.h"
#include "adc.h"
#include <lpc214x.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


char sms_outbox[140];
volatile int sms_pending = 0;

char sms_number[20];
char phone_read[15];
char pass[5];
char msg_cmd [260];
volatile int gsm_busy = 0;

volatile int sms_index = -1;
char sms_header[100];
char sms_body[100];
char *gsm_cmds[] =
{
    "AT\r\n",                  // Check GSM presence
    "ATE0\r\n",                // Disable echo
    "AT+CMGF=1\r\n",           // Set SMS text mode
    "AT+CNMI=2,1,0,0,0\r\n"    // Enable new SMS indication
    //"AT+CMGD=1\r"             // Optional: delete SMS
};

int GSM_WaitForResponse(unsigned int timeout_ms)
{
    /* Loop until timeout expires */
    while (timeout_ms--)
    {
        /* rx_ready is set by UART ISR
           when a complete response is received */
        if (rx_ready)
            return 1;   // Success

        delay_ms(1);   // 1 ms granularity
    }

    /* Timeout occurred */
    return 0;
}

int CheckPasskey(char *msg)
{
		EEPROM_ReadString(PASSKEY, pass);
    return (strncmp(msg, pass, 4) == 0);
}

char GetCommandType(char *msg)
{
    return msg[4];   // XXXX C
}

void GetCommandData(char *msg, char *data)
{
    int i = 5, j = 0;

    while (msg[i] != '$' && msg[i] != '\0')
    {
        data[j++] = msg[i++];
    }
    data[j] = '\0';
}


void SendSensorInfo(void)
{
    int temp = get_temperature();

    snprintf(sms_outbox, sizeof(sms_outbox),
        "The current temperature is %d degree Celsius."
        ,temp);

		
    GSM_SendSMS(phone_read, sms_outbox);
}

void SendSetpointInfo(void)
{
		int set_point;
    if (EEPROM_ReadByte(EEPROM_TEMP_ADDR, &temp))
			set_point = temp;

		snprintf(sms_outbox, sizeof(sms_outbox),
        "The current set point is %d degree Celsius.\r\n"
        "Thank you.", set_point);

    //sprintf(msg, "SET POINT:%dC", set_point);
    GSM_SendSMS(phone_read, sms_outbox);
}

/* ---------------------------------------------------
   GSM INITIALIZATION FUNCTION
--------------------------------------------------- */
void GSM_Init(void)
{
    int cmd = 0;   // Index of current AT command

    /* Infinite loop ensures GSM eventually initializes */
    while(1)
    {
        rx_ready = 0;              // Clear previous response flag
        gsm_resp = GSM_NONE;       // Reset GSM response state

        /* ---- DISPLAY CURRENT COMMAND ON LCD ---- */
        lcd_clear();
        delay_ms(5);

        lcd_cmd(0x80);             // Move cursor to first line
        lcd_print(gsm_cmds[cmd]);  // Show command being sent
        delay_ms(500);

        lcd_cmd(0xC0);             // Move cursor to second line
        delay_ms(2);

        /* ---- SEND AT COMMAND TO GSM ---- */
        UART0_TxString(gsm_cmds[cmd]);

        /* ---- WAIT FOR GSM RESPONSE ---- */
        if (!GSM_WaitForResponse(GSM_TIMEOUT_MS))
        {
            /* No response ? GSM likely hung */
            GSM_InitError();
            cmd = 0;               // Restart init sequence
            continue;
        }

        /* If GSM rebooted during init */
        if (gsm_resp == GSM_MODEM_RESET)
        {
            GSM_InitError();
            cmd = 0;
            continue;
        }

        /* ---- DISPLAY GSM RESPONSE ---- */
        lcd_print(rx_line);
        delay_ms(1200);            // Important: GSM needs time

        /* ---- PROCESS RESPONSE ---- */
        if (gsm_resp == GSM_OK)
        {
            cmd++;   // Move to next AT command

            /* All commands executed successfully */
            if (cmd >= 4)
            {
                lcd_clear();
                delay_ms(5);
                lcd_print("GSM READY");
								IOSET0=1<<22;
                delay_ms(2000);
                break;   // Exit init loop
            }
        }
        else
        {
            /* Any ERROR ? restart GSM initialization */
            GSM_InitError();
						IOCLR0=1<<22;
            delay_ms(2000);
            cmd = 0;
        }

        /* Allow GSM internal processing to settle */
        delay_ms(500);
    }
		

}


/* ---------------------------------------------------
   SEND SMS FUNCTION
--------------------------------------------------- */
void GSM_SendSMS(char *number, char *msg)
{
	
    rx_ready = 0;
    gsm_resp = GSM_NONE;

    /* Display destination number */
    lcd_clear();
    lcd_cmd(0x80);
    lcd_print("TO:");
    lcd_print(number);
    delay_ms(1500);

    lcd_clear();
    lcd_cmd(0x80);
    lcd_print("SEND SMS");
    delay_ms(500);

    /* Step 1: Send AT+CMGS command */
    UART0_TxString("AT+CMGS=\"");
    UART0_TxString(number);
    UART0_TxString("\"\r\n");

    /* Wait for '>' prompt */
    if (!GSM_WaitForResponse(GSM_SMS_TIMEOUT_MS))
    {
        GSM_InitError();
        GSM_Init();
        return;
    }

    if (gsm_resp == GSM_MODEM_RESET)
    {
        GSM_InitError();
        GSM_Init();
        return;
    }

    /* GSM must respond with '>' */
    if (gsm_resp != GSM_PROMPT)
    {
        lcd_clear();
        lcd_cmd(0x80);
        lcd_print("NO PROMPT");
        delay_ms(2000);
        return;
    }

    /* Step 2: Send message text */
    rx_ready = 0;
    gsm_resp = GSM_NONE;

    lcd_clear();
    lcd_cmd(0x80);
    lcd_print("SENDING...");
    delay_ms(500);

    UART0_TxString(msg);
    UART0_TxChar(0x1A);   // CTRL+Z terminates SMS

    /* Step 3: Wait for confirmation */
    //while(!rx_ready);
		if (!GSM_WaitForResponse(GSM_SMS_TIMEOUT_MS))
    {
        GSM_InitError();
        GSM_Init();
        return;
    }
    lcd_clear();
    lcd_print(rx_line);
    delay_ms(1500);

    if (gsm_resp == GSM_CMGS || gsm_resp == GSM_OK)
    {
        lcd_clear();
        lcd_cmd(0x80);
        lcd_print("SMS SENT");
        delay_ms(2000);
    }
    else
    {
        lcd_clear();
        lcd_cmd(0x80);
        lcd_print("SMS FAILED");
        delay_ms(2000);
    }
}

void NormalizePhoneNumber(char *num)
{
    int len = strlen(num);

    /* Case 1: starts with "91" and length > 10 */
    if (len > 10 && num[0] == '9' && num[1] == '1')
    {
        memmove(num, num + 2, len - 1);  // shift left by 2
    }

    /* Case 2: starts with "+91" (optional safety) */
    else if (len > 10 && num[0] == '+' && num[1] == '9' && num[2] == '1')
    {
        memmove(num, num + 3, len - 2);
    }
}


/* ---------------------------------------------------
   CHECK IF SMS IS FROM AUTHORIZED NUMBER
--------------------------------------------------- */
int GSM_IsAuthorizedNumber(char *header)
{
    char *p;
    int i = 0;

    /* GSM header format:
       +CMGR:"REC READ","919xxxxxxxx","",date */

    /* Find first quote */
    p = strchr(header, '"');
    if (!p) return 0;

    /* Skip REC READ */
    p = strchr(p + 1, '"');
    if (!p) return 0;

    /* Start of phone number */
    p = strchr(p + 1, '"');
    if (!p) return 0;

    p++;  // Point to first digit

    /* Copy phone number */
    while (*p && *p != '"' && i < sizeof(sms_number) - 1)
    {
        sms_number[i++] = *p++;
    }
    sms_number[i] = '\0';

		NormalizePhoneNumber(sms_number);

    /* Compare with authorized number */
    if (strcmp(sms_number, phone_read) == 0)
        return 1;

    return 0;
}


/* ---------------------------------------------------
   GSM INITIALIZATION ERROR HANDLER
--------------------------------------------------- */
void GSM_InitError(void)
{
    lcd_clear();
    lcd_cmd(0x80);
    lcd_print("INIT ERROR");
    delay_ms(2000);
		IO0CLR=(1<<22);
}


/* ---------------------------------------------------
   READ SMS FUNCTION
--------------------------------------------------- */
void GSM_ReadSMS(int index)
{
    char cmd[20];

    rx_ready = 0;
    gsm_resp = GSM_NONE;

    lcd_clear();
    lcd_cmd(0x80);
    lcd_print("READ SMS");
    delay_ms(500);

    /* Request SMS from GSM memory */
    sprintf(cmd, "AT+CMGR=%d\r\n", index);
    UART0_TxString(cmd);

    /* Wait for header */
    if (!GSM_WaitForResponse(GSM_TIMEOUT_MS))
    {
        GSM_InitError();
        GSM_Init();
        return;
    }

    if (gsm_resp == GSM_MODEM_RESET)
    {
        GSM_InitError();
        GSM_Init();
        return;
    }

    strcpy(sms_header, rx_line);
    rx_ready = 0;

    /* Wait for message body */
    if (!GSM_WaitForResponse(GSM_TIMEOUT_MS))
    {
        GSM_InitError();
        GSM_Init();
        return;
    }

    strcpy(sms_body, rx_line);
    rx_ready = 0;

    /* Check sender authorization */
    if (GSM_IsAuthorizedNumber(sms_header))
    {
        /*lcd_clear();
        lcd_cmd(0x80);
        lcd_print("AUTH SMS");
        delay_ms(500);

        lcd_clear();
        lcd_cmd(0x80);
        lcd_print("FROM:");
        lcd_print(sms_number);
        delay_ms(2000);

        lcd_clear();
        lcd_cmd(0x80);
        lcd_print("MSG:");
        lcd_cmd(0xC0);
        lcd_print(sms_body);
        delay_ms(3000);*/
				
				ProcessSMSCommand(sms_body);
			
				lcd_clear();
        lcd_cmd(0x80);
        lcd_print("FROM:");
        lcd_print(sms_number);
        delay_ms(2000);

        /*lcd_clear();
        lcd_cmd(0x80);
        lcd_print("MSG:");
        lcd_cmd(0xC0);
        lcd_print(sms_body);
        delay_ms(3000);*/
				
				

        /* Simple command-response 
        if(strcmp(sms_body,"HI")==0)
            GSM_SendSMS("9112449698","BYE");*/
    }
    else
    {
        lcd_clear();
        lcd_cmd(0x80);
        lcd_print("UNAUTH SMS");
        lcd_cmd(0xC0);
        lcd_print(sms_number);
				snprintf(sms_outbox, sizeof(sms_outbox),
									"Security alert: An unauthorized access attempt was detected.\r\n"
									"The request was received from mobile number %s.",
									 sms_number);
				GSM_SendSMS(phone_read, sms_outbox);
        delay_ms(1500);
    }

    /* Wait for OK */
    if (!GSM_WaitForResponse(GSM_TIMEOUT_MS))
    {
        GSM_InitError();
        GSM_Init();
        return;
    }
    rx_ready = 0;

    /* Delete all SMS from memory */
    UART0_TxString("AT+CMGD=1,4\r\n");
    if (!GSM_WaitForResponse(GSM_TIMEOUT_MS))
    {
        GSM_InitError();
        GSM_Init();
        return;
    }

    lcd_clear();
    lcd_cmd(0x80);
    lcd_print("SMS DELETED");
    delay_ms(1500);
}


/* ---------------------------------------------------
   EXTRACT SMS INDEX FROM +CMTI
--------------------------------------------------- */
int GSM_ExtractSMSIndex(char *str)
{
    char *p;

    /* +CMTI: "SM",3 */
    p = strchr(str, ',');
    if (p)
        return atoi(p + 1);

    return -1;
}
int IsValidEndMarker(char *sms)
{
    int len = strlen(sms),i;

    /* Minimum valid length: 0786X$ */
    if (len < 6)
        return 0;

    /* '$' must be the LAST character */
    if (sms[len - 1] != '$')
        return 0;

    /* Ensure '$' appears only once */
    for (i = 0; i < len - 1; i++)
    {
        if (sms[i] == '$')
            return 0;
    }

    return 1;   // VALID
}

void ProcessSMSCommand(char *sms)
{
    char cmd;
    char data[20];
    int temp;

    lcd_clear();
    lcd_print("CMD RXD");
    delay_ms(500);

    /* Validate passkey */
    if (!CheckPasskey(sms))
    {
				snprintf(sms_outbox, sizeof(sms_outbox),
											"The request could not be processed due to an invalid passkey.\r\n"
											"Please verify and try again.");
        GSM_SendSMS(phone_read, sms_outbox);
        return;
    }
    /* ---- END MARKER CHECK ---- */
    if (!IsValidEndMarker(sms))
    {
				snprintf(sms_outbox, sizeof(sms_outbox),
											"The request could not be processed due to an invalid message format.\r\n"
											"Please ensure the command ends with the '$' symbol.");
        GSM_SendSMS(phone_read, sms_outbox);
        return;
    }

    cmd = GetCommandType(sms);
    GetCommandData(sms, data);

    switch (cmd)
    {
        case 'T':   // Temperature set point
            temp = atoi(data);
            UpdateTemperatureSetpoint(temp);
						snprintf(sms_outbox, sizeof(sms_outbox),
											"Your request has been processed successfully.\r\n"
											"The temperature set point has been updated to %d degree Celsius.", temp);
            GSM_SendSMS(phone_read, sms_outbox);
            break;

        case 'M':   // Mobile number update
            UpdateMobileNumber(data);
						snprintf(sms_outbox, sizeof(sms_outbox),
										"Your request is successful.\r\n"
										"The alert mobile number has been updated.Message recived from %s and updated to %s",phone_read,data);
            GSM_SendSMS(phone_read, sms_outbox);
            break;

        case 'I':   // Sensor info
            SendSensorInfo();
            break;
				
				case 'S': 	//SET POINT INFO
						SendSetpointInfo();
						break;
				
        default:
						snprintf(sms_outbox, sizeof(sms_outbox),
											"You have sent an invalid command.\r\n"
											"Please check it once and try again.");
            GSM_SendSMS(phone_read, sms_outbox);
            break;
    }
}

