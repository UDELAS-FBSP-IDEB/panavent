#ifndef VALVULA_EXPIRATORIA_H
#define	VALVULA_EXPIRATORIA_H
#include "../panavent.utilities.X/pid.h"

typedef struct { 
    void (*abrir)();
    void (*cerrar)(); 
    void (*controlPeep)(float presion);
    void (*setConstates)(PIDconstantes constantes);
    void (*inicializar)();
} ValvulaExpiratoria;

extern   ValvulaExpiratoria valvulaExpiratoria;

void ValvulaExpiratoria_inicializar();
void ValvulaExpiratoria_abrir();
void ValvulaExpiratoria_cerrar();
void ValvulaExpiratoria_controlPeep(float presion);
void ValvulaExpiratoria_setConstantesPID(PIDconstantes constantes);
 
#endif