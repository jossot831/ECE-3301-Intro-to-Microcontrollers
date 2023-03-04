#include <p18f4620.h>
#include <stdio.h>
#include <math.h>
#include <usart.h>

#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF

#define D3_RED PORTAbits.RA4
#define D3_GREEN PORTAbits.RA5

#define D2_RED PORTBbits.RB0
#define D2_GREEN PORTBbits.RB1
#define D2_BLUE PORTBbits.RB2
#define D1_RED PORTBbits.RB3
#define D1_GREEN PORTBbits.RB4
#define D1_BLUE PORTBbits.RB5

char array[10]={0x40,0x79,0x24,0x30,0x19,0x12,0x2,0x78,0x00,0x10};          // 7 seg numbers

float Tc;                                                                   // float # for celcius
float Tf;                                                                   // float # for farenheit
int tempT;                                                                  // int temperature
                                                                            // Start of Prototyping Area
void putch(char);
void init_UART();
void Init_ADC(void);
unsigned int Get_Full_ADC(void);
void select_ch(char channel);
void DO_DISPLAY_7SEG_UPPER(char);
void DO_DISPLAY_7SEG_LOWER(char);
void DO_DISPLAY_D1(int);
void DO_DISPLAY_D2(int);
void DO_DISPLAY_D3(int);
void WAIT_1_SEC();

void SET_D1_OFF();
void SET_D1_RED();
void SET_D1_GREEN();
void SET_D1_BLUE();
void SET_D1_WHITE();

void SET_D2_OFF();
void SET_D2_RED();
void SET_D2_GREEN();
void SET_D2_YELLOW();
void SET_D2_BLUE();
void SET_D2_PURPLE();
void SET_D2_CYAN();
void SET_D2_WHITE();

void SET_D3_RED();
void SET_D3_GREEN();
void SET_D3_YELLOW();                                                       // End of prototyping area



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

void main()
{
// Place the rest of your code here
    init_UART();                                            // REQUIRED in handout
    Init_ADC();                                             // initialize A2D converter
    TRISA = 0x0F;                                           // set port A as input
    TRISB = 0x00;                                           // set port B as input
    TRISC = 0x00;                                           // set port C as output
    TRISD = 0x00;                                           // set port D as output
    TRISE = 0x00;                                           // set port E as output
    PORTC = 0xFF;                                           // Cleans up 7 seg display when program finished
    PORTD = 0xFF;                                           // Cleans up 7 seg display when program finished
    PORTB = 0x00;                                           // Cleans up 7 seg display when program finished
    
//    while(1)                                              // infinite loop numbers on display
//    {
//        for (int i=0; i<10; i++)
//        {
//            DO_DISPLAY_7SEG_UPPER(i);
//            DO_DISPLAY_7SEG_LOWER(i);
//            WAIT_1_SEC();
//        }
//    }
    while(1)
    {
        select_ch(0);
        int num_step = Get_Full_ADC();                       // grabs values from Get_Full_ADC and store in num_step
        float voltage_mv = num_step * 4.0;                  // equation to find voltage in milivolts
        float temperature_C = (1035.0 - voltage_mv) / 5.5;  // equation to find temp in C
        float temperature_F = 1.80 * temperature_C + 32.0;  // equation to find temp in F
        int tempF = (int) temperature_F;                    // gets temperature_F and store in tempF as int
        char U = tempF / 10;                                // upper digit value
        char L = tempF % 10;                                // lower digit value
        DO_DISPLAY_7SEG_UPPER(U);                           // displays lower digit value
        DO_DISPLAY_7SEG_LOWER(L);                           // displays upper digit value
        
        printf ("Steps = %d \r\n", num_step);               // displays steps in teraterm
        printf ("Voltage = %f (mV) \r\n", voltage_mv);      // displays voltage in mV in teraterm
        printf ("Temperature = %d F \r\n", tempF);          // displays temp in F in teraterm
        DO_DISPLAY_D2(tempF);                               // DISPLAYS tempF in D1
        DO_DISPLAY_D1(tempF);                               // Displays tempF in D2
        
        select_ch(1);
        int next_num_step = Get_Full_ADC();                 // stores integer from Get_full_ADC and store it 
        int next_voltage_mv = next_num_step * 4;            // converts to voltage mili volts
        printf("Light Voltage = %d (mV)\r\n\n", next_voltage_mv);   // displays light voltage in mV in teraterm
        DO_DISPLAY_D3(next_voltage_mv);                     // Displays voltage in D3
        
        WAIT_1_SEC();       // delay 1 sec
        
    }
} 
void WAIT_1_SEC()                                         // Delays for 1 sec
{
    for(int j=0;j<17000;j++);
}
void select_ch(char channel)                              // selects between channels  
{
    ADCON0 = channel * 4 + 1;
}

