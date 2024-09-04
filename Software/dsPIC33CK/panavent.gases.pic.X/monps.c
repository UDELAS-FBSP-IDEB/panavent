 #include "monps.h"

monps _monitorps;

monps * inicializarMonps(){
    monps *_m = &_monitorps;
    _monitorps.accionarValvulaSeguridad = &accionarValvulaSeguridad;
    _monitorps.getInfoCarga = &getInfoCarga;
    return _m;
}

bool accionarValvulaSeguridad(bool acc){ 
 
    uint8_t buffer[2];
    buffer[0] = acc? 1 : 0;
    buffer[1] = GenerarCheckSum(buffer,1);
    if (I2CMaster.Write(MONPS_ADDR,  buffer, 2)){
        while(I2CMaster.IsBusy()){};  
        if(I2CMaster.ErrorGet()==I2C_HOST_ERROR_NONE){
            return true;
        }  
    }
    return false;
};
bool getInfoCarga(infoCarga * info){
    uint8_t buffer[6];
    if(I2CMaster.Read(MONPS_ADDR, buffer, 6)){
       while(I2CMaster.IsBusy()){};  
       enum I2C_HOST_ERROR err = I2CMaster.ErrorGet();
       if(err==I2C_HOST_ERROR_NONE){
           if (ValidarCheckSum(buffer[5], buffer, 5)){   
                int i =0;
                info->nivelCarga =  UnpackFloat(&buffer[i], &i);
                info->enSuministro = buffer[i]==1? true : false;
                return true;
           } 
       } 
    }
    return false;
};