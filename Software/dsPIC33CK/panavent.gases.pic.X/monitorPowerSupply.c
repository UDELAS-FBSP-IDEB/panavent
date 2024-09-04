#include "monitorPowerSupply.h"
#include "../panavent.utilities.X/utilities.h"

bool valvulaAccionada = false;

MonitorPowerSuply monitorPowerSuply = {
    .addr = 0x75,
    .cerrarValvulaSeguridad = &MonitorPowerSupply_AccionarValvulaSeguridad,
    .getInfoCarga = &MonitorPowerSupply_GetInfoCarga
};

bool MonitorPowerSupply_AccionarValvulaSeguridad(bool acc) {
    if(valvulaAccionada!=acc){
        uint8_t buffer[2];
        buffer[0] = acc ? 1 : 0;
        buffer[1] = GenerarCheckSum(buffer, 1);
        if (PCA9543APW_setCanal(0x71, 1)) {
            if (I2CMaster.Write(monitorPowerSuply.addr, buffer, 2)) {
                while (I2CMaster.IsBusy()) {
                }
                if (I2CMaster.ErrorGet() == I2C_HOST_ERROR_NONE) {
                    valvulaAccionada=acc;
                    return true;
                }
            }
        }
        
    }
    return false;
};

bool MonitorPowerSupply_GetInfoCarga() {
    if (PCA9543APW_setCanal(0x71, 1)) {
        uint8_t buffer[6];

        if (I2CMaster.Read(monitorPowerSuply.addr, buffer, 6)) {
                while (I2CMaster.IsBusy()) {
                }
                enum I2C_HOST_ERROR err = I2CMaster.ErrorGet();
                if (err == I2C_HOST_ERROR_NONE) {
                    if (ValidarCheckSum(buffer[5], buffer, 5)) {
                        int i = 0;
                                monitorPowerSuply.nivelBateria = UnpackFloat(&buffer[i], &i);
                                monitorPowerSuply.enSuministro = buffer[i] == 1 ? true : false;
                        return true;
                    }
                }
            }
    }
    return false;
};