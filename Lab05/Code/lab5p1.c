#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>

#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF
//#pragma config CCP2MX = PORTBE

#define D1_RED PORTBbits.RB0
#define D1_GREEN PORTBbits.RB1
#define D1_BLUE PORTBbits.RB2
#define D2_RED PORTEbits.RE1
#define D2_GREEN PORTEbits.RE2

#define DP PORTDbits.RD7
#define E0 PORTEbits.RE1

char array[10]={0x40,0x79,0x24,0x30,0x19,0x12,0x2,0x78,0x00,0x10};          // 7 seg numbers



void Activate_Buzzer();                                     // Beginning prototyping
void Deactivate_Buzzer();
void init_UART();
void putch (char);
unsigned int Get_Full_ADC(void);
void Init_ADC(void);
void DO_DISPLAY_7SEG_UPPER(char);
void DO_DISPLAY_7SEG_LOWER(char);
void DO_DISPLAY_D1(int T);
void DO_DISPLAY_D2(int T);
void select_ch(char);
void WAIT_1_SEC();                                         // end of prototyping


void main()
{
    init_UART();                                            // REQUIRED in handout
    Init_ADC();                                             // initialize A2D converter
    TRISA = 0xFF;                                           // set port A as input
    TRISB = 0x00;                                           // set port B as input
    TRISC = 0x00;                                           // set port C as output
    TRISD = 0x00;                                           // set port D as output
    TRISE = 0x00;                                           // set port E as output
//    PORTC = 0xFF;                                         // Cleans up 7 seg display when program finished
//    PORTD = 0xFF;                                         // Cleans up 7 seg display when program finished
//    PORTB = 0x00;                                         // Cleans up 7 seg display when program finished

    while(1)
    {
        select_ch(0);                                      // select channel 0 
        int num_step = Get_Full_ADC();                     // grabs values from Get_Full_ADC and store in num_step
        float volt = (num_step * 4.0)/1000;                // equation to find voltage in milivolts
        
        
        int U = (int) volt;                                 // upper digit value
        int L = (int) ((volt - U) * 10.0);                  // lower digit value
        DO_DISPLAY_7SEG_UPPER(U);                           // displays lower digit value
        DO_DISPLAY_7SEG_LOWER(L);                           // displays upper digit value
        DP=0;                                               // turns on decimal point. led on when low

        printf ("Voltage = %f (V) \r\n", volt);             // displays voltage in V in teraterm

        
        WAIT_1_SEC();       // delay 1 sec
        PORTB = 0x00;
    }
}

void putch (char c)
{
 while (!TRMT);
 TXREG = c; 
 }
void init_UART()
{
 OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
USART_BRGH_HIGH, 25);
 OSCCON = 0x60;
}
void Activate_Buzzer()                                    // turns on buzzer
{
PR2 = 0b11111001 ;
T2CON = 0b00000101 ;
CCPR2L = 0b01001010 ;
CCP2CON = 0b00111100 ;
} 
void Deactivate_Buzzer()                                    // turns off buzzer
{
CCP2CON = 0x0;
 PORTBbits.RB3 = 0;
} 
unsigned int Get_Full_ADC(void)
{
int result;
    ADCON0bits.GO=1;                                        // Start conversion
    while(ADCON0bits.DONE==1);                              // waiting for conversion
    result = (ADRESH * 0x100) + ADRESL;                     // combine result of upper byte with lower
    return result;                                          // return the MSB value
}
void Init_ADC(void)
{
    ADCON0 = 0x01;                                          // setting for ADCON0
    ADCON1 = 0x1A;                                          //  setting for ADCON1 AN0-AN4
    ADCON2 = 0xA9;                                          // setting for ADCON2 Always use
}
void DO_DISPLAY_7SEG_UPPER(char digit)
{
    PORTC = array[digit] & 0x3F;                             //  Masks the lower 6 bit and output to PORTC
    char PE = array[digit] & 0x40;                           // Testing bit 7
    if (PE == 0) PORTE = 0x00;                              // Of bit 7 is equal to 0. output 0 to PORTE
    else PORTE = 0x01;                                      // If bit 7 is equal to 1. output 1 to PORTE
}
void DO_DISPLAY_7SEG_LOWER(char digit)
{
    PORTD = array[digit];                                   // Decodes and outputs based on array 
    
}

void select_ch(char channel)                              // selects between channels  
{
    ADCON0 = channel * 4 + 1;
}
void WAIT_1_SEC()                                         // Delays for 1 sec
{
    for(int j=0;j<17000;j++);
}
