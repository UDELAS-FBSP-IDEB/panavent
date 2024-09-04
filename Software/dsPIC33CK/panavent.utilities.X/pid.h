#ifndef PID_H
#define	PID_H

#include <xc.h>  
#include <stdbool.h>
#include <stdint.h> 

typedef struct {
    float   emaAlpha;
    float   maxOutput;
    float   minOutput;
    float   kp;
    float   ki;
    float   kd;  
    float   testTarget;
} PIDconstantes;

typedef struct {
    float   lastCalc;
    float   lastSp;
    float   error;
    float   integral;
    float   derivativa; 
    float   previousFilteredError; 
    float   lastTime;
    float   output;
} PIDcalculos;

typedef struct {
    float meta; 
    float medicion; 
    PIDconstantes constantes;
    PIDcalculos calculos; 
} PIDrequest;

typedef struct { 
    void (*inicializar)();
    void (*desinicializar)(); 
    bool (*calcular)(PIDrequest *req);
    void (*resetDeltaTime)(PIDrequest *req); 
} PID;

extern const PID pid;

void  PID_inicializar();
void  PID_desinicializar();
void  PID_timerCallBack();
bool PID_calcular(PIDrequest *req);
void PID_resetDeltaTime(PIDrequest *req);

#endif	