#ifndef TIMER1_CONTROLLER_H
#define TIMER1_CONTROLLER_H 
   
#include "timerController_interface.h"

#define TIMER1_NUM_CALLBACKS    10
#define TIMER1_PERIOD           0.00005

bool Timer1Controller_quitarCallback(void *timerCallback);
bool Timer1Controller_agregarCallback(void *timerCallback, float tiempo);
void Timer1Controller_quitarCallbacks(void); 
ErrorCode Timer1Controller_inicializar(void);
void Timer1Controller_desinicializar(void);
void Timer1Controller_timerCallBack();
  
extern const TimerController timer1Controller; 

#endif
