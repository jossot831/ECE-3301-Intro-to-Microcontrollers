#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>
#include "Utils.h"

#include "I2C.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "Fan_Support.h"
#include "Main.h"
#include "ST7735_TFT.h"

#pragma config OSC = INTIO67
#pragma config BOREN =OFF
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config CCP2MX = PORTBE

void Initialize_Screen();
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;

char found;
char tempSecond = 0xff; 
signed int DS1621_tempC, DS1621_tempF;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;
unsigned char setup_fan_temp = 75;
unsigned char Nec_state = 0;
short nec_ok = 0;
char Nec_code1;
char FAN;
char duty_cycle;
int rps;
int rpm;
int ALARMEN;

char buffer[33]     = " ECE3301L Fall'21 L12\0";
char *nbr;
char *txt;
char tempC[]        = "+25";
char tempF[]        = "+77";
char time[]         = "00:00:00";
char date[]         = "00/00/00";
char alarm_time[]   = "00:00:00";
char Alarm_SW_Txt[] = "OFF";
char Fan_SW_Txt[]   = "OFF";                // text storage for Heater Mode

char array1[21]={0xa2,0x62,0xe2,0x22,0x02,0xc2,0xe0,0xa8,0x90,0x68,0x98,0xb0,0x30,0x18,0x7a,0x10,0x38,0x5a,0x42,0x4a,0x52}; // controller code

     
char Display_D1[11] = {0, 0x02, 0x04, 0x06, 0x10, 0x12, 0x14, 0x16, 0x16, 0x16, 0x16}; // array for D1
char Display_D2[4] = {0, 0x40, 0xC0, 0x80};                                            // array for D2
    
char DC_Txt[]       = "000";                // text storage for Duty Cycle value
char RTC_ALARM_Txt[]= "0";                      //
char RPM_Txt[]      = "0000";               // text storage for RPM

char setup_time[]       = "00:00:00";
char setup_date[]       = "01/01/00";
char setup_alarm_time[] = "00:00:00"; 
char setup_fan_text[]   = "075F";
char MASKA;
char MASKD;

#define part1 0
#define part2 0
#define part3 1

void putch (char c)
{   
    while (!TRMT);       
    TXREG = c;
}

void init_UART()
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF & USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
    OSCCON = 0x70;
}

void Do_Init()                      // Initialize the ports 
{ 
    init_UART();                    // Initialize the uart
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1=0x0F;
    TRISA = 0x00;
    TRISB = 0x01;
    TRISC = 0x01;
    TRISD = 0x00;
    TRISE = 0x00;

    RBPU=0;
    TMR3L = 0x00;                   
    T3CON = 0x03;
    I2C_Init(100000); 

    DS1621_Init();
    init_INTERRUPT();
    FAN = 0;
}

void main() 
{
    Do_Init();                                                  // Initialization  
    Initialize_Screen();  

#if (part1 == 1)                                                // Start of part 1
    FAN_EN = 1;                                                                 // emables fan
    FAN_PWM = 1;                                                                // enables PWM
    
    
    duty_cycle = 100;                                                           // duty cycle set 100%
    while (1)
    {
        DS3231_Read_Time();                                                     // function for time

        if(tempSecond != second)
        {   
            rpm = get_RPM();                                                    // gets rpm value from function
            tempSecond = second;                                        
            DS1621_tempC = DS1621_Read_Temp();                                  // calculates degrees in celcius    
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;                         // calculates degree in farenheit
            
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year); // prints info on tera
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
        }
    }
#endif                                                                                 // end of part 1
    
    
#if (part2 == 1)                                                                // Start of part 2
    FAN_EN = 1;
    duty_cycle = 0;                            //  modifying the value and rerun the program
                                                // observe TeraTerm to see fan runs faster or slower
    do_update_pwm(duty_cycle);                  // modifies registers PR2, T2CON, CCP1CON and CCPR1L 
                                                // based off duty cycle
    while(1)
        {
        DS3231_Read_Time();                                                     // function for time

            if(tempSecond != second)
            {
                tempSecond = second;
                DS1621_tempC = DS1621_Read_Temp();                              // calculates temp in celcius
                DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;                     // calculates temp in farenheit
                rpm = get_RPM();                                                // gets rpm value
                printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year); // prints info
                printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
                printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
                char U = (int)(duty_cycle/10);                                      // for array
                Set_DC_RGB(U);                                                      // rgb light D1
                Set_RPM_RGB(rpm);                                               // RGB light for D2
            }
        }
#endif                                                                          // end of part 2  
    
#if (part3 == 1)
    FAN_EN = 0;
    FAN_LED = 0;
    duty_cycle = 50;
    do_update_pwm(duty_cycle);
    while (1)
    {

        DS3231_Read_Time();

        if(tempSecond != second)
        {
            tempSecond = second;
            DS1621_tempC = DS1621_Read_Temp();                                  // calculates temp in celcius
            DS1621_tempF = (DS1621_tempC * 9 / 5) + 32;                         // calculates temp in farenheit
            rpm = get_RPM();                                                    // // get rpm value
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temp = %d C = %d F ", DS1621_tempC, DS1621_tempF);
            printf ("RPM = %d  dc = %d\r\n", rpm, duty_cycle);
            char U = (int)(duty_cycle/10);                                      // for array
            Set_DC_RGB(U);                                                      // rgb light D1
            Set_RPM_RGB(rpm);                                                   // RGB light D2
            Update_Screen();
        }
        
       if (nec_ok == 1)
        {
            nec_ok = 0;
            printf ("NEC_Code = %x\r\n", Nec_code1);       // make sure to use Nec_code1

            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge

            found = 0xff;
            for (int j=0; j< 21; j++)
            {
                if (Nec_code1 == array1[j]) 
                {
                    found = j;
                    j = 21;
                }
            }
            
            if (found == 0xff) 
            {
                printf ("Cannot find button \r\n");
            }
            else
            {
                Do_Beep();
                printf ("button = %d \r\n", found);
                if (found == 5) 
                {
                    Toggle_Fan();
                    printf (" Toggle Fan triggered\r\n");                            // prints toggle fan started
                
                }
                if (found == 6) 
                {
                    Decrease_Speed();
                    printf (" Decreased triggered\r\n");                            // prints decrease speed started
                }
                if (found == 7) 
                {
                    Increase_Speed();
                    printf (" Increased triggered\r\n");                            // prints increase speed started
                }if (found == 8)
                {                    
                    DS3231_Setup_Time();
                    printf("\nTime has been reset\r\n\n");
                }
                else do_update_pwm(duty_cycle);
            }            
        }
    }
#endif
}


