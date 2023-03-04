
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

char array[8]={0xC3, 0xE1, 0x47, 0x62, 0x86, 0x20, 0x04, 0xA5}; //RGB Lights in Hexadecimal
void Delay_One_Sec()           // Causes delay between switching colors
{
 for(int I=0; I <17000; I++);
 } 

void main()
{
char in;                        //  Use variable ?in? as char
    TRISA = 0xff;               //  0xff makes port A as inputs
    TRISB = 0x00;               //  0x00 makes port B as outputs
    TRISC = 0x00;               //  0x00 makes port C as outputs
    ADCON1 = 0x0f;              //  sets pins from Port A & B in digital
    PORTB = 0x00;               //  Removes values from previous parts
 
    TRISD = 0;                  //  makes port D as inputs
 while (1)
{
     for (char i=0; i<8; i++)
     {
         PORTC = i;             //  Same as in part 3. 
         PORTD = array[i];      //  Calls in array for RGB Leds in D2 and D3 
         Delay_One_Sec();       //  Delays to differentiate colors
     }
     
 }
}



