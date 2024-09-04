  
#ifndef VALVULA_H
#define	VALVULA_H

#include <xc.h> 
#include <stdbool.h>  
#include "adcEventDriver.h"
#include "mcc_generated_files/pwm/pwm_interface.h"

 
#define VALVE_STACK_SIZE 3 
#define FULL_PERIOD_PWM_CCP1PRL_VAL 20000  
#define AMORTIGUADOR 10.0 

enum tipoValvula{
    CHECK,
    PROPORCIONAL
};

enum estadoValvulaSeg{
    ABIERTA,
    CERRADA
};

typedef struct { 
    const struct PWM_INTERFACE * pwm; 
    enum ADC_CHANNEL ch;
    enum tipoValvula tipo;
    enum estadoValvulaSeg estadoNormal;
    enum estadoValvulaSeg estado;
    enum estadoValvulaSeg estadoPrevio;    
    float maxCorriente;
    float minCorriente; 
    float corriente;
    float voltajeOperacion;
    float seriesResistor;
    float shuntResistor;
    float corrienteMeta;
    bool maxCorrienteAlcanzada; 
    bool corriendo; 
    float _PWMdtc;   
    void (*run)(enum ADC_CHANNEL ch);
    void (*stop)(enum ADC_CHANNEL ch);
    void (*setCorrienteMeta)(enum ADC_CHANNEL ch, float corrienteMeta); 
    void (*setCorriente)(enum ADC_CHANNEL ch, float corriente); 
    void (*calcularPWM)(enum ADC_CHANNEL ch);
    void (*setPWM)(enum ADC_CHANNEL ch, float pct);
} valvula;

 
void runValvula(enum ADC_CHANNEL ch); 
void stopValvula(enum ADC_CHANNEL ch);
void setCorrienteMetaValvula(enum ADC_CHANNEL ch, float corrienteMeta); 
valvula * getValvula(enum ADC_CHANNEL ch);
void calcularPWMValvula(enum ADC_CHANNEL ch);

valvula * inicializarValvula( 
        const struct PWM_INTERFACE * pwm, 
        enum ADC_CHANNEL PinEntrada, 
        enum tipoValvula Tipo, 
        enum estadoValvulaSeg Estado, 
        float CorrienteMinima, 
        float CorrienteMaxima, 
        float ShuntResistor,float SeriesResistor,float VoltajeOperacion);
    
void setCorrienteValvula(enum ADC_CHANNEL ch, float corriente);
void setPWMValvula(enum ADC_CHANNEL ch, float pct);
#endif	/* Valvula_h */

