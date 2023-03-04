#include <stdio.h>

#include <p18f4620.h>
#include "I2C_Support.h"
#include "I2C.h"

#define ACCESS_CFG      0xAC
#define START_CONV      0xEE
#define READ_TEMP       0xAA
#define CONT_CONV       0x02
#define ACK     1
#define NAK     0


extern unsigned char second, minute, hour, dow, day, month, year;
extern unsigned char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
extern unsigned char alarm_second, alarm_minute, alarm_hour, alarm_date;
extern unsigned char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;


void DS1621_Init()
{
    char Device = 0x48;             // I2C address for this DS1621 device      
    I2C_Write_Cmd_Write_Data (Device, ACCESS_CFG, CONT_CONV);
    I2C_Write_Cmd_Only(Device, START_CONV);       
}


int DS1621_Read_Temp()
{
  char Device = 0x48;               // I2C address for this DS1621 device 
  char Cmd = READ_TEMP;             // read_temp command  

  char Data_Ret;    
  I2C_Start();                      // Start I2C protocol
  I2C_Write((Device << 1) | 0);     // Device address
  I2C_Write(Cmd);                   // Send register address
  I2C_ReStart();                    // Restart I2C
  I2C_Write((Device << 1) | 1);     // Initialize data read
  Data_Ret = I2C_Read(NAK);         //
  I2C_Stop(); 
  return Data_Ret;
}

void DS3231_Read_Time()
{
  char Device = 0x68;               // I2C address for this DS3231
  char Address = 0x00;              // value for the register 0x00 pointing to the register 'second'
  char Data_Ret;    
  I2C_Start();                      // Start I2C protocol
  I2C_Write((Device << 1) | 0);     // DS3231 address Write mode
  I2C_Write(Address);               // Send register address
  I2C_ReStart();                    // Restart I2C
  I2C_Write((Device << 1) | 1);     // Initialize data read
  // Data_Ret = I2C_Read(NAK);        
  second = I2C_Read(ACK);           // allows system to read the register'second' from DS3231
  minute = I2C_Read(ACK);           // allows system to read the register'minute' from DS3231
  hour = I2C_Read(ACK);             // allows system to read the register'hour' from DS3231
  dow = I2C_Read(ACK);              // allows system to read the register'dow' from DS3231
  day = I2C_Read(ACK);              // allows system to read the register'day' from DS3231
  month = I2C_Read(ACK);            // allows system to read the register'month' from DS3231
  year = I2C_Read(NAK);             // NAK will terminate read sequence 
  
  I2C_Stop();                       // no void routine so no need for return statement                                  
}

void DS3231_Setup_Time()
{
    
    char Device = 0x68;                                                         // I2C address for the DS3231 device 
    char Address = 0x00;                                                        // value for the register 0x00 pointing to the register 'second'
    
    second = 0x00;                                                              // Set the second value 
    minute = 0x00;                                                              // Set the minute value 
    hour = 0x00;                                                                // Set the hour value 
    dow = 0x00;                                                                 // Set the day of the week
    day = 0x28;                                                                 // Set the day
    month = 0x10;                                                               // Set the month
    year = 0x21;                                                                // Set the year 
        
    I2C_Start();                                                                // Start I2C protocol
    I2C_Write((Device << 1) | 0);                                               // Device address Write mode
    I2C_Write(Address);                                                         // Send register address
    I2C_Write(second);                                                          // Initialize data read for second
    I2C_Write(minute);                                                          // Initialize data read for minute
    I2C_Write(hour);                                                            // Initialize data read for hour
    I2C_Write(dow);                                                             // Initialize data read for dow
    I2C_Write(day);                                                             // Initialize data read for day
    I2C_Write(month);                                                           // Initialize data read for month
    I2C_Write(year);                                                            // Initialize data read for year
    I2C_Stop(); 
}

