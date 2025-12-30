#ifndef EEPROM_H
#define EEPROM_H
/*-----------------------------------------------------------
  EEPROM + I2C Interface Header (LPC2148)
-----------------------------------------------------------*/

#include <lpc214x.h>

/* ---------------- CONFIGURATION ---------------- */

/* I2C timeout (ms) */
#define I2C_TIMEOUT_MS    100

/* Error indication LED */
#define ERROR_LED         (1<<21)     // P0.21 (active HIGH)

/* EEPROM slave addresses (24xx series) */
#define EEPROM_ADDR_W     0xA0
#define EEPROM_ADDR_R     0xA1



/* External GSM number buffer */
extern char phone_read[15];

/* ---------------- I2C APIs ---------------- */
void I2C_Init(void);

/* (Optional) expose these only if used elsewhere */
int  I2C_Start(void);
void I2C_Stop(void);
int  I2C_Write(unsigned char data);
int  I2C_Read(unsigned char *data);

/* ---------------- EEPROM APIs ---------------- */
int  EEPROM_WriteByte(unsigned int mem_addr, unsigned char data);
int  EEPROM_ReadByte(unsigned int mem_addr, unsigned char *data);

void EEPROM_WriteString(unsigned int addr, char *str);
void EEPROM_ReadString(unsigned int addr, char *buffer);

void EEPROM_WriteInt(unsigned int addr, int value);
int  EEPROM_ReadInt(unsigned int addr, int *value);

/* ---------------- APPLICATION HELPERS ---------------- */
void UpdateTemperatureSetpoint(int temp);
void UpdateMobileNumber(char *num);

#endif
/*-----------------------------------------------------------
  End of EEPROM_H
-----------------------------------------------------------*/
