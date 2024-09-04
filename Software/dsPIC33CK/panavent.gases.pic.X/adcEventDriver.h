#ifndef ADCEventDriver_h
#define ADCEventDriver_h 

#include "adcEventHandler.h"
#include <xc.h>
#include <stdbool.h>
#include "mcc_generated_files/adc/adc1.h"


#ifndef ADC_EVENT_HANDLER_STACK_SIZE
    #define ADC_EVENT_HANDLER_STACK_SIZE 10
#endif

bool ADCEventDriver_CancelarHandler(adcEventHandler  handler, enum ADC_CHANNEL ch);
bool ADCEventDriver_AgregarHandler(adcEventHandler  handler, enum ADC_CHANNEL ch, uint8_t avgMuestras);
void ADCEventDriver_ClearHandlers(void);
void ADCEventDriver_RaiseEvent(enum ADC_CHANNEL ch, uint16_t adcVal);
void ADCEventDriver_Iniciar(void);
void ADCEventDriver_Detener(void);
void ADCEventDriver_Convertir(enum ADC_CHANNEL ch);

extern const struct ADCEventDriver{   
    //void (*RaiseEvent)(enum ADC_CHANNEL ch, uint16_t adcVal);
    void (*iniciar)(void);
    void (*Detener)(void);
    void (*Convertir)(enum ADC_CHANNEL ch);
    void (*ClearHandlers)(void); 
    bool (*agregarHandler)(adcEventHandler  handler, enum ADC_CHANNEL ch, uint8_t avgMuestras);
    bool (*CancelarHandler)(adcEventHandler  handler, enum ADC_CHANNEL ch);  
} adcEventDriver;


#endif
