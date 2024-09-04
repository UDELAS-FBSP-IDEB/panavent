#ifndef MONPS_H
#define	MONPS_H
#include <stdbool.h> 
#include "comun.h" 
#include "../panavent.hardware.drivers.X/pca9543apw_interface.h"
#include "../panavent.hardware.drivers.X/mcc_generated_files/i2c_host/i2c1.h"
 

typedef struct { 
    uint8_t addr;
    float nivelBateria;
    bool enSuministro;
    bool (*cerrarValvulaSeguridad)(bool acc);
    bool (*getInfoCarga)();
} MonitorPowerSuply;

extern   MonitorPowerSuply monitorPowerSuply;

bool MonitorPowerSupply_AccionarValvulaSeguridad(bool acc);
bool MonitorPowerSupply_GetInfoCarga();

#endif	