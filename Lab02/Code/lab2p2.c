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
//Code is for 4 LED lights
void main()
{
char in;                // Use variable ?in? as char
    TRISA = 0xff;       //0xff makes port A as inputs
    TRISB = 0x00;       //0x00 makes port B as outputs
    TRISC = 0x00;       //0x00 makes port C as outputs
    ADCON1 = 0x0f;      // sets pins from Port A & B in digital

    PORTB = 0x00;       //Removes values from part 1
 
    while (1)
    {
    in = PORTA;         // read data from PORTA and save it into in
    PORTC = in;         // Output the data to PORTB
    } 
 }

