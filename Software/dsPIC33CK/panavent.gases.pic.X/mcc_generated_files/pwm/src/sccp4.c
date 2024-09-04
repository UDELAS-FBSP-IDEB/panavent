/**
 * SCCP4 Generated Driver Source File
 * 
 * @file 	  sccp4.c
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP4 driver
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
#include "../sccp4.h"

// Section: File specific functions

void (*SCCP4_PWMHandler)(void) = NULL;

// Section: Driver Interface

const struct PWM_INTERFACE PWM4 = {
    .PWM_Initialize          = &SCCP4_PWM_Initialize,
    .PWM_Deinitialize        = &SCCP4_PWM_Deinitialize,
    .PWM_Enable              = &SCCP4_PWM_Enable,
    .PWM_Disable             = &SCCP4_PWM_Disable,
    .PWM_PeriodSet           = &SCCP4_PWM_PeriodSet,
    .PWM_DutyCycleSet        = &SCCP4_PWM_DutyCycleSet,
    .PWM_SoftwareTriggerSet  = &SCCP4_PWM_SoftwareTriggerSet,
    .PWM_DeadTimeSet         = NULL,
    .PWM_OutputModeSet       = NULL,
    .PWM_CallbackRegister = &SCCP4_PWM_CallbackRegister,
    .PWM_Tasks               = &SCCP4_PWM_Tasks
};

// Section: SCCP4 Module APIs

void SCCP4_PWM_Initialize (void)
{
    // MOD Dual Edge Compare, Buffered(PWM); CCSEL disabled; TMR32 16 Bit; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; 
    CCP4CON1L = 0x5;
    // SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; 
    CCP4CON1H = 0x0;
    // ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; 
    CCP4CON2L = 0x0;
    // ICSEL ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN enabled; OENSYNC disabled; 
    CCP4CON2H = 0x100;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; 
    CCP4CON3H = 0x0;
    // ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; 
    CCP4STATL = 0x0;
    // TMRL 0x0000; 
    CCP4TMRL = 0x0;
    // TMRH 0x0000; 
    CCP4TMRH = 0x0;
    // PRL 20000; 
    CCP4PRL = 0x4E20;
    // PRH 0; 
    CCP4PRH = 0x0;
    // CMPA 0; 
    CCP4RA = 0x0;
    // CMPB 10000; 
    CCP4RB = 0x2710;
    // BUFL 0x0000; 
    CCP4BUFL = 0x0;
    // BUFH 0x0000; 
    CCP4BUFH = 0x0;
    SCCP4_PWM_CallbackRegister(&SCCP4_PWM_Callback);
    
}

void SCCP4_PWM_Deinitialize (void)
{
    CCP4CON1Lbits.CCPON = 0;
    
    CCP4CON1L = 0x0;
    CCP4CON1H = 0x0;
    CCP4CON2L = 0x0;
    CCP4CON2H = 0x100;
    CCP4CON3H = 0x0;
    CCP4STATL = 0x0;
    CCP4TMRL = 0x0;
    CCP4TMRH = 0x0;
    CCP4PRL = 0xFFFF;
    CCP4PRH = 0xFFFF;
    CCP4RA = 0x0;
    CCP4RB = 0x0;
    CCP4BUFL = 0x0;
    CCP4BUFH = 0x0;
}

void SCCP4_PWM_Enable( void )
{
    CCP4CON1Lbits.CCPON = 1;
}


void SCCP4_PWM_Disable( void )
{
    CCP4CON1Lbits.CCPON = 0;
}

void SCCP4_PWM_PeriodSet(size_t periodCount)
{
    CCP4PRL = periodCount;
}

void SCCP4_PWM_DutyCycleSet(size_t dutyCycleCount)
{
    CCP4RB = dutyCycleCount;
}

void SCCP4_PWM_SoftwareTriggerSet( void )
{
    CCP4STATLbits.TRSET = 1;
}

void SCCP4_PWM_CallbackRegister(void* handler)
{
    if(NULL != handler)
    {
        SCCP4_PWMHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP4_PWM_Callback ( void )
{ 

} 


void SCCP4_PWM_Tasks( void )
{    
    if(IFS2bits.CCT4IF == 1)
    {
        // SCCP4 callback function 
        (*SCCP4_PWMHandler)();
        IFS2bits.CCT4IF = 0;
    }
}
/**
 End of File
*/
