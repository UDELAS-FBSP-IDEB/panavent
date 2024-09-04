#ifndef MEZCLADOR_H
#define	MEZCLADOR_H

#include <stdbool.h> 
#include <stdint.h>
#include "../panavent.utilities.X/utilities.h"   
#include "../panavent.hardware.drivers.X/mcc_generated_files/adc/adc_types.h"
#include "../panavent.hardware.drivers.X/sfm_interface.h" 
#include "../panavent.utilities.X/pid.h"
typedef struct { 
    const struct PWM_INTERFACE * pwm;   
    float corriente;   
    float corrienteMeta;   
} Valvula;

typedef struct { 
    bool inicializado;  
    float presionAire;
    float presionOxigeno;
    float flujoAire;
    float flujoOxigeno;
    float flujoProximal;   
    ErrorCode (*inicializar)();
    bool (*mezclar)();
    void (*setConstatesPID)(uint8_t index, PIDconstantes constantes, bool test);
    void (*setCalibracion)(uint8_t index, float M, float B); 
    float (*mezclarAire)();
    float (*mezclarOxigeno)(); 
    void (*leerPresiones)(); 
    float fa;
    float fo;
    float fm;
    float cm;
    float cl; 
    float data;
} Mezclador;

extern Mezclador mezclador;

ErrorCode Mezclador_inicializar();
void Mezclador_timerCallBack();
void Mezclador_conversionCallBack(enum ADC_CHANNEL ch, uint16_t adcVal); 
void Mezclador_setConstantesPID(uint8_t index, PIDconstantes constantes, bool test);
bool Mezclador_mezclar(); 
float Mezclador_getPresionAire();
float Mezclador_getPresionOxigeno();
void Mezclador_setCalibracion(uint8_t index, float M, float B);
float Mezclador_mezclarAire();
float Mezclador_mezclarOxigeno();
float Mezclador_mezclarGas(Sfm *sensor, float *flujo, float calB, int pidIndex, int mezcladorIndex, float *corrienteMeta, bool *filterInit, MovingAverageFilter *filter, float *meta);
void Mezclador_leerPresiones();

double butterworthFilter(double input);

#endif