#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>

// Code meant for PART 5
#pragma config OSC = INTIO67
#pragma config WDT=OFF
#pragma config LVP=OFF
#pragma config BOREN =OFF
#pragma config CCP2MX = PORTBE


#define NS_RED PORTAbits.RA1
#define NS_GREEN PORTAbits.RA2

#define NSLT_RED PORTAbits.RA3 
#define NSLT_GREEN PORTAbits.RA4 

#define EW_RED PORTBbits.RB4
#define EW_GREEN PORTBbits.RB5

#define EWLT_RED PORTEbits.RE0
#define EWLT_GREEN PORTEbits.RE2

#define E0 PORTEbits.RE1
#define MODE_LED PORTBbits.RB6
#define SEC_LED PORTDbits.RD7

#define OFF 0
#define RED 1
#define GREEN 2
#define YELLOW 3

#define NSPED_SW PORTAbits.RA5
#define EWPED_SW PORTBbits.RB0
#define NSLT_SW PORTBbits.RB1
#define EWLT_SW PORTBbits.RB2


char array[10]={0x40,0x79,0x24,0x30,0x19,0x12,0x2,0x78,0x00,0x10};          // 7 seg numbers

void            init_UART();                                                    // Start prototyping
void            putch (char);
void            Init_ADC(void);                
unsigned int    Get_Full_ADC(void);         
void            Display_Lower_Digit(char);
void            Display_Upper_Digit(char);
void            Display_Lower_Off(char digit);
void            Display_Upper_Off(char digit);
void            Select_ADC_Channel(char);
void            Activate_Buzzer(void);
void            Deactivate_Buzzer(void);
void            Wait_One_Second(void);
void            Wait_Half_Second(void);
void            Wait_N_Seconds(char);
void            Set_NS(char);
void            Set_NSLT(char);
void            Set_EW(char);
void            Set_EWLT(char);                                                 // end of prototyping            


void init_UART()                                                                // Needed to run TeraTerm
{
    OpenUSART (USART_TX_INT_OFF & USART_RX_INT_OFF &
    USART_ASYNCH_MODE & USART_EIGHT_BIT & USART_CONT_RX &
    USART_BRGH_HIGH, 25);
    OSCCON = 0x60;
}

void putch (char c)                                                             // Used for Teraterm
{
    while (!TRMT);
    TXREG = c;
}
void Display_Lower_Off(char digit)                                              // turns off lower display
{
    PORTD = 0xFF;
}
void Display_Upper_Off(char digit)                                              // turns off upper display
{
    PORTC = 0xFF;
    E0 = 1;
}
void Display_Lower_Digit(char digit)                                            // Displays upper value of voltage
{                                                                               // on PORTD using bits 0 through 6 
    PORTD = array[digit];                                                       // for segments a through g, respectively                
}                                           

void Display_Upper_Digit(char digit)                                            // Displays lower value of voltage.
{                                                                               // PORTC bits 0 through 5 display   
    PORTC = array[digit] & 0x3f;                                                // segments a through f, respectively.
    if (array[digit] > 0x3F)                                                    // needed to use segment g
        E0 = 1;                                                                 // turn off bit 0 from PORTE if g must be off
    else
        E0 = 0;                                                                 // turn on bit 0 from PORTE if g must be on
}

void Select_ADC_Channel(char channel)                                           // specifies channel to be used
{
    ADCON0 = channel * 4 + 1;                                                   // shifts bit by 2 to the left
} 
void Init_ADC(void)                                                             // initialize the A2D converter 
{
    ADCON1=0x0E;                                                             // selects pins AN0 through AN5 as analog signals,
                                                                                // selects VREF+ and VREF- as reference voltages
    ADCON2=0xA9;                                                                // right justify the result
                                                                                // Set the bit conversion time (TAD) and acquisition time
}

unsigned int Get_Full_ADC(void)             // finds the number of steps
{
int result;
    ADCON0bits.GO=1;                                                            // Start Conversion
    while(ADCON0bits.DONE==1);                                                  // Wait for conversion to be completed (DONE=0)
    result = (ADRESH * 0x100) + ADRESL;                                         // Combine result of upper byte and lower byte into
    return result;                                                              // return the most significant 8- bits of the result.
}

void Activate_Buzzer()                                                          // turns on buzzer
{
    PR2 = 0b11111001;
    T2CON = 0b00000101;
    CCPR2L = 0b01001010;
    CCP2CON = 0b00111100;
}

void Deactivate_Buzzer()                                                        // turns off buzzer
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0;
}

