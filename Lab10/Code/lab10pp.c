#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>
#include <string.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF
#pragma config CCP2MX = PORTBE

#include "I2C.h"
#include "I2C_Support.h"
#include "Interrupt.h"
#include "ST7735.h"



#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text


// colors
#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

   

#define Circle_Size     20              // Size of Circle for Light
#define Circle_X        60              // Location of Circle
#define Circle_Y        80              // Location of Circle
#define Text_X          52
#define Text_Y          77
#define TS_1            1               // Size of Normal Text
#define TS_2            2               // Size of Big Text

#define RD               ST7735_RED
#define BU               ST7735_BLUE
#define GR               ST7735_GREEN
#define MA               ST7735_MAGENTA
#define BK               ST7735_BLACK

#define  D1R            0x02            // begin color leds
#define  D1G            0x04
#define  D1B            0x10
#define  D1W            0x16
#define  D1M            0x12

#define D1_RED PORTAbits.RA1            // pins on the D1 RGB. a1, a2 ,a4
#define D1_GREEN PORTAbits.RA2
#define D1_BLUE PORTAbits.RA4

#define KEYPRESS_LED         PORTEbits.RE1   //  KEYPRESS_LED as PORTE bit RE1

char tempSecond = 0xff; 
char second = 0x00;
char minute = 0x00;
char hour = 0x00;
char dow = 0x00;
char day = 0x00;
char month = 0x00;
char year = 0x00;
char setup_second, setup_minute, setup_hour, setup_day, setup_month, setup_year;
char alarm_second, alarm_minute, alarm_hour, alarm_date;
char setup_alarm_second, setup_alarm_minute, setup_alarm_hour;

char array1[21]={0xa2, 0x62, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, 0x68,        // names displayed on tera
                 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, 0x5a, 0x42, 0x4a, 0x52};
char txt1[21][4] ={"CH-\0", "CH \0", "CH+\0", "PRE\0", "NXT\0", "PLY\0", "VL-\0",   // Names on the controller
                   "VL+\0", "EQ \0", " 0 \0", "100\0", "200\0", " 1 \0", " 2 \0" ,
                   " 3 \0", " 4 \0", " 5 \0" ," 6 \0" ," 7 \0" ," 8 \0" ," 9 \0"};
int color[21]={RD, RD, RD, BU, BU, GR, MA, MA, MA, BK, BK, BK, BK, BK, BK, BK, BK,  // colors on the controller
               BK, BK, BK, BK};
int D1[10] = {0, 0x02, 0x04, 0x06, 0x10, 0x12, 0x14, 0x16, 0x16, 0x16};         // color array

char TempSecond;

char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

short nec_ok = 0;
unsigned long long Nec_code;
char Nec_code1;
char found;
unsigned char Nec_state = 0;

void Activate_Buzzer(void);                             // beginning of prototyping
void Deactivate_Buzzer(void);
void Initialize_Screen(void);
void WAIT_1_SEC_Beep(void);
void Wait_Half_Second(void);
void SET_D1_WHITE();
void SET_D1_OFF();
void SET_D1_RED();
void SET_D1_GREEN();
void SET_D1_YELLOW();
void SET_D1_BLUE();
void SET_D1_PURPLE();
void SET_D1_CYAN();                                     // end of prototyping

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


void Wait_One_Sec()
{                                                   // wait one sec
    for (int j=0; j<17000; j++);
}
void Wait_Half_Second()
{
    T0CON = 0x03;                               // Timer 0, 16-bit mode, prescaler 1:16
    TMR0L = 0xDB;                               // set the lower byte of TMR
    TMR0H = 0x0B;                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                       // Turn on the Timer 0
    while (INTCONbits.TMR0IF == 0);             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                       // turn off the Timer 0
}
void WAIT_1_SEC_Beep()                                // wait 1 second with beep
{
    Activate_Buzzer();
    KEYPRESS_LED = 1;
    for (int j=0; j<17000; j++);
    KEYPRESS_LED = 0;
    Deactivate_Buzzer();
}

void Do_Init()                      // Initialize the ports 
{ 
    DS1621_Init();                  // calling DS1621_Init() from I2C_Support.c
                                    // to initiliaze DS1621 chip
    init_UART();                    // Initialize the uart
    OSCCON=0x70;                    // Set oscillator to 8 MHz 
    
    ADCON1=0x0F;
    
    TRISA = 0x00;                   // TRISx ports
    TRISB = 0x01;
    TRISC = 0x00;
    TRISD = 0x00;
    TRISE = 0x00;

    RBPU=0;
    init_INTERRUPT();    
    I2C_Init(100000); 
    DS1621_Init();

}