void Init_ADC(void)
{
    ADCON0 = 0x01;                                           // setting for ADCON0
    ADCON1 = 0x1B;                                          //  setting for ADCON1
    ADCON2 = 0xA9;                                          // setting for ADCON2
}

unsigned int Get_Full_ADC(void)
{
int result;
    ADCON0bits.GO=1;                                        // Start conversion
    while(ADCON0bits.DONE==1);                              // waiting for conversion
    result = (ADRESH * 0x100) + ADRESL;                     // combine result of upper byte with lower
    return result;                                          // return the MSB value
}
void DO_DISPLAY_D1(int T)
{
    if(T < 45) SET_D1_OFF();                                // if temp less than 45, D1=OFF
    else if (T >=46 && T < 56) SET_D1_RED();                // if temp greater than 46 but less than 56, D1=RED
    else if (T >=56 && T < 66) SET_D1_GREEN();              // if temp greater than 56 but less than 66, D1=GREEN
    else if (T >=66 && T < 76) SET_D1_BLUE();               // if temp greater than 66 but less than 76, D1=BLUE
    else if (T>=76)  SET_D1_WHITE();                        // if temp greater than 76, D1=WHITE
}
void DO_DISPLAY_D2(int T)
{
//    if (T >= 70) SET_D2_WHITE();
//    else if (T >=60 && T < 69) SET_D2_CYAN();             // if temp greater than 60 but less than 69, D2=CYAN
//    else if (T >=50 && T < 59) SET_D2_PURPLE();           // if temp greater than 50 but less than 59, D2=PURPLE
//    else if (T >=40 && T < 49) SET_D2_BLUE();             // if temp greater than 40 but less than 49, D2=BLUE
//    else if (T >=30 && T < 39) SET_D2_YELLOW();           // if temp greater than 30 but less than 39, D2=YELLOW
//    else if (T >=20 && T < 29) SET_D2_GREEN();            // if temp greater than 20 but less than 29, D2=GREEN
//    else if (T >=10 && T < 19) SET_D2_RED();              // if temp greater than 10 but less than 19, D2=RED
//    else if (T<10)SET_D2_OFF();                           // if temp  less than 10, D2=OFF
    if (T >= 70) SET_D2_WHITE();                            // if temp greater than 70, D2=WHITE
    else PORTB = T/10;                                      // temp divided by 10 represents color which outputs to PORTB
}                                                           // e.g temp = 55 / 10 = 5 purple
void DO_DISPLAY_D3(int mv)
{
    if (mv < 2500)
    {
        SET_D3_RED();                                       // if voltage less than 2.5V D3=RED
    }
    else if (mv >= 2500 && mv < 3400)
    {
        SET_D3_GREEN();                                     // if voltage is less than 3.4v but greater than 2.5V, D3=GREEN
    }
    else if (mv >= 3400)
    { 
        SET_D3_YELLOW();                                    // if voltage is greater than 3.4, D3=YELLOW
    }
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
void SET_D1_OFF()                               // Area of D1 D2 D3 color combinations
{
D1_RED = 0;
D1_GREEN = 0;
D1_BLUE = 0;
} // SETS D1 OFF
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
void SET_D1_BLUE()
{
D1_RED = 0;
D1_GREEN = 0;
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
D2_BLUE = 0;
}
void SET_D2_RED()
{
D2_RED = 1;
D2_GREEN = 0;
D2_BLUE = 0;
}
void SET_D2_GREEN()
{
D2_RED = 0;
D2_GREEN = 1;
D2_BLUE = 0;
}
void SET_D2_YELLOW()
{
D2_RED = 1;
D2_GREEN = 1;
D2_BLUE = 0;
}
void SET_D2_BLUE()
{
D2_RED = 0;
D2_GREEN = 0;
D2_BLUE = 1;
}
void SET_D2_PURPLE()
{
D2_RED = 1;
D2_GREEN = 0;
D2_BLUE = 1;
}
void SET_D2_CYAN()
{
D2_RED = 0;
D2_GREEN = 1;
D2_BLUE = 1;
}
void SET_D2_WHITE()
{
D2_RED = 1;
D2_GREEN = 1;
D2_BLUE = 1;
}

void SET_D3_RED()
{
D3_RED = 1;
D3_GREEN = 0;

}
void SET_D3_GREEN()
{
D3_RED = 0;
D3_GREEN = 1;

}
void SET_D3_YELLOW()
{
D3_RED = 1;
D3_GREEN = 1;

}                                                                   // Ending of Area of D1 D2 D3 color combinations