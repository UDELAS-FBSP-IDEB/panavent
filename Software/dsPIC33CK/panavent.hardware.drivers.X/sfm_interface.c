#include "sfm_interface.h" 
#define FCY 30000000
#include <libpic30.h> 
#include "mcc_generated_files/i2c_host/i2c1.h" 
#include "pca9543apw_interface.h"
 

Sfm sfmOxi = {
    .sfmType = SFM_4200,
    .inicializar = SfmOxi_Inicializar,
    .leerFlujo = SfmOxi_LeerFlujo
};
Sfm sfmAire = {
    .sfmType = SFM_4200,
    .inicializar = SfmAire_Inicializar,
    .leerFlujo = SfmAire_LeerFlujo
};
Sfm sfmProxi = {
     .sfmType = SFM_3200,
    .inicializar = SfmProxi_Inicializar,
    .leerFlujo = SfmProxi_LeerFlujo
};

bool SFM4200_Inicializar(Sfm * sfm){  
    sfm->flow = 0;
    sfm->temp = 0;  
    uint8_t data[2];
    if(SFM4200_GetParametro(SFM4200_READ_SERIAL_NUMBER_U, data,0)){
        sfm->serialNo = ((((uint32_t)data[0] << 8) | data[1]) << 16);
    }else{
        return false;
    }
    if(SFM4200_GetParametro(SFM4200_READ_SERIAL_NUMBER_L, data,0)){
        sfm->serialNo |= (((uint32_t)data[0] << 8) | data[1]);
    }else{
        return false;
    }
    if(SFM4200_GetParametro(SFM4200_FLOW_SCALE_FACTOR, data,0)){
        sfm->flowScale = (((uint16_t)data[0] << 8) | data[1]);
    }else{
        return false;
    }
    if(SFM4200_GetParametro(SFM4200_FLOW_OFFSET, data,0)){
        sfm->flowOffset = (((uint16_t)data[0] << 8) | data[1]);
    }else{
        return false;
    }
    if(SFM4200_GetParametro(SFM4200_TEMP_SCALE_FACTOR, data,0)){
        sfm->tempScale = (((uint16_t)data[0] << 8) | data[1]);
    }else{
        return false;
    }
    if(SFM4200_GetParametro(SFM4200_TEMP_OFFSET, data,0)){
        sfm->tempOffset = (((uint16_t)data[0] << 8) | data[1]);
    } else{
        return false;
    }    
    SFM4200_SetModo(sfm,FLOW);
    return  true; 
}; 

void SFM4200_Deinicializar(Sfm * sfm){ 
    if(sfm!=NULL){ 
        sfm = NULL; 
    }
};

bool SFM4200_SetModo(Sfm *sfm, SfmModo modo){ 
    uint16_t cmd;
    sfm->modo = modo;
    switch(sfm->modo)
    {
        case TEMP:
            cmd = SFM4200_READ_TEMP_REGISTER;
            break;
        case FLOW:
            cmd = SFM4200_READ_FLOW_REGISTER;
            break;
    }
    if (SFM4200_Write(cmd)){
        __delay_ms(100);
        return true;
    }else{
        return false;
    }
}

bool SFM4200_LeerFlujo(Sfm *sfm){ 
    if(sfm!=NULL){  
        if(sfm->modo==FLOW){
            uint8_t resData[2];
            if (SFM4200_Read(resData)){
                uint16_t val = ((uint16_t)resData[0] << 8) | resData[1];
                sfm->flow= (val - sfm->flowOffset)/(sfm->flowScale);;
                return true;  
            }                    
        }  
    } 
    return false;
}

bool SFM4200_Write(uint16_t comando){  
    uint8_t cmdData[2];
    cmdData[0] = (uint8_t)((comando & 0xFF00) >> 8);
    cmdData[1] = (uint8_t)((comando & 0x00FF) >> 0);
    if (I2CMaster.Write(SFM4200_I2C_ADDRESS,  cmdData, 2)){
       while(I2CMaster.IsBusy()){}  
        return true;
    }
    return false;
}

bool SFM4200_Read(uint8_t * data){ 
    uint8_t resData[3]={0,0,0};
    enum I2C_HOST_ERROR err = I2CMaster.ErrorGet();
    if(err==I2C_HOST_ERROR_NONE){
         if(I2CMaster.Read(SFM4200_I2C_ADDRESS, resData, 3)){
           while(I2CMaster.IsBusy()){} 
            err = I2CMaster.ErrorGet();
            if(err==I2C_HOST_ERROR_NONE){
                if(SFM4200_ValidarCheckSum(resData)){
                    data[0] = resData[0];
                    data[1] = resData[1];
                    return true;
                }
            } 
        }   
    }
    return false;
}

bool SFM4200_GetParametro(uint16_t comando, uint8_t * data, uint16_t delay_us){ 
    enum I2C_HOST_ERROR err;
    uint8_t cmdData[2];
    cmdData[0] = (uint8_t)((comando & 0xFF00) >> 8);
    cmdData[1] = (uint8_t)((comando & 0x00FF) >> 0); 
    uint8_t resData[3]; 
    if (I2CMaster.Write(SFM4200_I2C_ADDRESS,  cmdData, 2)){
       while(I2CMaster.IsBusy()){}  
        if(I2CMaster.ErrorGet()==I2C_HOST_ERROR_NONE){
           if(delay_us>0) __delay_ms(delay_us);
           if(I2CMaster.Read(SFM4200_I2C_ADDRESS, resData, 3)){
              while(I2CMaster.IsBusy()){} 
               err = I2CMaster.ErrorGet();
               if(err==I2C_HOST_ERROR_NONE){
                   if(SFM4200_ValidarCheckSum(resData)){
                       data[0] = resData[0];
                       data[1] = resData[1];
                       return true;
                   }
               } 
           }
        }  
    }
    return false;
}

bool SFM4200_ValidarCheckSum(const uint8_t * data){ 
    uint8_t revCheckSum= data[2];
    uint8_t CalcCheckSum= SFM4200_GenerarCheckSum(data);
    return (CalcCheckSum==revCheckSum);
}

uint8_t SFM4200_GenerarCheckSum(const uint8_t* data){
    uint8_t crc = 0;
    uint8_t crc_bit; 
    uint16_t ix;
    for (ix = 0; ix < 2; ++ix) {
        crc ^= (data[ix]);
        for (crc_bit = 8; crc_bit > 0; --crc_bit) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}


bool SfmAire_Inicializar() {
    return PCA9543APW_setCanal(0x70, 1) && SFM4200_Inicializar(&sfmAire);
}

bool SfmOxi_Inicializar() {
    return PCA9543APW_setCanal(0x70, 2) && SFM4200_Inicializar(&sfmOxi);
}

bool SfmProxi_Inicializar() {
    return PCA9543APW_setCanal(0x71, 1) && SFM4200_Inicializar(&sfmProxi);
}

bool SfmAire_LeerFlujo() {
     return PCA9543APW_setCanal(0x70, 1) && SFM4200_LeerFlujo(&sfmAire);
}

bool SfmOxi_LeerFlujo() {
     return PCA9543APW_setCanal(0x70, 2) && SFM4200_LeerFlujo(&sfmOxi);
}

bool SfmProxi_LeerFlujo() { 
    if(PCA9543APW_setCanal(0x71, 1)){
        SFM4200_LeerFlujo(&sfmProxi);
        if (sfmProxi.sfmType == SFM_3200) {
            sfmProxi.flow = -1 * sfmProxi.flow;
        }
        return true;
    }else{
        return false;
    } 
}

