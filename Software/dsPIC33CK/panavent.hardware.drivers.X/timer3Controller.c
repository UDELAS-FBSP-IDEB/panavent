#include "timer3Controller.h" 
#include "mcc_generated_files/timer/sccp3.h"
TimerCallbacks Timer3Controller_callbacks[10];
 
const TimerController timer3Controller = { 
    .inicializar = &Timer3Controller_iniciar,
    .desinicializar = &Timer3Controller_detener,
    .quitarCallback = &Timer3Controller_quitarCallback,
    .agregarCallback = &Timer3Controller_agregarCallback,
    .quitarCallbacks = &Timer3Controller_quitarCallbacks 
};

bool Timer3_Incializado = false;

bool Timer3Controller_quitarCallback(void *timerCallback){
    uint8_t i = 0;
    for(; i < TIMER3_NUM_CALLBACKS; i++)
    {
        if(Timer3Controller_callbacks[i].callback == timerCallback  )
        { 
            Timer3Controller_callbacks[i].callback = NULL;
            return true; 
        }
    }
    return false; 
};
bool Timer3Controller_agregarCallback(void *timerCallback, float tiempo){
    uint8_t i = 0;
    for(; i < TIMER3_NUM_CALLBACKS; i++)
    {
        if(Timer3Controller_callbacks[i].callback ==    NULL) {            
            Timer3Controller_callbacks[i].callback = timerCallback;
            Timer3Controller_callbacks[i].tiempo =  tiempo;  
            Timer3Controller_callbacks[i].tiempo =0;
            return true;
        }
    }   
    return false;
};
void Timer3Controller_quitarCallbacks(void){
    uint8_t i = 0;
    for(; i < TIMER3_NUM_CALLBACKS; i++)
    {
        Timer3Controller_callbacks[i].callback = NULL; 
    }   
}; 
ErrorCode Timer3Controller_iniciar(void){
    //if (!Timer3_Incializado){
        Timer3.Initialize(); 
        Timer3.Start();  
        Timer3.TimeoutCallbackRegister(&Timer3Controller_timerCallBack); 
        Timer3_Incializado=true;
    //}
    return NO_ERROR;
};
void Timer3Controller_detener(void){
    if (Timer3_Incializado){
        Timer3.Stop(); 
        Timer3.Deinitialize();   
        Timer3_Incializado=false;
    }
};
void Timer3Controller_timerCallBack(){
    uint8_t i = 0;
    if(Timer3_Incializado){
        for(; i < TIMER3_NUM_CALLBACKS; i++)
        {    
            if(Timer3Controller_callbacks[i].callback != NULL)
            {
                Timer3Controller_callbacks[i].conteo++;   
                if(Timer3Controller_callbacks[i].tiempo <= (Timer3Controller_callbacks[i].conteo*TIMER3_PERIOD))
                {                    
                    Timer3Controller_callbacks[i].callback();
                    Timer3Controller_callbacks[i].conteo = 0;
                }
            } 
        }
    } 
};