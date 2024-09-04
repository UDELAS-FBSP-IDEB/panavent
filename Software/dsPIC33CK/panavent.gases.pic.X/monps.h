#ifndef MONPS_H
#define	MONPS_H
#include "mcc_generated_files/i2c_host/i2c1.h"  
#include "protocolo.h" 
#define MONPS_ADDR 0x75 

typedef struct { 
    bool enSuministro;
    float nivelCarga;
} infoCarga;

typedef struct { 
    bool (*accionarValvulaSeguridad)(bool acc);
    bool (*getInfoCarga)(infoCarga * info);
} monps;

monps * inicializarMonps();
bool accionarValvulaSeguridad(bool acc);
bool getInfoCarga(infoCarga * info);

#endif	