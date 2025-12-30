#include "eeprom.h"
#include "delay.h"
#include "gsm.h"
#include <lpc214x.h>


/* ---------- INTERNAL UTILS ---------- */

static int I2C_WaitSI(unsigned int timeout)
{
    while (timeout--)
    {
        if (I2C0CONSET & (1<<3))
            return 1;
        delay_ms(1);
    }
    return 0;
}

static void I2C_ResetBus(void)
{
    I2C0CONCLR = (1<<6);          // Disable I2C
    delay_ms(2);

    I2C0CONCLR = (1<<5)|(1<<4)|(1<<3)|(1<<2);
    delay_ms(2);

    I2C0CONSET = (1<<6);          // Enable I2C
}

static void I2C_ErrorHandler(void)
{
    IOCLR0 = ERROR_LED;           // LED ON
    GSM_SendSMS(phone_read, "EEPROM DISCONNECTED");

    I2C_ResetBus();
    delay_ms(500);
}

static void I2C_ClearError(void)
{
    IOSET0 = ERROR_LED;           // LED OFF
}

/* ---------- I2C CORE ---------- */

void I2C_Init(void)
{
    PINSEL0 |= (1<<4) | (1<<6);   // P0.2 SCL, P0.3 SDA

    I2C0SCLH = 75;
    I2C0SCLL = 75;

    I2C0CONSET = (1<<6);          // Enable I2C
}

static int I2C_Start(void)
{
    I2C0CONCLR = (1<<3);
    I2C0CONSET = (1<<5);

    if (!I2C_WaitSI(I2C_TIMEOUT_MS))
    {
        I2C_ErrorHandler();
        return 0;
    }

    I2C0CONCLR = (1<<5);
    I2C_ClearError();
    return 1;
}

static void I2C_Stop(void)
{
    I2C0CONSET = (1<<4);
    I2C0CONCLR = (1<<3);
}

static int I2C_Write(unsigned char data)
{
    I2C0DAT = data;
    I2C0CONCLR = (1<<3);

    if (!I2C_WaitSI(I2C_TIMEOUT_MS))
    {
        I2C_ErrorHandler();
        return 0;
    }

    I2C_ClearError();
    return 1;
}

static int I2C_Read(unsigned char *data)
{
    I2C0CONCLR = (1<<2);   // NACK
    I2C0CONCLR = (1<<3);

    if (!I2C_WaitSI(I2C_TIMEOUT_MS))
    {
        I2C_ErrorHandler();
        return 0;
    }

    *data = I2C0DAT;
    I2C_ClearError();
    return 1;
}

/* ---------- EEPROM APIs ---------- */

int EEPROM_WriteByte(unsigned int addr, unsigned char data)
{
    if (!I2C_Start()) return 0;
    if (!I2C_Write(EEPROM_ADDR_W)) return 0;
    if (!I2C_Write(addr >> 8)) return 0;
    if (!I2C_Write(addr & 0xFF)) return 0;
    if (!I2C_Write(data)) return 0;

    I2C_Stop();
    delay_ms(10);
    return 1;
}

int EEPROM_ReadByte(unsigned int addr, unsigned char *data)
{
    if (!I2C_Start()) return 0;
    if (!I2C_Write(EEPROM_ADDR_W)) return 0;
    if (!I2C_Write(addr >> 8)) return 0;
    if (!I2C_Write(addr & 0xFF)) return 0;

    if (!I2C_Start()) return 0;
    if (!I2C_Write(EEPROM_ADDR_R)) return 0;
    if (!I2C_Read(data)) return 0;

    I2C_Stop();
    return 1;
}

void EEPROM_WriteString(unsigned int addr, char *str)
{
    while (*str)
        EEPROM_WriteByte(addr++, *str++);
    EEPROM_WriteByte(addr, '\0');
}

void EEPROM_ReadString(unsigned int addr, char *buf)
{
    unsigned char ch;

    while (1)
    {
        if (!EEPROM_ReadByte(addr++, &ch))
        {
            *buf = '\0';
            return;
        }
        *buf++ = ch;
        if (ch == '\0') return;
    }
}

void EEPROM_WriteInt(unsigned int addr, int value)
{
    EEPROM_WriteByte(addr, (value>>8)&0xFF);
    EEPROM_WriteByte(addr+1, value&0xFF);
}

int EEPROM_ReadInt(unsigned int addr, int *value)
{
    unsigned char msb, lsb;

    if (!EEPROM_ReadByte(addr, &msb)) return 0;
    if (!EEPROM_ReadByte(addr+1, &lsb)) return 0;

    *value = ((int)msb<<8) | lsb;
    return 1;
}

void UpdateMobileNumber(char *num)
{ 
		EEPROM_WriteString(EEPROM_MOB_ADDR, num);
}
void UpdateTemperatureSetpoint(int temp) 
{
		EEPROM_WriteByte(EEPROM_TEMP_ADDR, temp); 
}