void main() 
{ 
    
    Do_Init();                                                  // Initialization  
//    while (1)
//    {
//        char tempC = DS1621_Read_Temp();
//        char tempF = (tempC * 9 / 5) + 32;
//        printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);    // part 1, of lab
//        Wait_One_Sec();                                                           // reads the temperature
//    }
     // DS3231_Setup_Time();         // part 3, presetup  the times
      while (1)
      {

        DS3231_Read_Time();
        if(tempSecond != second)
        {
            tempSecond = second;
            signed char tempC = DS1621_Read_Temp();                             // temperature in celcius
            signed char tempF = (tempC * 9 / 5) + 32;                           // temperature in farenheit
            printf ("%02x:%02x:%02x %02x/%02x/%02x",hour,minute,second,month,day,year);
            printf (" Temperature = %d degreesC = %d degreesF\r\n", tempC, tempF);
            
            int T = (int) tempF;                                                // converting to int
                                                                  // temperatures based off from Lab 4 handout
//            if (T >= 70) SET_D1_WHITE();                          // if temp greater than 70, D1 WHITE
//            else if (T >=60 && T < 69) SET_D1_CYAN();             // if temp greater than 60 but less than 69, D1=CYAN
//            else if (T >=50 && T < 59) SET_D1_PURPLE();           // if temp greater than 50 but less than 59, D1=PURPLE
//            else if (T >=40 && T < 49) SET_D1_BLUE();             // if temp greater than 40 but less than 49, D1=BLUE
//            else if (T >=30 && T < 39) SET_D1_YELLOW();           // if temp greater than 30 but less than 39, D1=YELLOW
//            else if (T >=20 && T < 29) SET_D1_GREEN();            // if temp greater than 20 but less than 29, D1=GREEN
//            else if (T >=10 && T < 19) SET_D1_RED();              // if temp greater than 10 but less than 19, D1=RED
//            else if (T<10)SET_D1_OFF();                           // if temp  less than 10, D1=OFF
            PORTA = D1[(tempF/10)];
        }
    
        if (nec_ok == 1)            // 0x90 is button code for EQ, 8 position
        {
            nec_ok = 0;
            Nec_code1 = (char) ((Nec_code >> 8));
            INTCONbits.INT0IE = 1;          // Enable external interrupt
            INTCON2bits.INTEDG0 = 0;        // Edge programming for INT0 falling edge
            
            char found = 0xff;
            
            // add code here to look for code

            for (char i=0; i<21; i++)           // Loop to find for code
            {
                if(Nec_code1==array1[i])
                {
                    found=i;
                    i=21;
                }
            }                        
            printf ("NEC_Code = %08lx %x\r\n", Nec_code, Nec_code1);            // prints on tera nec_code
            printf ("%d \r\n", found);                                          // print on tera found
            if (found == 8)
            {
          
                DS3231_Setup_Time();
                printf (" Preset time started\r\n");                            // prints preset time started
          
            }

            if (found != 0xff)
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1); 
            
                WAIT_1_SEC_Beep();          // Buzzer sound w/ KEYPRESS_LED
            
            }

        }
      }
            
    
}



void Activate_Buzzer()                    // function to activate the buzzer
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

void Deactivate_Buzzer()                    // function to deactivate the buzzer
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0;
}

void Initialize_Screen()                    // function for LCD screen
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Fall 21-S4");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 10 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}

//void SET_D1_WHITE()                                                             // beginning of D1 colors
//{
//    D1_RED = 1;
//    D1_GREEN = 1;
//    D1_BLUE = 1;
//}
//void SET_D1_OFF()
//{
//    D1_RED = 0;
//    D1_GREEN = 0;
//    D1_BLUE = 0;
//}
//void SET_D1_RED()
//{
//    D1_RED = 1;
//    D1_GREEN = 0;
//    D1_BLUE = 0;
//}
//void SET_D1_GREEN()
//{
//    D1_RED = 0;
//    D1_GREEN = 1;
//    D1_BLUE = 0;
//}
//void SET_D1_YELLOW()
//{
//    D1_RED = 1;
//    D1_GREEN = 1;
//    D1_BLUE = 0;
//}
//void SET_D1_BLUE()
//{
//    D1_RED = 0;
//    D1_GREEN = 0;
//    D1_BLUE = 1;
//}
//void SET_D1_PURPLE()
//{
//    D1_RED = 1;
//    D1_GREEN = 0;
//    D1_BLUE = 1;
//}
//void SET_D1_CYAN()
//{
//    D1_RED = 0;
//    D1_GREEN = 1;
//    D1_BLUE = 1;
//}                                                                               // end of d1 colors