void Wait_One_Second()
{
    SEC_LED = 0;                                                                // First, turn off the SEC LED
    Wait_Half_Second();                                                          // Wait for half second (or 500 msec)
    SEC_LED = 1;                                                                 // then turn on the SEC LED
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
}

void Wait_Half_Second()
{
    T0CON = 0x02;                                                               // Timer 0, 16-bit mode, prescaler 1:8
    TMR0L = 0xDB;                                                               // set the lower byte of TMR
    TMR0H = 0x0B;                                                               // set the upper byte of TMR
    INTCONbits.TMR0IF = 0;                                                      // clear the Timer 0 flag
    T0CONbits.TMR0ON = 1;                                                       // Turn on the Timer 0
    
    while (INTCONbits.TMR0IF == 0);                                             // wait for the Timer Flag to be 1 for done
    T0CONbits.TMR0ON = 0;                                                       // turn off the Timer 0
}

void Wait_N_Seconds(char seconds)                                               // Causes delay by N, seconds
 {
    char I;
        for (I = 0; I< seconds; I++)
        {
            Wait_One_Second();
        }
}

void Set_NS(char color)
{
    switch (color)
    {
        case OFF: NS_RED =0;NS_GREEN=0;break;                                   // Turns off the NS LED
        case RED: NS_RED =1;NS_GREEN=0;break;                                   // Sets NS LED RED
        case GREEN: NS_RED =0;NS_GREEN=1;break;                                 // sets NS LED GREEN
        case YELLOW: NS_RED =1;NS_GREEN=1;break;                                // sets NS LED YELLOW
    }
}

void Set_NSLT(char color)
{
    switch (color)
    {
        case OFF: NSLT_RED =0;NSLT_GREEN=0;break;                               // Turns off the NSLT LED
        case RED: NSLT_RED =1;NSLT_GREEN=0;break;                               // Sets NSlT LED RED
        case GREEN: NSLT_RED =0;NSLT_GREEN=1;break;                             // sets NSLT LED GREEN
        case YELLOW: NSLT_RED =1;NSLT_GREEN=1;break;                            // sets NSLT LED YELLOW
    }
}

void Set_EW(char color)
{
    switch (color)
    {
        case OFF: EW_RED =0;EW_GREEN=0;break;                                   // Turns off the EW LED
        case RED: EW_RED =1;EW_GREEN=0;break;                                   // Sets EW LED RED
        case GREEN: EW_RED =0;EW_GREEN=1;break;                                 // sets EW LED GREEN
        case YELLOW: EW_RED =1;EW_GREEN=1;break;                                // sets EW LED YELLOW
    }
}

void Set_EWLT(char color)
{
    switch (color)
    {
        case OFF: EWLT_RED =0;EWLT_GREEN=0;break;                               // Turns off the EWLT LED
        case RED: EWLT_RED =1;EWLT_GREEN=0;break;                               // Sets EWLT LED RED
        case GREEN: EWLT_RED =0;EWLT_GREEN=1;break;                             // sets EWLT LED GREEN
        case YELLOW: EWLT_RED =1;EWLT_GREEN=1;break;                            // sets EWLT LED YELLOW
    }
}

void Wait_One_Second_With_Beep()
{
    SEC_LED = 1;                                                                // First, turn on the SEC LED
   Activate_Buzzer();                                                           // Activate the buzzer
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
    SEC_LED = 0;                                                                // then turn off the SEC LED
    Deactivate_Buzzer ();                                                       // Deactivate the buzzer
    Wait_Half_Second();                                                         // Wait for half second (or 500 msec)
} 
void PED_Control(char DIRECTION, char NUM_SEC)                                  // direction,  1== East west. 0 === north south
{   
    for(char i = NUM_SEC - 1; i>0; i--)
    {
        if (DIRECTION == 0)
        {
            Display_Upper_Digit(i);
        }
        else
        {
            Display_Lower_Digit(i); 
        }
        Wait_One_Second_With_Beep();
    }
    PORTD = 0xFF;
    PORTC = 0xFF;
    E0 = 1;
    Wait_One_Second_With_Beep();
}
void Night_Mode()                                                               // sequence of leds for night mode
{
    PORTD = 0xFF;
    PORTC = 0xFF;
    E0 = 1;
    Set_NSLT(RED);                                                              // STEP 1
    Set_EW(RED);
    Set_EWLT(RED);
    Set_NS(GREEN);
    
    Wait_N_Seconds(9);                                                          // STEP 2
    
    Set_NS(YELLOW);                                                             // STEP 3
    Wait_N_Seconds(3);
    
    Set_NS(RED);                                                                // STEP 4
    
    if (EWLT_SW == 1) //switch4                                                 // STEP 5
    {
        Set_EWLT(GREEN);                                                        // STEP 6
        Wait_N_Seconds(7);
        
        Set_EWLT(YELLOW);                                                       // STEP 7
        Wait_N_Seconds(3);
        
        Set_EWLT(RED);                                                          // STEP 8
    }
    Set_EW(GREEN);                                                              // STEP 9
    Wait_N_Seconds(9);
    
    Set_EW(YELLOW);                                                             // STEP 10
    Wait_N_Seconds(3);
    
    Set_EW(RED);                                                                // STEP 11
        
    if (NSLT_SW == 1)// Switch 3                                                // STEP 12
    {
        Set_NSLT(GREEN);                                                        // STEP 13
        Wait_N_Seconds(7);
        
        Set_NSLT(YELLOW);                                                       // STEP 14 
        Wait_N_Seconds(3);
        
        Set_NSLT(RED);                                                          // STEP 15
        
        
    }
    // STEP 16 FINISH
}

