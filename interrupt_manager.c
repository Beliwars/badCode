
#include "interrupt_manager.h"
#include "mcc.h"

void interrupt INTERRUPT_InterruptManager (void)
{
   // interrupt handler
    if(PIE1bits.TXIE == 1 && PIR1bits.TXIF == 1)
    {
        EUSART_Transmit_ISR();
    }
    else if(PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1)
    {
        EUSART_Receive_ISR();
    }
    else if(INTCONbits.PEIE == 1 && PIE2bits.BCLIE == 1 && PIR2bits.BCLIF == 1)
    {
        I2C_BusCollisionISR();
    }
    else if(INTCONbits.PEIE == 1 && PIE1bits.SSPIE == 1 && PIR1bits.SSPIF == 1)
    {
        I2C_ISR();
    }
    else if(INTCONbits.RBIF==1)
    {
        pbchange();
    }
    else
    {
        //Unhandled Interrupt
    }
}

void pbchange(void)
{
    unsigned char state;
     __delay_ms(1); 				//	delay for 1 ms here for de-bounce 
	state = REB << 1 | REA;			//  combine the pin status and assign to state variable 
	
    if(previous_state == 0x0){				
        if(state == 0x1)
		{
			count -= 1;				//Decrement
			//LCDsetCursor(8,0);			//goto proper position on the LCD screen
			//LCDdataWrite(count);		//display the count value on to LCD
            EUSART_Write('-');
            //EUSART_Write("\n");
		}
        else if( state == 0x2)
		{	
			count += 1;				//Increment
			//LCDsetCursor(8,0);			//goto proper position on the LCD screen
			//LCDdataWrite(count);		//display the count value on to LCD
            EUSART_Write('+');
            //EUSART_Write("\n");
		}
		
	}	
	previous_state = state;				// store the current state value to oldstate value this value will be used in next call					

	PORTB = PORTB;					// read or Any read or write of PORTB,This will end the mismatch condition
	INTCONbits.RBIF = OFF; 			// clear the porb change interrupt flag 
}
/**
 End of File
*/