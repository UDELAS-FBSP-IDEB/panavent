/**
 * PINS Generated Driver Header File 
 * 
 * @file      pins.h
 *            
 * @defgroup  pinsdriver Pins Driver
 *            
 * @brief     The Pin Driver directs the operation and function of 
 *            the selected device pins using dsPIC MCUs.
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

#ifndef PINS_H
#define PINS_H
// Section: Includes
#include <xc.h>

// Section: Device Pin Macros

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA0 GPIO Pin which has a custom name of IO_RA0 to High
 * @pre      The RA0 must be set as Output Pin             
 * @return   none  
 */
#define IO_RA0_SetHigh()          (_LATA0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RA0 GPIO Pin which has a custom name of IO_RA0 to Low
 * @pre      The RA0 must be set as Output Pin
 * @return   none  
 */
#define IO_RA0_SetLow()           (_LATA0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RA0 GPIO Pin which has a custom name of IO_RA0
 * @pre      The RA0 must be set as Output Pin
 * @return   none  
 */
#define IO_RA0_Toggle()           (_LATA0 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RA0 GPIO Pin which has a custom name of IO_RA0
 * @return   none  
 */
#define IO_RA0_GetValue()         _RA0

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA0 GPIO Pin which has a custom name of IO_RA0 as Input
 * @return   none  
 */
#define IO_RA0_SetDigitalInput()  (_TRISA0 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RA0 GPIO Pin which has a custom name of IO_RA0 as Output
 * @return   none  
 */
#define IO_RA0_SetDigitalOutput() (_TRISA0 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB2 GPIO Pin which has a custom name of IO_RB2 to High
 * @pre      The RB2 must be set as Output Pin             
 * @return   none  
 */
#define IO_RB2_SetHigh()          (_LATB2 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB2 GPIO Pin which has a custom name of IO_RB2 to Low
 * @pre      The RB2 must be set as Output Pin
 * @return   none  
 */
#define IO_RB2_SetLow()           (_LATB2 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB2 GPIO Pin which has a custom name of IO_RB2
 * @pre      The RB2 must be set as Output Pin
 * @return   none  
 */
#define IO_RB2_Toggle()           (_LATB2 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB2 GPIO Pin which has a custom name of IO_RB2
 * @return   none  
 */
#define IO_RB2_GetValue()         _RB2

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB2 GPIO Pin which has a custom name of IO_RB2 as Input
 * @return   none  
 */
#define IO_RB2_SetDigitalInput()  (_TRISB2 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB2 GPIO Pin which has a custom name of IO_RB2 as Output
 * @return   none  
 */
#define IO_RB2_SetDigitalOutput() (_TRISB2 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB10 GPIO Pin which has a custom name of IO_RB10 to High
 * @pre      The RB10 must be set as Output Pin             
 * @return   none  
 */
#define IO_RB10_SetHigh()          (_LATB10 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB10 GPIO Pin which has a custom name of IO_RB10 to Low
 * @pre      The RB10 must be set as Output Pin
 * @return   none  
 */
#define IO_RB10_SetLow()           (_LATB10 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB10 GPIO Pin which has a custom name of IO_RB10
 * @pre      The RB10 must be set as Output Pin
 * @return   none  
 */
#define IO_RB10_Toggle()           (_LATB10 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB10 GPIO Pin which has a custom name of IO_RB10
 * @return   none  
 */
#define IO_RB10_GetValue()         _RB10

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB10 GPIO Pin which has a custom name of IO_RB10 as Input
 * @return   none  
 */
#define IO_RB10_SetDigitalInput()  (_TRISB10 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB10 GPIO Pin which has a custom name of IO_RB10 as Output
 * @return   none  
 */
#define IO_RB10_SetDigitalOutput() (_TRISB10 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB11 GPIO Pin which has a custom name of IO_RB11 to High
 * @pre      The RB11 must be set as Output Pin             
 * @return   none  
 */
#define IO_RB11_SetHigh()          (_LATB11 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Sets the RB11 GPIO Pin which has a custom name of IO_RB11 to Low
 * @pre      The RB11 must be set as Output Pin
 * @return   none  
 */
#define IO_RB11_SetLow()           (_LATB11 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Toggles the RB11 GPIO Pin which has a custom name of IO_RB11
 * @pre      The RB11 must be set as Output Pin
 * @return   none  
 */
#define IO_RB11_Toggle()           (_LATB11 ^= 1)

/**
 * @ingroup  pinsdriver
 * @brief    Reads the value of the RB11 GPIO Pin which has a custom name of IO_RB11
 * @return   none  
 */
#define IO_RB11_GetValue()         _RB11

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB11 GPIO Pin which has a custom name of IO_RB11 as Input
 * @return   none  
 */
#define IO_RB11_SetDigitalInput()  (_TRISB11 = 1)

/**
 * @ingroup  pinsdriver
 * @brief    Configures the RB11 GPIO Pin which has a custom name of IO_RB11 as Output
 * @return   none  
 */
#define IO_RB11_SetDigitalOutput() (_TRISB11 = 0)

/**
 * @ingroup  pinsdriver
 * @brief    Initializes the PINS module
 * @return   none  
 */
void PINS_Initialize(void);



#endif
