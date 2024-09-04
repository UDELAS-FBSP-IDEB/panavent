/**
 * SCCP2 Generated Driver Source File
 * 
 * @file 	  sccp2.c
 * 
 * @ingroup   pwmdriver
 * 
 * @brief 	  This is the generated driver source file for SCCP2 driver
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
#include "../sccp2.h"

// Section: File specific functions

void (*SCCP2_PWMHandler)(void) = NULL;

// Section: Driver Interface

const struct PWM_INTERFACE PWM2 = {
    .PWM_Initialize          = &SCCP2_PWM_Initialize,
    .PWM_Deinitialize        = &SCCP2_PWM_Deinitialize,
    .PWM_Enable              = &SCCP2_PWM_Enable,
    .PWM_Disable             = &SCCP2_PWM_Disable,
    .PWM_PeriodSet           = &SCCP2_PWM_PeriodSet,
    .PWM_DutyCycleSet        = &SCCP2_PWM_DutyCycleSet,
    .PWM_SoftwareTriggerSet  = &SCCP2_PWM_SoftwareTriggerSet,
    .PWM_DeadTimeSet         = NULL,
    .PWM_OutputModeSet       = NULL,
    .PWM_CallbackRegister = &SCCP2_PWM_CallbackRegister,
    .PWM_Tasks               = &SCCP2_PWM_Tasks
};

// Section: SCCP2 Module APIs

void SCCP2_PWM_Initialize (void)
{
    // MOD Dual Edge Compare, Buffered(PWM); CCSEL disabled; TMR32 16 Bit; TMRPS 1:1; CLKSEL FOSC/2; TMRSYNC disabled; CCPSLP disabled; CCPSIDL disabled; CCPON disabled; 
    CCP2CON1L = 0x5;
    // SYNC None; ALTSYNC disabled; ONESHOT disabled; TRIGEN disabled; IOPS Each Time Base Period Match; RTRGEN disabled; OPSRC Timer Interrupt Event; 
    CCP2CON1H = 0x0;
    // ASDG 0x0; SSDG disabled; ASDGM disabled; PWMRSEN disabled; 
    CCP2CON2L = 0x0;
    // ICSEL ; AUXOUT Disabled; ICGSM Level-Sensitive mode; OCAEN enabled; OENSYNC disabled; 
    CCP2CON2H = 0x100;
    // PSSACE Tri-state; POLACE disabled; OSCNT None; OETRIG disabled; 
    CCP2CON3H = 0x0;
    // ICOV disabled; ICDIS disabled; SCEVT disabled; ASEVT disabled; TRCLR disabled; TRSET disabled; ICGARM disabled; 
    CCP2STATL = 0x0;
    // TMRL 0x0000; 
    CCP2TMRL = 0x0;
    // TMRH 0x0000; 
    CCP2TMRH = 0x0;
    // PRL 39999; 
    CCP1PRL = 0x9C3F;
    // PRH 0; 
    CCP2PRH = 0x0;
    // CMPA 0; 
    CCP2RA = 0x0;
    // CMPB 10000; 
    CCP2RB = 0x2710;
    // BUFL 0x0000; 
    CCP2BUFL = 0x0;
    // BUFH 0x0000; 
    CCP2BUFH = 0x0;
    SCCP2_PWM_CallbackRegister(&SCCP2_PWM_Callback);
    
}

void SCCP2_PWM_Deinitialize (void)
{
    CCP2CON1Lbits.CCPON = 0;
    
    CCP2CON1L = 0x0;
    CCP2CON1H = 0x0;
    CCP2CON2L = 0x0;
    CCP2CON2H = 0x100;
    CCP2CON3H = 0x0;
    CCP2STATL = 0x0;
    CCP2TMRL = 0x0;
    CCP2TMRH = 0x0;
    CCP2PRL = 0xFFFF;
    CCP2PRH = 0xFFFF;
    CCP2RA = 0x0;
    CCP2RB = 0x0;
    CCP2BUFL = 0x0;
    CCP2BUFH = 0x0;
}

void SCCP2_PWM_Enable( void )
{
    CCP2CON1Lbits.CCPON = 1;
}


void SCCP2_PWM_Disable( void )
{
    CCP2CON1Lbits.CCPON = 0;
}

void SCCP2_PWM_PeriodSet(size_t periodCount)
{
    CCP2PRL = periodCount;
}

void SCCP2_PWM_DutyCycleSet(size_t dutyCycleCount)
{
    CCP2RB = dutyCycleCount;
}

void SCCP2_PWM_SoftwareTriggerSet( void )
{
    CCP2STATLbits.TRSET = 1;
}

void SCCP2_PWM_CallbackRegister(void* handler)
{
    if(NULL != handler)
    {
        SCCP2_PWMHandler = handler;
    }
}

void __attribute__ ((weak)) SCCP2_PWM_Callback ( void )
{ 

} 


void SCCP2_PWM_Tasks( void )
{    
    if(IFS1bits.CCT2IF == 1)
    {
        // SCCP2 callback function 
        (*SCCP2_PWMHandler)();
        IFS1bits.CCT2IF = 0;
    }
}
/**
 End of File
*/
