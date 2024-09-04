#include "adcEventDriver.h" 


bool ADCEventDriver_corriendo=false;
ADCEventHandlerStackItem ADCEventDriver_eventHandlerStack[ADC_EVENT_HANDLER_STACK_SIZE];
 
const struct ADCEventDriver adcEventDriver = {
    .Convertir = &ADCEventDriver_Convertir,
    .iniciar = &ADCEventDriver_Iniciar,
    .Detener = &ADCEventDriver_Detener,
    .ClearHandlers = &ADCEventDriver_ClearHandlers,
    .agregarHandler = &ADCEventDriver_AgregarHandler,
    .CancelarHandler = &ADCEventDriver_CancelarHandler, 
};

void ADCEventDriver_Iniciar(void){
    if(!ADCEventDriver_corriendo){
        ADC1.Initialize();
        ADC1.Enable();     
        ADC1.adcMulticoreInterface->ChannelCallbackRegister(&ADCEventDriver_RaiseEvent);
        ADCEventDriver_corriendo=true;
    } 
}

void ADCEventDriver_Detener(void){ 
    if(!ADCEventDriver_corriendo){
        ADC1.Deinitialize(); 
        ADCEventDriver_corriendo=false;
    }  
}

bool ADCEventDriver_CancelarHandler(adcEventHandler  handler, enum ADC_CHANNEL ch){  
    uint8_t i = 0;
    for(; i < ADC_EVENT_HANDLER_STACK_SIZE; i++)
    {
        if(ADCEventDriver_eventHandlerStack[i].eventHandler == handler && ADCEventDriver_eventHandlerStack[i].ch == ch)
        { 
            ADCEventDriver_eventHandlerStack[i].eventHandler = NULL;
            return true; 
        }
    }
    return false;   
};
 
bool ADCEventDriver_AgregarHandler(adcEventHandler  handler, enum ADC_CHANNEL ch, uint8_t avgMuestras){    
    uint8_t i = 0;
    for(; i < ADC_EVENT_HANDLER_STACK_SIZE; i++)
    {
        if(ADCEventDriver_eventHandlerStack[i].eventHandler ==    NULL) {            
            ADCEventDriver_eventHandlerStack[i].eventHandler = handler;
            ADCEventDriver_eventHandlerStack[i].conv = 0; 
            ADCEventDriver_eventHandlerStack[i].ch = ch;  
            ADCEventDriver_eventHandlerStack[i].avgMuestras = avgMuestras;
            ADCEventDriver_eventHandlerStack[i].conteo = 0; 
            return true;
        }
    }   
    return false;
};
 
void ADCEventDriver_ClearHandlers(void){
    uint8_t i = 0;
    for(; i < ADC_EVENT_HANDLER_STACK_SIZE; i++)
    {
        ADCEventDriver_eventHandlerStack[i].eventHandler = NULL; 
    }   
};

void ADCEventDriver_Convertir(enum ADC_CHANNEL ch){
    uint16_t CHx = 0;
//    CHx = ch==AN1? 1 : 0;
//    CHx = ch==AN2? 1 : 0;
    CHx = ch==AN3? 3 : 0;
    CHx = ch==AN4? 4 : 0;
//    CHx = ch==AN5? 1 : 0;
//    CHx = ch==AN6? 1 : 0;
//    CHx = ch==AN7? 1 : 0;
    CHx = ch==AN8? 8 : 0;
//    CHx = ch==AN9? 1 : 0;
    ADCON3Lbits.CNVCHSEL = CHx;
    ADCON3Lbits.CNVRTCH = 1;
}

void ADCEventDriver_RaiseEvent(enum ADC_CHANNEL channel, uint16_t adcVal){
    uint8_t i = 0;
    if(ADCEventDriver_corriendo){
        for(; i < ADC_EVENT_HANDLER_STACK_SIZE; i++)
        {    
            if(ADCEventDriver_eventHandlerStack[i].eventHandler != NULL && ADCEventDriver_eventHandlerStack[i].ch==channel)
            {
                ADCEventDriver_eventHandlerStack[i].conteo++;  
                if (ADCEventDriver_eventHandlerStack[i].conteo==1){
                    ADCEventDriver_eventHandlerStack[i].conv =adcVal;
                }else{
                    ADCEventDriver_eventHandlerStack[i].conv +=adcVal;
                    ADCEventDriver_eventHandlerStack[i].conv /= 2;
                }
                if(ADCEventDriver_eventHandlerStack[i].avgMuestras == ADCEventDriver_eventHandlerStack[i].conteo)
                {                    
                    ADCEventDriver_eventHandlerStack[i].eventHandler(ADCEventDriver_eventHandlerStack[i].ch, ADCEventDriver_eventHandlerStack[i].conv);
                    ADCEventDriver_eventHandlerStack[i].conteo = 0;
                }
            } 
        }
    } 
}

