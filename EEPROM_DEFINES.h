#ifndef EEPROM_DEFINES_H
#define EEPROM_DEFINES_H
/*-----------------------------------------------------------
 Header Guard:
 Prevents multiple inclusion of this I2C EEPROM
 address definition file.
-----------------------------------------------------------*/

/*-----------------------------------------------------------
 EEPROM_ADDR_W:
 - I2C slave address for EEPROM WRITE operation
 - Binary: 1010 0000
   1010 ? EEPROM device identifier
   000  ? A2, A1, A0 hardware address pins (assumed LOW)
   0    ? R/W bit = 0 (WRITE)
-----------------------------------------------------------*/
#define EEPROM_ADDR_W 0xA0   // 1010 0000

/*-----------------------------------------------------------
 EEPROM_ADDR_R:
 - I2C slave address for EEPROM READ operation
 - Same device address as WRITE
 - R/W bit = 1 (READ)
-----------------------------------------------------------*/
#define EEPROM_ADDR_R 0xA1

#endif
/*-----------------------------------------------------------
 End of I2C_DEFINES_H
-----------------------------------------------------------*/
