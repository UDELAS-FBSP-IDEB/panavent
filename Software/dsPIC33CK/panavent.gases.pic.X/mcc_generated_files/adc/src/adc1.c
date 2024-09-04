/**
 * ADC1 Generated Driver Source File
 * 
 * @file      adc1.c
 *            
 * @ingroup   adcdriver
 *            
 * @brief     This is the generated driver source file for ADC1 driver
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
#include <stddef.h>
#include "../adc1.h"

// Section: File specific functions

void (*ADC1_CommonHandler)(void) = NULL;
void (*ADC1_ChannelHandler)(enum ADC_CHANNEL channel, uint16_t adcVal) = NULL;
void (*ADC1_ComparatorHandler)(enum ADC_CMP comparator) = NULL;

// Section: File specific data type definitions

//Defines an object for ADC_MULTICORE.
const struct ADC_MULTICORE adc1Multicore = {
    .ChannelTasks                           = NULL,
    .ComparatorTasks                        = NULL,
    .IndividualChannelInterruptEnable       = &ADC1_IndividualChannelInterruptEnable,
    .IndividualChannelInterruptDisable      = &ADC1_IndividualChannelInterruptDisable,
    .IndividualChannelInterruptFlagClear    = &ADC1_IndividualChannelInterruptFlagClear,
    .ChannelCallbackRegister                = &ADC1_ChannelCallbackRegister,
    .ComparatorCallbackRegister             = &ADC1_ComparatorCallbackRegister,
    .CorePowerEnable                        = NULL,
    .SharedCorePowerEnable                  = &ADC1_SharedCorePowerEnable,
};

//Defines an object for ADC_INTERFACE.

const struct ADC_INTERFACE ADC1 = {
    .Initialize             = &ADC1_Initialize,
    .Deinitialize           = &ADC1_Deinitialize,
    .Enable                 = &ADC1_Enable,
    .Disable                = &ADC1_Disable,
    .SoftwareTriggerEnable  = &ADC1_SoftwareTriggerEnable,
    .SoftwareTriggerDisable = &ADC1_SoftwareTriggerDisable,
    .ChannelSelect          = &ADC1_ChannelSelect, 
    .ConversionResultGet    = &ADC1_ConversionResultGet,
    .IsConversionComplete   = &ADC1_IsConversionComplete,
    .InterruptEnable        = &ADC1_InterruptEnable,
    .InterruptDisable       = &ADC1_InterruptDisable,
    .InterruptFlagClear     = &ADC1_InterruptFlagClear,
    .InterruptPrioritySet   = &ADC1_InterruptPrioritySet,
    .CommonCallbackRegister = &ADC1_CommonCallbackRegister,
    .Tasks                  = &ADC1_Tasks,
    .adcMulticoreInterface = &adc1Multicore,
};

// Section: Driver Interface Function Definitions

void ADC1_Initialize (void)
{
    // ADSIDL disabled; ADON enabled; 
    ADCON1L = (0x8000 & 0x7FFF); //Disabling ADON bit
    // FORM Integer; SHRRES 12-bit resolution; 
    ADCON1H = 0x60;
    // SHRADCS 2; REFCIE disabled; SHREISEL Early interrupt is generated 1 TADCORE clock prior to data being ready; REFERCIE disabled; EIEN disabled; 
    ADCON2L = 0x0;
    // SHRSAMC 43; 
    ADCON2H = 0x2B;
    // SWCTRG disabled; SHRSAMP disabled; SUSPEND disabled; SWLCTRG disabled; SUSPCIE disabled; CNVCHSEL AN0; REFSEL disabled; 
    ADCON3L = 0x0;
    // SHREN enabled; CLKDIV 1; CLKSEL FOSC/2; 
    ADCON3H = (0x80 & 0xFF00); //Disabling C0EN, C1EN, C2EN, C3EN and SHREN bits
    // SIGN0 disabled; SIGN4 disabled; SIGN3 disabled; SIGN2 disabled; SIGN1 disabled; SIGN7 disabled; SIGN6 disabled; SIGN5 disabled; 
    ADMOD0L = 0x0;
    // SIGN8 disabled; SIGN9 disabled; SIGN10 disabled; SIGN11 disabled; 
    ADMOD0H = 0x0;
    // SIGN16 disabled; SIGN17 disabled; 
    ADMOD1L = 0x0;
    // IE1 disabled; IE0 disabled; IE3 enabled; IE2 disabled; IE5 disabled; IE4 enabled; IE10 disabled; IE7 disabled; IE6 disabled; IE9 disabled; IE8 enabled; IE11 disabled; 
    ADIEL = 0x118;
    // IE17 disabled; IE16 disabled; 
    ADIEH = 0x0;
    // 
    ADSTATL = 0x0;
    // 
    ADSTATH = 0x0;
    // CMPEN10 disabled; CMPEN11 disabled; CMPEN6 disabled; CMPEN5 disabled; CMPEN4 disabled; CMPEN3 disabled; CMPEN2 disabled; CMPEN1 disabled; CMPEN0 disabled; CMPEN9 disabled; CMPEN8 disabled; CMPEN7 disabled; 
    ADCMP0ENL = 0x0;
    // CMPEN10 disabled; CMPEN11 disabled; CMPEN6 disabled; CMPEN5 disabled; CMPEN4 disabled; CMPEN3 disabled; CMPEN2 disabled; CMPEN1 disabled; CMPEN0 disabled; CMPEN9 disabled; CMPEN8 disabled; CMPEN7 disabled; 
    ADCMP1ENL = 0x0;
    // CMPEN10 disabled; CMPEN11 disabled; CMPEN6 disabled; CMPEN5 disabled; CMPEN4 disabled; CMPEN3 disabled; CMPEN2 disabled; CMPEN1 disabled; CMPEN0 disabled; CMPEN9 disabled; CMPEN8 disabled; CMPEN7 disabled; 
    ADCMP2ENL = 0x0;
    // CMPEN10 disabled; CMPEN11 disabled; CMPEN6 disabled; CMPEN5 disabled; CMPEN4 disabled; CMPEN3 disabled; CMPEN2 disabled; CMPEN1 disabled; CMPEN0 disabled; CMPEN9 disabled; CMPEN8 disabled; CMPEN7 disabled; 
    ADCMP3ENL = 0x0;
    // CMPEN16 disabled; CMPEN17 disabled; 
    ADCMP0ENH = 0x0;
    // CMPEN16 disabled; CMPEN17 disabled; 
    ADCMP1ENH = 0x0;
    // CMPEN16 disabled; CMPEN17 disabled; 
    ADCMP2ENH = 0x0;
    // CMPEN16 disabled; CMPEN17 disabled; 
    ADCMP3ENH = 0x0;
    // CMPLO 0x0; 
    ADCMP0LO = 0x0;
    // CMPLO 0x0; 
    ADCMP1LO = 0x0;
    // CMPLO 0x0; 
    ADCMP2LO = 0x0;
    // CMPLO 0x0; 
    ADCMP3LO = 0x0;
    // CMPHI 0x0; 
    ADCMP0HI = 0x0;
    // CMPHI 0x0; 
    ADCMP1HI = 0x0;
    // CMPHI 0x0; 
    ADCMP2HI = 0x0;
    // CMPHI 0x0; 
    ADCMP3HI = 0x0;
    // 
    ADFL0DAT = 0x0;
    // 
    ADFL1DAT = 0x0;
    // 
    ADFL2DAT = 0x0;
    // 
    ADFL3DAT = 0x0;
    // OVRSAM 4x; MODE Oversampling Mode; FLCHSEL AN0; IE disabled; FLEN disabled; 
    ADFL0CON = 0x400;
    // OVRSAM 4x; MODE Oversampling Mode; FLCHSEL AN0; IE disabled; FLEN disabled; 
    ADFL1CON = 0x400;
    // OVRSAM 4x; MODE Oversampling Mode; FLCHSEL AN0; IE disabled; FLEN disabled; 
    ADFL2CON = 0x400;
    // OVRSAM 4x; MODE Oversampling Mode; FLCHSEL AN0; IE disabled; FLEN disabled; 
    ADFL3CON = 0x400;
    // HIHI disabled; LOLO disabled; HILO disabled; BTWN disabled; LOHI disabled; CMPEN disabled; IE disabled; 
    ADCMP0CON = 0x0;
    // HIHI disabled; LOLO disabled; HILO disabled; BTWN disabled; LOHI disabled; CMPEN disabled; IE disabled; 
    ADCMP1CON = 0x0;
    // HIHI disabled; LOLO disabled; HILO disabled; BTWN disabled; LOHI disabled; CMPEN disabled; IE disabled; 
    ADCMP2CON = 0x0;
    // HIHI disabled; LOLO disabled; HILO disabled; BTWN disabled; LOHI disabled; CMPEN disabled; IE disabled; 
    ADCMP3CON = 0x0;
    // LVLEN9 disabled; LVLEN8 disabled; LVLEN11 disabled; LVLEN7 disabled; LVLEN10 disabled; LVLEN6 disabled; LVLEN5 disabled; LVLEN4 disabled; LVLEN3 disabled; LVLEN2 disabled; LVLEN1 disabled; LVLEN0 disabled; 
    ADLVLTRGL = 0x0;
    // LVLEN17 disabled; LVLEN16 disabled; 
    ADLVLTRGH = 0x0;
    // EIEN9 disabled; EIEN7 disabled; EIEN8 disabled; EIEN5 disabled; EIEN6 disabled; EIEN3 disabled; EIEN4 disabled; EIEN1 disabled; EIEN2 disabled; EIEN0 disabled; EIEN11 disabled; EIEN10 disabled; 
    ADEIEL = 0x0;
    // EIEN17 disabled; EIEN16 disabled; 
    ADEIEH = 0x0;
    // EISTAT0 disabled; EISTAT11 disabled; EISTAT1 disabled; EISTAT2 disabled; EISTAT3 disabled; EISTAT4 disabled; EISTAT5 disabled; EISTAT6 disabled; EISTAT7 disabled; EISTAT10 disabled; EISTAT8 disabled; EISTAT9 disabled; 
    ADEISTATL = 0x0;
    // EISTAT16 disabled; EISTAT17 disabled; 
    ADEISTATH = 0x0;
    // SHRCIE disabled; WARMTIME 16 Source Clock Periods; 
    ADCON5H = (0x400 & 0xF0FF); //Disabling WARMTIME bit
    // 
    ADCBUF0 = 0x0;
    // 
    ADCBUF1 = 0x0;
    // 
    ADCBUF2 = 0x0;
    // 
    ADCBUF3 = 0x0;
    // 
    ADCBUF4 = 0x0;
    // 
    ADCBUF5 = 0x0;
    // 
    ADCBUF6 = 0x0;
    // 
    ADCBUF7 = 0x0;
    // 
    ADCBUF8 = 0x0;
    // 
    ADCBUF9 = 0x0;
    // 
    ADCBUF10 = 0x0;
    // 
    ADCBUF11 = 0x0;
    // 
    ADCBUF16 = 0x0;
    // 
    ADCBUF17 = 0x0;
        
    ADC1_CommonCallbackRegister(&ADC1_CommonCallback);
    ADC1_ChannelCallbackRegister(&ADC1_ChannelCallback);
    ADC1_ComparatorCallbackRegister(&ADC1_ComparatorCallback);
    
    // Clearing AN3 interrupt flag.
    IFS5bits.ADCAN3IF = 0;
    // Enabling AN3 interrupt.
    IEC5bits.ADCAN3IE = 1;
    // Clearing AN4 interrupt flag.
    IFS5bits.ADCAN4IF = 0;
    // Enabling AN4 interrupt.
    IEC5bits.ADCAN4IE = 1;
    // Clearing AN8 interrupt flag.
    IFS6bits.ADCAN8IF = 0;
    // Enabling AN8 interrupt.
    IEC6bits.ADCAN8IE = 1;

    // Setting WARMTIME bit
    ADCON5Hbits.WARMTIME = 0xF;
    // Enabling ADC Module
    ADCON1Lbits.ADON = 0x1;
    // Enabling Power for the Shared Core
    ADC1_SharedCorePowerEnable();

    //TRGSRC0 None; TRGSRC1 None; 
    ADTRIG0L = 0x0;
    //TRGSRC3 None; TRGSRC2 None; 
    ADTRIG0H = 0x0;
    //TRGSRC4 None; TRGSRC5 None; 
    ADTRIG1L = 0x0;
    //TRGSRC6 None; TRGSRC7 None; 
    ADTRIG1H = 0x0;
    //TRGSRC8 None; TRGSRC9 None; 
    ADTRIG2L = 0x0;
    //TRGSRC11 None; TRGSRC10 None; 
    ADTRIG2H = 0x0;
    //TRGSRC17 None; TRGSRC16 None; 
    ADTRIG4L = 0x0;
}

void ADC1_Deinitialize (void)
{
    uint16_t dummy; //buffers has to be read before clearing interrupt flags
    ADCON1Lbits.ADON = 0;
    
    dummy = ADCBUF3;
    IFS5bits.ADCAN3IF = 0;
    IEC5bits.ADCAN3IE = 0;
    
    dummy = ADCBUF4;
    IFS5bits.ADCAN4IF = 0;
    IEC5bits.ADCAN4IE = 0;
    
    dummy = ADCBUF8;
    IFS6bits.ADCAN8IF = 0;
    IEC6bits.ADCAN8IE = 0;
    
    ADCON1L = 0x0;
    ADCON1H = 0x60;
    ADCON2L = 0x0;
    ADCON2H = 0x0;
    ADCON3L = 0x0;
    ADCON3H = 0x0;
    ADMOD0L = 0x0;
    ADMOD0H = 0x0;
    ADMOD1L = 0x0;
    ADIEL = 0x0;
    ADIEH = 0x0;
    ADSTATL = 0x0;
    ADSTATH = 0x0;
    ADCMP0ENL = 0x0;
    ADCMP1ENL = 0x0;
    ADCMP2ENL = 0x0;
    ADCMP3ENL = 0x0;
    ADCMP0ENH = 0x0;
    ADCMP1ENH = 0x0;
    ADCMP2ENH = 0x0;
    ADCMP3ENH = 0x0;
    ADCMP0LO = 0x0;
    ADCMP1LO = 0x0;
    ADCMP2LO = 0x0;
    ADCMP3LO = 0x0;
    ADCMP0HI = 0x0;
    ADCMP1HI = 0x0;
    ADCMP2HI = 0x0;
    ADCMP3HI = 0x0;
    ADFL0DAT = 0x0;
    ADFL1DAT = 0x0;
    ADFL2DAT = 0x0;
    ADFL3DAT = 0x0;
    ADFL0CON = 0x0;
    ADFL1CON = 0x0;
    ADFL2CON = 0x0;
    ADFL3CON = 0x0;
    ADTRIG0L = 0x0;
    ADTRIG0H = 0x0;
    ADTRIG1L = 0x0;
    ADTRIG1H = 0x0;
    ADTRIG2L = 0x0;
    ADTRIG2H = 0x0;
    ADTRIG4L = 0x0;
    ADCMP0CON = 0x0;
    ADCMP1CON = 0x0;
    ADCMP2CON = 0x0;
    ADCMP3CON = 0x0;
    ADLVLTRGL = 0x0;
    ADLVLTRGH = 0x0;
    ADEIEL = 0x0;
    ADEIEH = 0x0;
    ADEISTATL = 0x0;
    ADEISTATH = 0x0;
    ADCON5L = 0x0;
    ADCON5H = 0x0;
    ADCBUF0 = 0x0;
    ADCBUF1 = 0x0;
    ADCBUF2 = 0x0;
    ADCBUF3 = 0x0;
    ADCBUF4 = 0x0;
    ADCBUF5 = 0x0;
    ADCBUF6 = 0x0;
    ADCBUF7 = 0x0;
    ADCBUF8 = 0x0;
    ADCBUF9 = 0x0;
    ADCBUF10 = 0x0;
    ADCBUF11 = 0x0;
    ADCBUF16 = 0x0;
    ADCBUF17 = 0x0;
}


void ADC1_SharedCorePowerEnable ( ) 
{
    ADCON5Lbits.SHRPWR = 1;   
    while(ADCON5Lbits.SHRRDY == 0)
    {
    }
    ADCON3Hbits.SHREN = 1;   
}


void ADC1_CommonCallbackRegister(void(*callback)(void))
{
    if(NULL != callback)
    {
        ADC1_CommonHandler = callback;
    }
}

void __attribute__ ((weak)) ADC1_CommonCallback (void)
{ 

} 


void __attribute__ ((weak)) ADC1_Tasks ( void )
{
    if(IFS5bits.ADCIF == 1)
    {
        (*ADC1_CommonHandler)();

        // clear the ADC1 interrupt flag
        IFS5bits.ADCIF = 0;
    }
}

void ADC1_ChannelCallbackRegister(void(*callback)(enum ADC_CHANNEL channel, uint16_t adcVal))
{
    if(NULL != callback)
    {
        ADC1_ChannelHandler = callback;
    }
}

void __attribute__ ((weak)) ADC1_ChannelCallback (enum ADC_CHANNEL channel, uint16_t adcVal)
{ 
    (void)channel;
    (void)adcVal;
} 


void __attribute__ ( ( __interrupt__ , auto_psv, weak ) ) _ADCAN3Interrupt ( void )
{
    uint16_t valAN3;
    //Read the ADC value from the ADCBUF
    valAN3 = ADCBUF3;

    (*ADC1_ChannelHandler)(AN3, valAN3);

    //clear the AN3 interrupt flag
    IFS5bits.ADCAN3IF = 0;
}

void __attribute__ ( ( __interrupt__ , auto_psv, weak ) ) _ADCAN4Interrupt ( void )
{
    uint16_t valAN4;
    //Read the ADC value from the ADCBUF
    valAN4 = ADCBUF4;

    (*ADC1_ChannelHandler)(AN4, valAN4);

    //clear the AN4 interrupt flag
    IFS5bits.ADCAN4IF = 0;
}

void __attribute__ ( ( __interrupt__ , auto_psv, weak ) ) _ADCAN8Interrupt ( void )
{
    uint16_t valAN8;
    //Read the ADC value from the ADCBUF
    valAN8 = ADCBUF8;

    (*ADC1_ChannelHandler)(AN8, valAN8);

    //clear the AN8 interrupt flag
    IFS6bits.ADCAN8IF = 0;
}




void ADC1_ComparatorCallbackRegister(void(*callback)(enum ADC_CMP comparator))
{
    if(NULL != callback)
    {
        ADC1_ComparatorHandler = callback;
    }
}

void __attribute__ ((weak)) ADC1_ComparatorCallback (enum ADC_CMP comparator)
{ 
    (void)comparator;
} 



