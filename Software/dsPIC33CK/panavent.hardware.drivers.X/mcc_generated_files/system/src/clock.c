/**
 * CLOCK Generated Driver Source File 
 * 
 * @file      clock.c
 *            
 * @ingroup   clockdriver
 *            
 * @brief     This is the generated source file for CLOCK driver
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
#include <stdint.h>
#include "../clock.h"

void CLOCK_Initialize(void)
{
    /*  
       Input frequency                               :  10.00 MHz
       Clock source                                  :  External Oscillator with PLL
       System frequency (Fosc)                       :  120.00 MHz [(10.00 MHz / 1) * 48 / 2 / 1 = 120.00 MHz]
       PLL VCO frequency (Fvco)                      :  480.00 MHz [(10.00 MHz / 1) * 48 = 480.00 MHz]
       PLL output frequency (Fpllo)                  :  120.00 MHz [(10.00 MHz / 1) * 48 / 2 = 120.00 MHz]
       PLL VCO divider frequency (Fvcodiv)           :  120.00 MHz [480.00 MHz / 2 = 240.00 MHz]
    */
    // RCDIV FRC/1; PLLPRE 1:1; DOZE 1:8; DOZEN disabled; ROI disabled; 
    CLKDIV = 0x2001; // Cambiado a POST2DIV 1:1 y VCODIV FVCO/2
    // PLLDIV 48; 
    PLLFBD = 0x30;
    // TUN Center frequency; 
    OSCTUN = 0x0;
    // PLLPOST 1:2; VCODIV FVCO/2; POST2DIV 1:1; 
    PLLDIV = 0x21; // Modificado para cambiar PLLPOST a 1:2 y VCODIV a FVCO/2
    // ROEN enabled; DIVSWEN enabled; ROSLP disabled; ROSEL FOSC; OE enabled; ROSIDL disabled; 
    REFOCONL = 0x9200;
    // RODIV 3; 
    REFOCONH = 0x3;
    // ROTRIM 384; 
    REFOTRIMH = 0xC000;
    // IOLOCK disabled; 
    RPCON = 0x0;
    // PMDLOCK disabled; 
    PMDCON = 0x0;
    // ADC1MD enabled; T1MD enabled; U2MD enabled; U1MD enabled; SPI2MD enabled; SPI1MD enabled; QEIMD enabled; PWMMD enabled; I2C1MD enabled; 
    PMD1 = 0x0;
    // CCP2MD enabled; CCP1MD enabled; CCP4MD enabled; CCP3MD enabled; 
    PMD2 = 0x0;
    // U3MD enabled; CRCMD enabled; 
    PMD3 = 0x0;
    // REFOMD enabled; 
    PMD4 = 0x0;
    // DMA1MD enabled; DMA2MD enabled; DMA3MD enabled; DMA0MD enabled; 
    PMD6 = 0x0;
    // PTGMD enabled; CMP1MD enabled; 
    PMD7 = 0x0;
    // DMTMD enabled; CLC3MD enabled; OPAMPMD enabled; BIASMD enabled; CLC4MD enabled; SENT1MD enabled; CLC1MD enabled; CLC2MD enabled; 
    PMD8 = 0x0;
    // CF no clock failure; NOSC PRIPLL; CLKLOCK unlocked; OSWEN Switch is Complete; 
    __builtin_write_OSCCONH((uint8_t) (0x03));
    __builtin_write_OSCCONL((uint8_t) (0x01));
    // Wait for Clock switch to occur
    while (OSCCONbits.OSWEN != 0);
    while (OSCCONbits.LOCK != 1);
}



