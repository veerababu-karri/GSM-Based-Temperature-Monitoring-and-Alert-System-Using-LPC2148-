#ifndef __GSM_H__
#define __GSM_H__


#define GSM_TIMEOUT_MS      5000   // 5 seconds
#define GSM_SMS_TIMEOUT_MS  10000  // 10 seconds

#define EEPROM_TEMP_ADDR     0x0010   // temperature set point
#define EEPROM_MOB_ADDR      0x0000   // mobile number (string)
#define PASSKEY              0x0020

extern char sms_outbox[140];
extern volatile int sms_pending;
extern volatile int gsm_busy ;

extern char msg_cmd [260];
typedef enum
{
    GSM_NONE = 0,       

    GSM_OK,             

    GSM_ERROR,          

    GSM_PROMPT,         

    GSM_CMGS,           

    GSM_CMTI,           

    GSM_RING,           

    GSM_MODEM_RESET     

} GSM_Response_t;

extern unsigned char temp;
extern volatile int sms_index;
extern char phone_read[15];
extern char pass[5];

void GSM_InitError(void);

int GSM_WaitForResponse(unsigned int timeout_ms);

void GSM_ReadSMS(int index);

void GSM_Init(void);

void GSM_SendSMS(char *number, char *msg);

int GSM_ExtractSMSIndex(char *str);

int CheckPasskey(char *msg);

char GetCommandType(char *msg);

void GetCommandData(char *msg, char *data);

int IsValidEndMarker(char *sms);

void SendSensorInfo(void);

void ProcessSMSCommand(char *sms);

void NormalizePhoneNumber(char *num);

void SendSetpointInfo(void);

#endif

