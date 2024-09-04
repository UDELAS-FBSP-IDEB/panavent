/**
 * INTERRUPT Generated Driver Source File 
 * 
 * @file      interrupt.c
 *            
 * @ingroup   interruptdriver
 *            
 * @brief     This is the generated driver source file for INTERRUPT driver
 *            
 * @version   Driver Version 1.0.0
 *            
 * @skipline  Device : dsPIC33CK32MC102
*/

/*
© [2022] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

// Section: Includes
#include <xc.h>
#include "../interrupt.h"

// Section: Driver Interface Function Definitions

void INTERRUPT_Initialize(void)
{
    // ADCAN8: ADC AN8 Convert Done
    // Priority: 1
    IPC24bits.ADCAN8IP = 1;
    
    // ADCAN4: ADC AN4 Convert Done
    // Priority: 1
    IPC23bits.ADCAN4IP = 1;
    
    // ADCAN3: ADC AN3 Convert Done
    // Priority: 1
    IPC23bits.ADCAN3IP = 1;
    
    // DMT: Dead Man Timer
    // Priority: 1
    IPC11bits.DMTIP = 1;
    
    // I2C1BC: I2C1 Bus Collision
    // Priority: 5
    IPC16bits.I2C1BCIP = 5;
    
    // MI2C1: I2C1 Master Event
    // Priority: 6
    IPC4bits.MI2C1IP = 6;
    
    // CNB: Change Notification B
    // Priority: 1
    IPC0bits.CNBIP = 1;
    
    // T1: Timer 1
    // Priority: 4
    IPC0bits.T1IP = 4;
    
    // CCT3: CCP3 Timer Event
    // Priority: 7
    IPC9bits.CCT3IP = 7;
    
}

void INTERRUPT_Deinitialize(void)
{
    //POR default value of priority
    IPC24bits.ADCAN8IP = 4;
    IPC23bits.ADCAN4IP = 4;
    IPC23bits.ADCAN3IP = 4;
    IPC11bits.DMTIP = 4;
    IPC16bits.I2C1BCIP = 4;
    IPC4bits.MI2C1IP = 4;
    IPC0bits.CNBIP = 4;
    IPC0bits.T1IP = 4;
    IPC9bits.CCT3IP = 4;
}
