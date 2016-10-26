
#ifndef INTERRUPT_MANAGER_H
#define INTERRUPT_MANAGER_H

#include "pin_manager.h"


#define INT_GlobalInterruptEnable() (INTCONbits.GIE = ON)

#define INT_GlobalInterruptDisable() (INTCONbits.GIE = OFF)

#define INT_PeripheralInterruptEnable() (INTCONbits.PEIE = ON)

#define INT_PeripheralInterruptDisable() (INTCONbits.PEIE = OFF)

#define INT_PORTBLevelChangeEnable() (INTCONbits.RBIE = ON)

#define INT_PORTBLevelChangeDisable() (INTCONbits.RBIE = OFF)

unsigned char previous_state;  // this variable need to be static as it has to retain the value between calls 
unsigned char count;		//this variable will incremented or decremented on encoder rotation

void pbchange(void);		//PORTB change routine this routine is being called by the interrupt service routine on PORTB change interrupts


void interrupt INTERRUPT_InterruptManager(void);

#endif  // INTERRUPT_MANAGER_H
/**
 End of File
*/