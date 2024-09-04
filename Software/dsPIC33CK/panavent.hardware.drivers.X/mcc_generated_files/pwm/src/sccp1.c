/**
 * SCCP1 Generated Driver Source File
 * 
 * @file 	  sccp1.c
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP1 driver
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
#include "../sccp1.h"

// Section: File specific functions

void (*SCCP1_PWMHandler)(void) = NULL;

// Section: Driver Interface

const struct PWM_INTERFACE PWM1 = {
    .PWM_Initialize          = &SCCP1_PWM_Initialize,
    .PWM_Deinitialize        = &SCCP1_PWM_Deinitialize,
    .PWM_Enable              = &SCCP1_PWM_Enable,
    .PWM_Disable             = &SCCP1_PWM_Disable,
    .PWM_PeriodSet           = &SCCP1_PWM_PeriodSet,
    .PWM_DutyCycleSet        = &SCCP1_PWM_DutyCycleSet,
    .PWM_SoftwareTriggerSet  = &SCCP1_PWM_SoftwareTriggerSet,
    .PWM_DeadTimeSet         = NULL,
    .PWM_OutputModeSet       = NULL,
    .PWM_CallbackRegister = &SCCP1_PWM_CallbackRegister,
    .PWM_Tasks               = &SCCP1_PWM_Tasks
};

// Section: SCCP1 Module APIs

void SCCP1_PWM_Initialize (void)
{
    // MOD Dual Edge Compare, Buffered(PWM); CCSEL disabled; TMR32 16 Bit; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; 
    CCP1CON1L = 0x5;
    // SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; 
    CCP1CON1H = 0x0;
    // ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; 
    CCP1CON2L = 0x0;
    // ICSEL ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN enabled; OENSYNC disabled; 
    CCP1CON2H = 0x100;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; 
    CCP1CON3H = 0x0;
    // ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; 
    CCP1STATL = 0x0;
    // TMRL 0x0000; 
    CCP1TMRL = 0x0;
    // TMRH 0x0000; 
    CCP1TMRH = 0x0;
    // PRL 39999; 
    CCP1PRL = 0x9C3F;
    // PRH 0; 
    CCP1PRH = 0x0;
    // CMPA 0; 
    CCP1RA = 0x0;
    // CMPB 10000; 
    CCP1RB = 0x2710;
    // BUFL 0x0000; 
    CCP1BUFL = 0x0;
    // BUFH 0x0000; 
    CCP1BUFH = 0x0;
    SCCP1_PWM_CallbackRegister(&SCCP1_PWM_Callback);
    
}

void SCCP1_PWM_Deinitialize (void)
{
    CCP1CON1Lbits.CCPON = 0;
    
    CCP1CON1L = 0x0;
    CCP1CON1H = 0x0;
    CCP1CON2L = 0x0;
    CCP1CON2H = 0x100;
    CCP1CON3H = 0x0;
    CCP1STATL = 0x0;
    CCP1TMRL = 0x0;
    CCP1TMRH = 0x0;
    CCP1PRL = 0xFFFF;
    CCP1PRH = 0xFFFF;
    CCP1RA = 0x0;
    CCP1RB = 0x0;
    CCP1BUFL = 0x0;
    CCP1BUFH = 0x0;
}

void SCCP1_PWM_Enable( void )
{
    CCP1CON1Lbits.CCPON = 1;
}


void SCCP1_PWM_Disable( void )
{
    CCP1CON1Lbits.CCPON = 0;
}

void SCCP1_PWM_PeriodSet(size_t periodCount)
{
    CCP1PRL = periodCount;
}

void SCCP1_PWM_DutyCycleSet(size_t dutyCycleCount)
{
    CCP1RB = dutyCycleCount;
}

void SCCP1_PWM_SoftwareTriggerSet( void )
{
    CCP1STATLbits.TRSET = 1;
}

void SCCP1_PWM_CallbackRegister(void* handler)
{
    if(NULL != handler)
    {
        SCCP1_PWMHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP1_PWM_Callback ( void )
{ 

} 


void SCCP1_PWM_Tasks( void )
{    
    if(IFS0bits.CCT1IF == 1)
    {
        // SCCP1 callback function 
        (*SCCP1_PWMHandler)();
        IFS0bits.CCT1IF = 0;
    }
}
/**
 End of File
*/
