📡 GSM-Based Temperature Monitoring and Alert System Using LPC2148
📌 Project Overview

The GSM-Based Temperature Monitoring and Alert System Using LPC2148 is an embedded system designed to continuously monitor ambient temperature and provide real-time alerts through GSM communication when the temperature exceeds a predefined threshold. The system integrates sensing, processing, storage, and wireless communication to ensure reliable remote monitoring and control.

The LPC2148 ARM7 microcontroller acts as the central control unit, interfacing with a temperature sensor, GSM module, EEPROM, LCD, keypad, and buzzer to deliver a complete monitoring solution suitable for industrial and commercial environments.

🎯 Objectives

To continuously monitor temperature using an analog sensor

To convert analog temperature values into digital form using ADC

To compare real-time temperature with a predefined set point

To send SMS alerts via GSM when abnormal conditions occur

To store configuration parameters in non-volatile memory

To provide local user interaction through LCD and keypad

To ensure secure remote configuration via SMS commands

🧠 System Architecture

The system follows a modular and interrupt-driven architecture, where each hardware peripheral is managed by a dedicated software module. This improves reliability, scalability, and ease of maintenance.

🔧 Hardware Requirements

Microcontroller: NXP LPC2148 (ARM7TDMI-S)

Temperature Sensor: LM35

GSM Module: SIM800 / SIM900

Display: 16×2 LCD

Memory: External EEPROM (24xx series)

Input: 4×4 Matrix Keypad

Alert Device: Buzzer

Clock Source: 12 MHz Crystal Oscillator

Power Supply: 5V regulated

💻 Software Requirements

Programming Language: Embedded C (C90 standard)

IDE: Keil µVision

Compiler: ARMCC

Debugger: ULINK / Simulator

Target Board: LPC2148 Development Board

🧩 Software Modules

main.c – System initialization and control logic

adc.c – Temperature acquisition and ADC processing

lcd.c – LCD interfacing and display routines

uart.c – UART communication driver

gsm.c – GSM AT command handling and SMS operations

i2c.c – I2C protocol implementation

eeprom.c – Non-volatile memory read/write functions

rtc.c – Real-time clock management

keypad.c – Keypad scanning and input decoding

EINT0_ISR.c – External interrupt handling

delay.c – Timing and delay routines

⚙️ Working Principle

The LM35 temperature sensor produces an analog voltage proportional to temperature. This voltage is converted into a digital value using the LPC2148’s 10-bit ADC. The processed temperature is displayed on an LCD and continuously compared with a stored threshold value.

If the temperature exceeds the set point:

A buzzer is activated

An SMS alert is sent to the authorized mobile number

Repeated alerts are avoided using a lock mechanism

Users can configure parameters such as temperature set point, mobile number, and RTC time using either a keypad-based local menu or secure SMS commands.

🔐 Security Features

Authorized mobile number verification

Passkey-based SMS command validation

Unauthorized SMS ignored

Incoming calls automatically rejected

✅ Key Features

Real-time temperature monitoring

GSM-based SMS alerts

EEPROM-based non-volatile storage

Interrupt-driven design

Secure remote access

Scalable modular firmware

🧪 Testing

The system has been tested for:

ADC accuracy and stability

GSM SMS send/receive reliability

EEPROM data retention after power loss

RTC time persistence

Stress conditions like continuous temperature variation

🏭 Applications

Industrial process monitoring

Cold storage temperature supervision

Telecom equipment room monitoring

Pharmaceutical and medical storage systems

🚀 Future Enhancements

Multi-sensor support

IoT/cloud integration

Mobile application interface

Data logging and analytics

Watchdog-based fault recovery

👨‍💻 Author

Project Title: GSM-Based Temperature Monitoring and Alert System Using LPC2148
Domain: Embedded Systems
Controller: ARM7 LPC2148
