#include "valvulaExpiratoria.h"
#include "../panavent.hardware.drivers.X/mcc_generated_files/pwm/sccp4.h"


PIDrequest vExpPidRequest;

ValvulaExpiratoria valvulaExpiratoria = {
    .inicializar = &ValvulaExpiratoria_inicializar,
    .abrir = &ValvulaExpiratoria_abrir,
    .cerrar = &ValvulaExpiratoria_cerrar,
    .controlPeep = &ValvulaExpiratoria_controlPeep,
    .setConstates = &ValvulaExpiratoria_setConstantesPID
};

void ValvulaExpiratoria_inicializar(){
    PWM4.PWM_Initialize(); 
    PWM4.PWM_Enable();
    PWM4.PWM_DutyCycleSet(0);
}

void ValvulaExpiratoria_abrir() {
    PWM4.PWM_DutyCycleSet(0);
};

void ValvulaExpiratoria_cerrar() {
    PWM4.PWM_DutyCycleSet(FULL_PERIOD_PWM_VAL);
};

void ValvulaExpiratoria_controlPeep(float presion) {
    if (pid.calcular(&vExpPidRequest)) {
        PWM4.PWM_DutyCycleSet(vExpPidRequest.calculos.output * FULL_PERIOD_PWM_VAL);
    }
};

void ValvulaExpiratoria_setConstantesPID(PIDconstantes constantes) {
    vExpPidRequest.constantes = constantes;
}