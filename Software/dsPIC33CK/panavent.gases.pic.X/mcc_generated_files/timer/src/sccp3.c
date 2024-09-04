/**
 * SCCP3-TIMER Generated Driver Source File
 * 
 * @file      sccp3.c
 * 
 * @ingroup   timerdriver
 * 
 * @brief     This is the generated driver source file for SCCP3-TIMER driver
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

// Section: Included Files

#include <xc.h>
#include <stddef.h> 
#include "../sccp3.h"

// Section: Data Type Definitions

#define MASK_32_BIT_LOW 0x0000FFFFU
#define MASK_32_BIT_HIGH 0xFFFF0000U

// Section: File specific functions

void (*SCCP3_TimeoutHandler)(void) = NULL;

// Section: Driver Interface

// Defines an object for TIMER_INTERFACE

const struct TIMER_INTERFACE Timer3 = {
    .Initialize     = &SCCP3_Timer_Initialize,
    .Deinitialize   = &SCCP3_Timer_Deinitialize,
    .Start          = &SCCP3_Timer_Start,
    .Stop           = &SCCP3_Timer_Stop,
    .PeriodCountSet = &SCCP3_Timer_PeriodCountSet,
    .TimeoutCallbackRegister = &SCCP3_TimeoutCallbackRegister,
    .Tasks          = NULL,
};

// Section: Driver Interface Function Definitions

void SCCP3_Timer_Initialize(void)
{
    // MOD ; CCSEL disabled; TMR32 16 Bit; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; 
    CCP3CON1L = 0x0; //The module is disabled, till other settings are configured
    //SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; 
    CCP3CON1H = 0x0;
    //ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; 
    CCP3CON2L = 0x0;
    //ICSEL ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN disabled; OENSYNC disabled; 
    CCP3CON2H = 0x0;
    //PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; 
    CCP3CON3H = 0x0;
    //ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; 
    CCP3STATL = 0x0;
    //TMRL 0x0000; 
    CCP3TMRL = 0x0;
    //TMRH 0x0000; 
    CCP3TMRH = 0x0;
    //PRL 29999; 
    CCP3PRL = 0x752F;
    //PRH 0; 
    CCP3PRH = 0x0;
    //CMPA 0; 
    CCP3RA = 0x0;
    //CMPB 0; 
    CCP3RB = 0x0;
    //BUFL 0x0000; 
    CCP3BUFL = 0x0;
    //BUFH 0x0000; 
    CCP3BUFH = 0x0;
    
    SCCP3_TimeoutCallbackRegister(&SCCP3_TimeoutCallback);

    IFS2bits.CCT3IF = 0;
    // Enabling SCCP3 interrupt
    IEC2bits.CCT3IE = 1;

}

void SCCP3_Timer_Deinitialize(void)
{
    CCP3CON1Lbits.CCPON = 0;
    
    IFS2bits.CCT3IF = 0;
    IEC2bits.CCT3IE = 0;
    
    CCP3CON1L = 0x0; 
    CCP3CON1H = 0x0; 
    CCP3CON2L = 0x0; 
    CCP3CON2H = 0x100; 
    CCP3CON3H = 0x0; 
    CCP3STATL = 0x0; 
    CCP3TMRL = 0x0; 
    CCP3TMRH = 0x0; 
    CCP3PRL = 0xFFFF; 
    CCP3PRH = 0xFFFF; 
    CCP3RA = 0x0; 
    CCP3RB = 0x0; 
    CCP3BUFL = 0x0; 
    CCP3BUFH = 0x0; 
}

void SCCP3_Timer_Start(void)
{
    IFS2bits.CCT3IF = 0;
    // Enable SCCP3 interrupt
    IEC2bits.CCT3IE = 1;
    
    CCP3CON1Lbits.CCPON = 1;
}

void SCCP3_Timer_Stop(void)
{
    CCP3CON1Lbits.CCPON = 0;
    
    IFS2bits.CCT3IF = 0;
    // Disable SCCP3 interrupt
    IEC2bits.CCT3IE = 0;
}

void SCCP3_Timer_PeriodCountSet(size_t count)
{
    CCP3PRL = count & MASK_32_BIT_LOW;
}

void SCCP3_TimeoutCallbackRegister(void* handler)
{
    if(NULL != handler)
    {
        SCCP3_TimeoutHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP3_TimeoutCallback (void)
{ 

} 

void __attribute__ ( ( interrupt, no_auto_psv ) ) _CCT3Interrupt (void)
{
    (*SCCP3_TimeoutHandler)();
    IFS2bits.CCT3IF = 0;
}

/**
 End of File
*/
