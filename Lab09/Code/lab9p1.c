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

#include "ST7735.h"
#define _XTAL_FREQ  8000000             // Set operation for 8 Mhz


void TIMER1_isr(void);
void INT0_isr(void);
void Initialize_Screen();

unsigned char Nec_state = 0;
unsigned char i,bit_count;
short nec_ok = 0;
unsigned long long Nec_code;
char Nec_code1;
unsigned int Time_Elapsed;

void WAIT_1_SEC_Beep(void);             // begin proto type
void Activate_Buzzer(void);
void Deactivate_Buzzer(void);           // end prototype
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

#define  D1R            0x02            // begin color leds
#define  D1G            0x04
#define  D1B            0x10
#define  D1W            0x16
#define  D1M            0x12

#define  D2R            0x01
#define  D2G            0x02
#define  D2B            0x04
#define  D2W            0x07
#define  D2M            0x05

#define  D3R            0x04
#define  D3G            0x10
#define  D3B            0x20
#define  D3W            0x34
#define  D3M            0x24            // end of color leds

#define KEYPRESS_LED         PORTEbits.RE1   //  KEYPRESS_LED as PORTE bit RE1


char buffer[31];                        // general buffer for display purpose
char *nbr;                              // general pointer used for buffer
char *txt;

char array1[21]={0xa2, 0x62, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, 0x68,        // names displayed on tera
                 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, 0x5a, 0x42, 0x4a, 0x52};
char txt1[21][4] ={"CH-\0", "CH \0", "CH+\0", "PRE\0", "NXT\0", "PLY\0", "VL-\0",   // Names on the controller
                   "VL+\0", "EQ \0", " 0 \0", "100\0", "200\0", " 1 \0", " 2 \0" ,
                   " 3 \0", " 4 \0", " 5 \0" ," 6 \0" ," 7 \0" ," 8 \0" ," 9 \0"};
int color[21]={RD, RD, RD, BU, BU, GR, MA, MA, MA, BK, BK, BK, BK, BK, BK, BK, BK,
               BK, BK, BK, BK};

char d1[21] = {D1R, 0, 0, D1B, 0, 0, D1M, 0, 0, D1W, 0, 0,                      // color on the controller left column
                D1W,0, 0, D1W,0, 0, D1W, 0, 0};
char d2[21] = {0, D2R, 0, 0, D2B, 0, 0, D2M, 0, 0, D2W, 0, 0,                   // color on the controller middle column
                D2W,0, 0, D2W,0, 0, D2W, 0};
char d3[21] = {0, 0, D3R, 0, 0, D3G, 0, 0, D3M, 0, 0, D3W, 0, 0,                // color on the controller right column
                D3W,0, 0, D3W,0, 0, D3W};
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

void interrupt high_priority chkisr()
{
    if (PIR1bits.TMR1IF == 1) TIMER1_isr();
    if (INTCONbits.INT0IF == 1) INT0_isr();
}

void TIMER1_isr(void)
{
    Nec_state = 0;                          // Reset decoding process
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge
    T1CONbits.TMR1ON = 0;                   // Disable T1 Timer
    PIR1bits.TMR1IF = 0;                    // Clear interrupt flag
}

void force_nec_state0()
{
    Nec_state=0;
    T1CONbits.TMR1ON = 0;
}

