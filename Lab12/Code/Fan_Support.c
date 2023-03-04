#include <p18f4620.h>
#include "Main.h"
#include "Fan_Support.h"
#include "stdio.h"

extern char HEATER;
extern char duty_cycle;
extern char heater_set_temp;
extern signed int DS1621_tempF;

void SET_D2_OFF();
void SET_D2_RED();
void SET_D2_BLUE();
void SET_D2_PURPLE();

#define D2_RED PORTDbits.RD6                                                    // pins on the D2 RGB. d6, d7 
#define D2_BLUE PORTDbits.RD7

int get_duty_cycle(signed int temp, int set_temp)
{
    if (temp > set_temp) duty_cycle = 0;
    else duty_cycle = 2 * (set_temp - temp);
    if (duty_cycle > 100) duty_cycle = 100;
    return duty_cycle;
                                                                                // add code to check if temp is greater than set_temp. If so, dc = 0. Else dc = 2 times of difference of set_temp and temp
                                                                                // check if dc is greater than 100. If so, set it to 100
                                                                                // return dc
}

void Monitor_Heater()                                                           // monitor heater
{
    duty_cycle = get_duty_cycle(DS1621_tempF, heater_set_temp);
    do_update_pwm(duty_cycle);
    if (HEATER == 1) Turn_On_Fan();
    else Turn_Off_Fan();
}

void Toggle_Heater()
{                                                                               // toggles HEATER variable
    HEATER = (!HEATER);
}

int get_RPM()                                                                   // rpm value
{
    int RPS = TMR3L / 2;
    TMR3L = 0;
    return (RPS * 60);
}

void Turn_Off_Fan()                                                             // turn off fanb
{
    HEATER = 0;
    FAN_EN = 0;
    FAN_LED = 0;
}

void Turn_On_Fan()                                                              // turn on fan
{
    HEATER = 1;
    do_update_pwm(duty_cycle);
    FAN_EN = 1;
    FAN_LED = 1;
}

void Set_RPM_RGB(int rpm)
{
    if (rpm >= 2700) SET_D2_BLUE();                                             // If rpm >= 2700, color is BLUE 
    else if (rpm >=1800 && rpm < 2700) SET_D2_PURPLE();                         // If rpm >=1800 and < 2700, color is PURPLE
    else if (rpm > 0 && duty_cycle < 1800) SET_D2_RED();                        // If rpm > 0 and < 1800, color is RED
    else if (rpm == 0) SET_D2_OFF();                                            // If rpm = 0, no color to be displayed
}






void SET_D2_OFF()                   // start of d2 colors
{
    D2_RED = 0;
    D2_BLUE = 0;
}
void SET_D2_RED()
{
    D2_RED = 1;
    D2_BLUE = 0;
}
void SET_D2_BLUE()
{
    D2_RED = 0;
    D2_BLUE = 1;
}
void SET_D2_PURPLE()
{
    D2_RED = 1;
    D2_BLUE = 1;
}