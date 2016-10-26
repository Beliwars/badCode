
#include <xc.h>
#include "pin_manager.h"

void PIN_MANAGER_Initialize(void)
{
    TRISD = 0x00;                   // set the PORTD to output 
    TRISCbits.TRISC6 = INPUT;      //Make RC6/TX an output
    TRISCbits.TRISC7 = INPUT;       //Make RC7/RX an Input
    TRISBbits.TRISB4 = INPUT;		// set rotary encoder pins to input
	TRISBbits.TRISB5 = INPUT;
    ADCON0 = 0x00;                  // FOSC/2, Channel 0 (RA0/AN0), 
    ADCON1 = 0x02;                  // Left Justified, PORT A all Analog Inputs, PORTE all digital, Vref+ = Vdd Vref- = Vss
    TRISA = 0x1F;
}

/**
 End of File
*/
