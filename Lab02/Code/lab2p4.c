
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <math.h>
#include <p18f4620.h>
#include <usart.h>

#pragma config OSC = INTIO67
#pragma config WDT = OFF
#pragma config LVP = OFF
#pragma config BOREN = OFF

void Delay_One_Sec()       // Causes delay between switching colors
{
 for(int I=0; I <17000; I++);
 } 

void main()
{
char in;                    //  Use variable ?in? as char
    TRISA = 0xff;           //  0xff makes port A as inputs
    TRISB = 0x00;           //  0x00 makes port B as outputs
    TRISC = 0x00;           //  0x00 makes port C as outputs
    ADCON1 = 0x0f;          //  sets pins from Port A & B in digital
    PORTB = 0x00;           //  Removes values from part 1
 
    TRISD = 0;              //  0 makes port D as inputs
 while (1)
{
     for (char i=0; i<8; i++)
     {
         PORTC = i;         // Same as in part 3. 
         PORTD = 7-i;       // Will display 7(White) and end with 0(off)
         Delay_One_Sec();   // Delays to differentiate light colors
     }
     
}
 }