void Day_Mode()                                                                 // sequence of LED for day mode
{
    PORTD = 0xFF;
    PORTC = 0xFF;
    E0 = 1;
    Set_NSLT(RED);                                                              // STEP 1
    Set_EW(RED);
    Set_EWLT(RED);
    Set_NS(GREEN);
    
    if(NSPED_SW == 1) //switch 1
    {
        PED_Control(0, 7);
    }
    
    Wait_N_Seconds(9);                                                          // STEP 2
    
    Set_NS(YELLOW);                                                             // STEP 3
    Wait_N_Seconds(3);
    
    Set_NS(RED);                                                                // STEP 4
    
    if ( EWLT_SW==1) // switch 4                                                // STEP 5
    {
        Set_EWLT(GREEN);                                                        // STEP 6
        Wait_N_Seconds(7);
        
        Set_EWLT(YELLOW);                                                       // STEP 7
        Wait_N_Seconds(3);
        
        Set_EWLT(RED);                                                          // STEP 8
    }
    Set_EW(GREEN);                                                              // STEP 9
    if(EWPED_SW == 1); //switch 2
    {
    PED_Control(1, 9);
    }
    
    Set_EW(GREEN);                                                              // STEP 10
    Wait_N_Seconds(8);
    
    Set_EW(YELLOW);                                                             // STEP 11
    Wait_N_Seconds(3);
    
    Set_EW(RED);                                                                // STEP 12
    
    if (NSLT_SW==1) // switch 3                                                 // STEP 13
    {
       Set_NSLT(GREEN);                                                         // STEP 14
        Wait_N_Seconds(7);
        
        Set_NSLT(YELLOW);                                                       // STEP 15
        Wait_N_Seconds(3);
        
        Set_NSLT(RED);                                                          // STEP 16
        
    }
    // STEP 17 FINISH
}
void main()
{
//    Init_IO();
    init_UART();                            // REQUIRED in handout
    Init_ADC();                             // initialize A2D converter
    TRISA = 0xE1;                           // Makes PORTA as input for switches 
                                            // 
    TRISB = 0x07;                           // makes PORTB as input for switches
    TRISC = 0x00;                           // makes PORTC as all outputs
    TRISD = 0x00;                           // makes PORTD as all outputs
    TRISE = 0x00;                           // makes PORTE as all outputs
    
    float volt;
    int MODE;
    int num_step;
    
    while (1)
    {
        Select_ADC_Channel(0);
        num_step = Get_Full_ADC();
        volt = num_step * 5/ 1024;
        MODE = volt < 2.5 ? 1:0;
        printf ("Voltage Photo Resistor = %f (V) \r\n", volt);
        if (MODE)
        {
            MODE_LED = 1;
            Day_Mode();
        }
        else
        {
            MODE_LED = 0;
            Night_Mode();
        }
    }
//    PED_Control(0,8);
//    PED_Control(1,6);
//    while (1)
//    {
//        for (int i=0; i<4;i++)
//        {
//            Set_NS(i);                                      // Set color for North-South direction
//            Set_NSLT(i);                                    // Set color for North-South Left-Turn direction
//            Set_EW(i);                                      // Set color for East-West direction
//            Set_EWLT(i);                                    // Set color for East-West Left-Turn direction
//            Wait_One_Second(1);                               // call Wait-N-Second routine to wait for 1 second
//        }
//    } 
}