void INT0_isr(void)
{
    PORTE = 0x00;
    TRISE = 0x00;
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    if (Nec_state != 0)
    {
        Time_Elapsed = (TMR1H << 8) | TMR1L;       // Store Timer1 value
        TMR1H = 0;                          // Reset Timer1
        TMR1L = 0;
    }
    
    switch(Nec_state)
    {
        case 0 :
        {
                                            // Clear Timer 1
            TMR1H = 0;                      // Reset Timer1
            TMR1L = 0;                      //
            PIR1bits.TMR1IF = 0;            //
            T1CON= 0x90;                    // Program Timer1 mode with count = 1usec using System clock running at 8Mhz
            T1CONbits.TMR1ON = 1;           // Enable Timer 1
            bit_count = 0;                  // Force bit count (bit_count) to 0
            Nec_code = 0;                   // Set Nec_code = 0
            Nec_state = 1;                  // Set Nec_State to state 1
            INTCON2bits.INTEDG0 = 1;        // Change Edge interrupt of INT0 to Low to High            
            return;
        }
        
        case 1 :
        {
            if ((Time_Elapsed > 8500) && (Time_Elapsed < 9500))     // Checks if Time_Elapsed value read is between 8500 usec and 9500 usec
                Nec_state = 2;                                      // Set Nec state to state 2
            else
                force_nec_state0();                                 // do force_nec_state0() 
                INTCON2bits.INTEDG0 = 0;                            // Change Edge interrupt of INT0 to High to Low 
            return;
        }
        
        case 2 :                            
        {
            if ((Time_Elapsed > 4000) && (Time_Elapsed < 5000))     // Checks if Time_Elapsed value read is between 4000 usec and 5000 usec
                Nec_state = 3;                                      // Set Nec state to state 3
            else
                force_nec_state0();                                 // do force_nec_state0() 
                INTCON2bits.INTEDG0 = 1;                            // Change Edge interrupt of INT0 to Low to High 
            return;           
        }
        
        case 3 :                            
        {
            if ((Time_Elapsed > 400) && (Time_Elapsed < 700))     // Checks if Time_Elapsed value read is between 400 usec and 700 usec
                Nec_state = 4;                                      // Set Nec state to state 4
            else
                force_nec_state0();                                 // do force_nec_state0() 
                INTCON2bits.INTEDG0 = 0;                            // Change Edge interrupt of INT0 to High to Low 
            return;  
        }
        
        case 4 :                            
        {
            if (Time_Elapsed > 400 && Time_Elapsed < 1800)      // Checks if Time_Elapsed value read is between 400 usec and 1800 usec
            {
                Nec_code = Nec_code << 1;                       // shift left nec_code by 1 bit 
                if (Time_Elapsed > 1000)                        // Check if Time_Elapsed is greater than 1000 usec 
                    Nec_code++;                                 // If true, add 1 to Nec_code 
                                                                //  else do nothing
                bit_count++;                                    // Increment variable bit_count by 1 
                if (bit_count > 31)                             // Checks if bit_count > 31 
                {
                    nec_ok = 1;                                 // set nec_ok flag to 1 
                    INTCONbits.INT0IE = 0;                      // set INT0IE = 0 , Disable external interrupt
                    Nec_state = 0;                              // set Nec_State to 0
                }
                Nec_state = 3;                                  // Set Nec_State to state 3 
            }
            else 
                force_nec_state0();                             // else, do force_nec_state0() 
            INTCON2bits.INTEDG0 = 1;                            // Change Edge interrupt of INT0 to Low to High
            return;
        }
    }
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
void main()
{
    init_UART();
    OSCCON = 0x70;                          // 8 Mhz
    nRBPU = 0;                              // Enable PORTB internal pull up resistor
    TRISA = 0x00;                           // PORTA output
    TRISB = 0x01;                           // PORTB output
    TRISC = 0x00;                           // PORTC as output
    TRISD = 0x00;                           // PORTD as output
   
    ADCON1 = 0x0F;                          // ADCON mode
    Initialize_Screen();
    INTCONbits.INT0IF = 0;                  // Clear external interrupt
    INTCON2bits.INTEDG0 = 0;                // Edge programming for INT0 falling edge H to L
    INTCONbits.INT0IE = 1;                  // Enable external interrupt
    TMR1H = 0;                              // Reset Timer1
    TMR1L = 0;                              //
    PIR1bits.TMR1IF = 0;                    // Clear timer 1 interrupt flag
    PIE1bits.TMR1IE = 1;                    // Enable Timer 1 interrupt
    INTCONbits.PEIE = 1;                    // Enable Peripheral interrupt
    INTCONbits.GIE = 1;                     // Enable global interrupts
    nec_ok = 0;                             // Clear flag
    Nec_code = 0x0;                         // Clear code
    
    while(1)
    {
        if (nec_ok == 1)
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
            //printf ("NEC_Code = %08lx %x\r\n", Nec_code, Nec_code1);
            //printf ("%d \r\n", found);
            if (found != 0xff) 
            {
                fillCircle(Circle_X, Circle_Y, Circle_Size, color[found]); 
                drawCircle(Circle_X, Circle_Y, Circle_Size, ST7735_WHITE);  
                drawtext(Text_X, Text_Y, txt1[found], ST7735_WHITE, ST7735_BLACK,TS_1); 
            PORTA=0x00;
            PORTB=0x00;
            PORTC=0x00;
            PORTA=d1[found];            // outputs code 
            PORTB=d3[found];            // outputs code
            PORTC=d2[found];            // outputs code
            WAIT_1_SEC_Beep();          // Buzzer sound w/ KEYPRESS_LED
            }

        }
    }
}


void Initialize_Screen()                            // display for LCD
{
    LCD_Reset();
    TFT_GreenTab_Initialize();
    fillScreen(ST7735_BLACK);
  
    /* TOP HEADER FIELD */
    txt = buffer;
    strcpy(txt, "ECE3301L Fall 21-S4");  
    drawtext(2, 2, txt, ST7735_WHITE, ST7735_BLACK, TS_1);

    strcpy(txt, "LAB 9 ");  
    drawtext(50, 10, txt, ST7735_WHITE, ST7735_BLACK, TS_1);
}
void WAIT_1_SEC_Beep()                                // wait 1 second 
{
    Activate_Buzzer();
    KEYPRESS_LED = 1;
    for (int j=0; j<17000; j++);
    KEYPRESS_LED = 0;
    Deactivate_Buzzer();
}

void Deactivate_Buzzer()                      // function to deactivate the buzzer
{
    CCP2CON = 0x0;
    PORTBbits.RB3 = 0;
}

void Activate_Buzzer()                        // function to activate the buzzer
{
    PR2 = 0b11111001 ;
    T2CON = 0b00000101 ;
    CCPR2L = 0b01001010 ;
    CCP2CON = 0b00111100 ;
}

#include <p18cxxx.h>

void main (void)

{

    ADCON1 = 0X0F;

    TRISB = 0x24;
    PORTB = 0x00;
    
    INTCON2bits.RBPU = 1;
    RCONbits.IPEN = 1;
      INTCON2bits.INTEDG2 = 1;
      INTCON3bits.INT2IP = 0;
      INTCON3bits.INT2IE = 1;
      INTCONbits.GIEH = 1;
      INTCONbits.GIEL = 1;

while(1)

{

    clr Wdt();
    if ( PROBbits.RB5 ==0)
        PORTBbits.RB4=0;

}

}
