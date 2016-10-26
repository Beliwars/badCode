/*
 * File:   main.c
 * Author: broblesv
 *
 * Created on 22 September 2016, 16:52
 */

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low Voltage In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
#pragma config WRT = OFF         // FLASH Program Memory Write Enable (Unprotected program memory may be written to by EECON control)

#include <xc.h>

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include "mcc.h"

#define LCD_RETRY_MAX  100  // define the retry count
#define LCD_ADDRESS    0x27 // slave device address


void main() {
    
    SYSTEM_Initialize();  
	
    /*PORTDbits.RD1 = HIGH;
    __delay_ms(1000);
    PORTDbits.RD1 = LOW;
    __delay_ms(1000);
    PORTDbits.RD1 = HIGH;
    __delay_ms(1000);
    PORTDbits.RD1 = LOW;
    __delay_ms(1000);
    
	/*OPTION_REGbits.nRBPU  = PULL_UP_ENABLED; 	// Enable PORTB Weak pull-up */
	//INTCONbits.RBIF = OFF;                      // Clear the interrupt flag 
	//INTCONbits.RBIE = 1;	// enable PORTB change interrupt
    //INT_PORTBLevelChangeEnable();
    //INTCONbits.GIE  = 1;	// enable the global interrupt
    //INT_GlobalInterruptEnable();
    
   /* LCD_Write_Str("Bienvenidos al peo, jajaja");
    LCDscrollDisplayLeft();
    LCDclear();
    LCD_Write_Str("Bienvenidos al peo 2, jajaja");
    LCDscrollDisplayRight();
    LCDclear();
    */
    
    /*EUSART_Write('b');
    EUSART_Write('e');
    EUSART_Write('l');
    EUSART_Write('i');*/
    while(1)
    {
        PORTDbits.RD1 = HIGH;
        __delay_ms(1000);
        //EUSART_Write('-');
        PORTDbits.RD1 = LOW;
        __delay_ms(1000);
       // EUSART_Write('+');
    }
}