#include "pid.h"
#include <libpic30.h>
#include "../panavent.hardware.drivers.X/timer3Controller.h"

volatile float pidTime = 0;
bool timer3iniciado = false;
bool pidInicializado = false; 
 

const PID pid = {
    .inicializar = &PID_inicializar,
    .desinicializar = &PID_desinicializar, 
    .calcular = &PID_calcular,
    .resetDeltaTime = &PID_resetDeltaTime
};

void PID_inicializar() {
    if (!pidInicializado) {
        timer3Controller.inicializar();
        timer3Controller.agregarCallback(&PID_timerCallBack, TIMER3_PERIOD);
        pidInicializado = true;
         
         
    }
};

void PID_desinicializar() {
    if (pidInicializado) {
        timer3Controller.quitarCallback(&PID_timerCallBack);
        pidInicializado = false;
    }
};

void PID_timerCallBack() {
    pidTime += TIMER3_PERIOD;
};
 
void PID_resetDeltaTime(PIDrequest *req){
    PIDcalculos *calculos = &(req->calculos);
   
    calculos->lastTime = pidTime; 
}

bool PID_calcular(PIDrequest *req) {
    PIDconstantes constantes = req->constantes;
    PIDcalculos *calculos = &(req->calculos); 
    float currentTime = pidTime;
    float deltaTime = currentTime - calculos->lastTime; 
    
    if (req->meta == 0) {
        calculos->integral = 0;
        calculos->previousFilteredError = 0;
        calculos->lastTime = currentTime;
        calculos->output = 0;
        return true;
    } else {
        if (deltaTime > 0) {
            float error = req->meta - req->medicion;
            float filteredError = constantes.emaAlpha * error + (1 - constantes.emaAlpha) * calculos->previousFilteredError;
            float derivative = (filteredError - calculos->previousFilteredError) / deltaTime;
            
             
//            if (error > 0 && (calculos->output < constantes.maxOutput)) {
//                calculos->integral += error * deltaTime;
//            }

            calculos->integral += error * deltaTime;
            float pidOutput = constantes.kp * error + constantes.ki * calculos->integral + constantes.kd * derivative;
            
            // Saturación de la salida y anti-windup
            if (pidOutput > constantes.maxOutput && constantes.maxOutput != 0) {
                pidOutput = constantes.maxOutput;
                // Ajustar integral solo si la salida está saturada
                if (constantes.ki != 0) {
                    calculos->integral = (pidOutput - constantes.kp * error - constantes.kd * derivative) / constantes.ki;
                }   
            } 
            
            if (pidOutput < constantes.minOutput && constantes.minOutput != 0) {
                pidOutput = constantes.minOutput; 
                if (constantes.ki != 0) {
                    calculos->integral = (pidOutput - constantes.kp * error - constantes.kd * derivative) / constantes.ki;
                }
            }
            
            calculos->previousFilteredError = filteredError;
            calculos->lastTime = currentTime;
            calculos->output = pidOutput;
            return true;
        } 
        return false;
    }
}

