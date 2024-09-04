/**
 * TMR1 Generated Driver Header File 
 * 
 * @file      tmr1.h
 * 
 * @ingroup   timerdriver
 * 
 * @brief     This is the generated driver header file for the TMR1 driver
 *
 * @version   Driver Version 1.0.0
 *
 * @skipline  Device : dsPIC33CK32MC102
*/

/*
� [2022] Microchip Technology Inc. and its subsidiaries.

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

#ifndef TMR1_H
#define TMR1_H

// Section: Included Files

#include <stddef.h>
#include "timer_interface.h"
// Section: Data Type Definitions


/**
 * @ingroup  timerdriver
 * @brief    Structure object of type TIMER_INTERFACE with the custom name given by 
 *           the user in the Melody Driver User interface. The default name 
 *           e.g. Timer1 can be changed by the user in the TIMER user interface. 
 *           This allows defining a structure with application specific name using 
 *           the 'Custom Name' field. Application specific name allows the API Portability.
*/
extern const struct TIMER_INTERFACE Timer1;

/**
 * @ingroup  timerdriver
 * @brief    Initializes the TMR1 module
 * @return   none
 */
void TMR1_Initialize ( void );

/**
 * @ingroup  timerdriver
 * @brief    Deinitializes the TMR1 to POR values
 * @return   none
 */
void TMR1_Deinitialize(void);


/**
 * @ingroup  timerdriver
 * @brief    Starts the timer
 * @pre      \ref TMR1_Initialize must be called
 * @return   none
 */
void TMR1_Start( void );

/**
 * @ingroup  timerdriver
 * @brief    Stops the timer
 * @pre      \ref TMR1_Initialize must be called
 * @return   none
 */
void TMR1_Stop( void );

/**
 * @ingroup    timerdriver
 * @brief      Sets the TMR1 period count value
 * @param[in]  count - number of clock counts
 * @return     none
 */
void TMR1_PeriodCountSet( size_t count );

/**
 * @ingroup    timerdriver
 * @brief      This function can be used to override default callback and to define 
 *             custom callback for TMR1 Timeout event.
 * @param[in]  handler - Address of the callback function.  
 * @return     none
 */
void TMR1_TimeoutCallbackRegister(void* handler);

/**
 * @ingroup  timerdriver
 * @brief    This is the default callback with weak attribute. The user can 
 *           override and implement the default callback without weak attribute 
 *           or can register a custom callback function using  \ref TMR1_TimeoutCallbackRegister.
 * @return   none  
 */
void TMR1_TimeoutCallback(void);


#endif //TMR1_H

/**
 End of File
*/


