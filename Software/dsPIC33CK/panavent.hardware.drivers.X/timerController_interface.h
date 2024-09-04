#ifndef TIMER_CONTROLLER_H
#define TIMER_CONTROLLER_H  

#include <xc.h>  
#include <stdbool.h>
#include "../panavent.utilities.X/utilities.h"
#include "mcc_generated_files/timer/timer_interface.h"
 
typedef struct {  
    ErrorCode (*inicializar)(void);
    void (*desinicializar)(void);
    void (*quitarCallbacks)(void); 
    bool (*agregarCallback)(void (*timerCallback), float tiempo);
    bool (*quitarCallback)(void (*timerCallback));  
} TimerController;

typedef struct {
    void (*callback)();
    float tiempo;
    float conteo;
} TimerCallbacks;



#endif
