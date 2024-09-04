#ifndef ADCEventHandler_h
#define ADCEventHandler_h

#include <stddef.h>
#include <stdint.h>
#include "mcc_generated_files/adc/adc_types.h"

typedef void (*adcEventHandler)(enum ADC_CHANNEL ch, uint16_t conv);

typedef struct {
    adcEventHandler eventHandler;
    enum ADC_CHANNEL ch; 
    uint16_t conv;
    uint8_t avgMuestras;
    uint8_t conteo;
    
} ADCEventHandlerStackItem;


#endif