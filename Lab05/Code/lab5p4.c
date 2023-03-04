
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>

#pragma config CCP2MX = PORTBE
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF


#define D1_RED PORTBbits.RB0
#define D1_GREEN PORTBbits.RB1
#define D1_BLUE PORTBbits.RB2
#define D2_RED PORTEbits.RE1
#define D2_GREEN PORTEbits.RE2

#define DP PORTDbits.RD7
#define E0 PORTEbits.RE1

char array[10]={0x40,0x79,0x24,0x30,0x19,0x12,0x2,0x78,0x00,0x10};          // 7 seg numbers

void SET_D1_OFF();
void SET_D1_RED();
void SET_D1_GREEN();
void SET_D1_YELLOW();
void SET_D1_BLUE();
void SET_D1_PURPLE();
void SET_D1_CYAN();
void SET_D1_WHITE();

void SET_D2_OFF();
void SET_D2_YELLOW();


void Activate_Buzzer();
void Deactivate_Buzzer();
void init_UART();
void putch (char);
unsigned int Get_Full_ADC(void);
void Init_ADC(void);
void DO_DISPLAY_7SEG_UPPER(char);
void DO_DISPLAY_7SEG_LOWER(char);
void DO_DISPLAY_D1(int);
void DO_DISPLAY_D2(int);
void select_ch(char);
void WAIT_1_SEC();


void main()
{
    init_UART();                                            // REQUIRED in handout
    Init_ADC();                                             // initialize A2D converter
    TRISA = 0xFF;                                           // set port A as input
    TRISB = 0x00;                                           // set port B as input
    TRISC = 0x00;                                           // set port C as output
    TRISD = 0x00;                                           // set port D as output
    TRISE = 0x00;                                           // set port E as output


    while(1)
    {
        select_ch(4);
        float RREF = 10000/1000.0;
        float VREF = 4.096;
        int num_step = Get_Full_ADC();                       // grabs values from Get_Full_ADC and store in num_step
        float VL1 = (num_step * 4.0)/1000;                   // equation to find voltage in volts
        float RL = RREF * VL1/(VREF - VL1);
        int U, L;
        if(RL >= 10)
        {
             U = ((int) RL)/10;                                // upper digit value
             L = ((int) RL) % 10;                              // lower digit value     
        }
        else if(RL < 10)
        {
             U = (int) RL;                                // upper digit value
             L = (int) ((RL - U) * 10.0);                 // lower digit value                                                   
        }      
        
        DO_DISPLAY_7SEG_UPPER(U);                        // displays lower digit value
        DO_DISPLAY_7SEG_LOWER(L);                        // displays upper digit value
        if (RL < 10.0)
            DP = 0;                                         // turns on decimal point
        else
            DP = 1;                                         // turns off decimal point
        
        printf ("Resistance = %f (K ohm) \r\n", RL);        // displays voltage in K ohm in teraterm

        char resistor = RL / 10;
        if (resistor>=7)                                                       
            PORTB = 0x07;                             // white color for RL >= 70
        else
            PORTB = resistor;

        if (RL < 0.07)
        {
            SET_D2_YELLOW();                          // make RGB LED D2 yellow
            Activate_Buzzer();                        // turn on buzzer
        }
        else
        {    
            SET_D2_OFF();                             // turn RGB LED D2 off
            Deactivate_Buzzer();                      // turn off buzzer
        }   
       
        WAIT_1_SEC();                                     // delay 1 sec
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
    PORTC = array[digit] & 0x3F;                //  Masks the lower 6 bit and output to PORTC
    char PE = array[digit] & 0x40;              // Testing bit 7
    if (PE == 0) PORTE = 0x00;                  // Of bit 7 is equal to 0. output 0 to PORTE
    else PORTE = 0x01;                          // If bit 7 is equal to 1. output 1 to PORTE
}
void DO_DISPLAY_7SEG_LOWER(char digit)
{
    PORTD = array[digit];                       // Decodes and outputs based on array 
    
}
void DO_DISPLAY_D1(int r)
{
//    if (r >= 70) SET_D1_WHITE();
//    else if (r >=60 && r < 69) SET_D1_CYAN();             // if resistance greater than 60 but less than 69, D1=CYAN
//    else if (r >=50 && r < 59) SET_D1_PURPLE();           // if resistance greater than 50 but less than 59, D1=PURPLE
//    else if (r >=40 && r < 49) SET_D1_BLUE();             // if resistance greater than 40 but less than 49, D1=BLUE
//    else if (r >=30 && r < 39) SET_D1_YELLOW();           // if resistance greater than 30 but less than 39, D1=YELLOW
//    else if (r >=20 && r < 29) SET_D1_GREEN();            // if resistance greater than 20 but less than 29, D1=GREEN
//    else if (r >=10 && r < 19) SET_D1_RED();              // if resistance greater than 10 but less than 19, D1=RED
//    else if (r<10)SET_D1_OFF();                           // if resistance  less than 10, D1=OFF
    if (r >= 70) SET_D1_WHITE();                            // if temp greater than 70, D1=WHITE
    else PORTB = r/10;                                      // temp divided by 10 represents color which outputs to PORTB
} 
void DO_DISPLAY_D2(int res)
{
    if (res < 0.070) 
    {
       
        SET_D2_YELLOW();                                    // if resistance is less than 70, D2=YELLOW and turn on buzzer
        Activate_Buzzer();
    }
    else  
    {
         SET_D2_OFF();                                       // if resistance is greater than or equal to 70, turn off D2 and turn off buzzer
        Deactivate_Buzzer();
    }
}
void select_ch(char channel)                                // selects between channels  
{
    ADCON0 = channel * 4 + 1;
}
void WAIT_1_SEC()                                           // Delays for 1 sec
{   
    for(int j=0;j<17000;j++);
}
void SET_D1_OFF()                                           // start of LED color combos
{
D1_RED = 0;
D1_GREEN = 0;
D1_BLUE = 0;
}
void SET_D1_RED()
{
D1_RED = 1;
D1_GREEN = 0;
D1_BLUE = 0;
}
void SET_D1_GREEN()
{
D1_RED = 0;
D1_GREEN = 1;
D1_BLUE = 0;
}
void SET_D1_YELLOW()
{
D1_RED = 1;
D1_GREEN = 1;
D1_BLUE = 0;
}
void SET_D1_BLUE()
{
D1_RED = 0;
D1_GREEN = 0;
D1_BLUE = 1;
}
void SET_D1_PURPLE()
{
D1_RED = 1;
D1_GREEN = 0;
D1_BLUE = 1;
}
void SET_D1_CYAN()
{
D1_RED = 0;
D1_GREEN = 1;
D1_BLUE = 1;
}
void SET_D1_WHITE()
{
D1_RED = 1;
D1_GREEN = 1;
D1_BLUE = 1;
}

void SET_D2_OFF()
{
D2_RED = 0;
D2_GREEN = 0;

}
void SET_D2_YELLOW()
{
D2_RED = 1;
D2_GREEN = 1;
}                                                               // end of color combos