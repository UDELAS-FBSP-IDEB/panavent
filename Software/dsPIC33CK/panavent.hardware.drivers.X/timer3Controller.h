#ifndef TIMER3_CONTROLLER_H
#define TIMER3_CONTROLLER_H 
  
#include "timerController_interface.h"

#define TIMER3_NUM_CALLBACKS    10
#define TIMER3_PERIOD           0.001

bool Timer3Controller_quitarCallback(void (*timerCallback));
bool Timer3Controller_agregarCallback(void (*timerCallback), float tiempo);
void Timer3Controller_quitarCallbacks(void); 
ErrorCode Timer3Controller_iniciar(void);
void Timer3Controller_detener(void);
void Timer3Controller_timerCallBack();
  
extern const TimerController timer3Controller; 

#endif
