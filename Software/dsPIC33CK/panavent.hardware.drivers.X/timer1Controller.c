#include "timer1Controller.h" 
#include "mcc_generated_files/timer/tmr1.h" 
 
TimerCallbacks Timer1Controller_callbacks[10];
 
const TimerController timer1Controller = 
{ 
    .inicializar = &Timer1Controller_inicializar,
    .desinicializar = &Timer1Controller_desinicializar,
    .quitarCallback = &Timer1Controller_quitarCallback,
    .agregarCallback = &Timer1Controller_agregarCallback,
    .quitarCallbacks = &Timer1Controller_quitarCallbacks 
};

bool Timer1_Incializado = false;

bool Timer1Controller_quitarCallback(void *timerCallback){
    uint8_t i = 0;
    for(; i < TIMER1_NUM_CALLBACKS; i++)
    {
        if(Timer1Controller_callbacks[i].callback == timerCallback  )
        { 
            Timer1Controller_callbacks[i].callback = NULL;
            return true; 
        }
    }
    return false; 
};
bool Timer1Controller_agregarCallback(void *timerCallback, float tiempo){
    uint8_t i = 0;
    for(; i < TIMER1_NUM_CALLBACKS; i++)
    {
        if(Timer1Controller_callbacks[i].callback ==    NULL) {            
            Timer1Controller_callbacks[i].callback = timerCallback;
            Timer1Controller_callbacks[i].tiempo =  tiempo;  
            Timer1Controller_callbacks[i].tiempo =0;
            return true;
        }
    }   
    return false;
};
void Timer1Controller_quitarCallbacks(void){
    uint8_t i = 0;
    for(; i < TIMER1_NUM_CALLBACKS; i++)
    {
        Timer1Controller_callbacks[i].callback = NULL; 
    }   
}; 
ErrorCode Timer1Controller_inicializar(void){
    if (!Timer1_Incializado){
        Timer1.Initialize(); 
        Timer1.Start();  
        Timer1.TimeoutCallbackRegister(&Timer1Controller_timerCallBack); 
        Timer1_Incializado=true; 
    }
    return NO_ERROR;
};
void Timer1Controller_desinicializar(void){
    if (Timer1_Incializado){
        Timer1.Stop(); 
        Timer1.Deinitialize();   
        Timer1_Incializado=false;
    }
};
void Timer1Controller_timerCallBack(){
    uint8_t i = 0;
    if(Timer1_Incializado){
        for(; i < TIMER1_NUM_CALLBACKS; i++)
        {    
            if(Timer1Controller_callbacks[i].callback != NULL)
            {
                Timer1Controller_callbacks[i].conteo++;   
                if(Timer1Controller_callbacks[i].tiempo <= (Timer1Controller_callbacks[i].conteo*TIMER1_PERIOD))
                {                    
                    Timer1Controller_callbacks[i].callback();
                    Timer1Controller_callbacks[i].conteo = 0;
                }
            } 
        }
    } 
};