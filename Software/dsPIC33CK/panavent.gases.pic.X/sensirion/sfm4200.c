#include "sfm.h"

#define STACK_SIZE 3
sfm sfm4200Stack[STACK_SIZE];
int  stackIx=0;
//bool settingModo =false;

//        sfm->LeerFlujo = &SFM4200LeerFlujo;
//        sfm->LeerTemperatura = &SFM4200LeerTemperatura;

sfm * SFM4200Inicializar(uint8_t id){
    if( stackIx <STACK_SIZE){ 
        sfm *sfm = &sfm4200Stack[stackIx];  
        sfm->SetModo = &SFM4200SetModo;
        sfm->id = id;  
        sfm->flow = 0;
        sfm->temp = 0;
        sfm->LeerFlujo = &SFM4200LeerFlujo; 
        stackIx++;
        uint8_t data[2];
        if(SFM4200GetParametro(SFM4200_READ_SERIAL_NUMBER_U, data,0)){
            sfm->serialNo = ((((uint32_t)data[0] << 8) | data[1]) << 16);
        }else{
            return NULL;
        }
        if(SFM4200GetParametro(SFM4200_READ_SERIAL_NUMBER_L, data,0)){
            sfm->serialNo |= (((uint32_t)data[0] << 8) | data[1]);
        }else{
            return NULL;
        }
        if(SFM4200GetParametro(SFM4200_FLOW_SCALE_FACTOR, data,0)){
            sfm->flowScale = (((uint16_t)data[0] << 8) | data[1]);
        }else{
            return NULL;
        }
        if(SFM4200GetParametro(SFM4200_FLOW_OFFSET, data,0)){
            sfm->flowOffset = (((uint16_t)data[0] << 8) | data[1]);
        }else{
            return NULL;
        }
        if(SFM4200GetParametro(SFM4200_TEMP_SCALE_FACTOR, data,0)){
            sfm->tempScale = (((uint16_t)data[0] << 8) | data[1]);
        }else{
            return NULL;
        }
        if(SFM4200GetParametro(SFM4200_TEMP_OFFSET, data,0)){
            sfm->tempOffset = (((uint16_t)data[0] << 8) | data[1]);
        } else{
            return NULL;
        }    
        SFM4200SetModo(id,FLOW);
        return  sfm; 
    } else{
        return NULL;
    }
}; 



void SFM4200Deinicializar(int id){
    sfm *sfm = SFM4200Get(id);
    if(sfm!=NULL){ 
        sfm = NULL;
        stackIx--;
    }
};

bool SFM4200SetModo(int id, enum SFM4200_MODO modo){
    sfm *sfm = SFM4200Get(id); 
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
    if (SFM4200Write(cmd)){
        __delay_ms(100);
        return true;
    }else{
        return false;
    }
}

float SFM4200LeerFlujo(int id){
    sfm *sfm = SFM4200Get(id);
    if(sfm!=NULL){  
        if(sfm->modo==FLOW){
            uint8_t resData[2];
            if (SFM4200Read(resData)){
                uint16_t val = ((uint16_t)resData[0] << 8) | resData[1];
                sfm->flow= (val - sfm->flowOffset)/(sfm->flowScale);;
                return (sfm->flow);  
            }                    
        }  
    } 
    return -1;
}

bool SFM4200Write(uint16_t comando){  
    uint8_t cmdData[2];
    cmdData[0] = (uint8_t)((comando & 0xFF00) >> 8);
    cmdData[1] = (uint8_t)((comando & 0x00FF) >> 0);
    if (I2CMaster.Write(SFM4200_I2C_ADDRESS,  cmdData, 2)){
        while(I2CMaster.IsBusy()){};  
        return true;
    }
    return false;
}

bool SFM4200Read(uint8_t * data){ 
    uint8_t resData[3]={0,0,0};
    enum I2C_HOST_ERROR err = I2CMaster.ErrorGet();
    if(err==I2C_HOST_ERROR_NONE){
         if(I2CMaster.Read(SFM4200_I2C_ADDRESS, resData, 3)){
            while(I2CMaster.IsBusy()){}; 
            err = I2CMaster.ErrorGet();
            if(err==I2C_HOST_ERROR_NONE){
                if(SFM4200ValidarCheckSum(resData)){
                    data[0] = resData[0];
                    data[1] = resData[1];
                    return true;
                }
            } 
        }   
    }
    return false;
}

bool SFM4200GetParametro(uint16_t comando, uint8_t * data, uint16_t delay_us){ 
    enum I2C_HOST_ERROR err;
    uint8_t cmdData[2];
    cmdData[0] = (uint8_t)((comando & 0xFF00) >> 8);
    cmdData[1] = (uint8_t)((comando & 0x00FF) >> 0); 
    uint8_t resData[3]; 
    if (I2CMaster.Write(SFM4200_I2C_ADDRESS,  cmdData, 2)){
        while(I2CMaster.IsBusy()){};  
        if(I2CMaster.ErrorGet()==I2C_HOST_ERROR_NONE){
           if(delay_us>0) __delay_ms(delay_us);
           if(I2CMaster.Read(SFM4200_I2C_ADDRESS, resData, 3)){
               while(I2CMaster.IsBusy()){}; 
               err = I2CMaster.ErrorGet();
               if(err==I2C_HOST_ERROR_NONE){
                   if(SFM4200ValidarCheckSum(resData)){
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

bool SFM4200ValidarCheckSum(const uint8_t * data){ 
    uint8_t revCheckSum= data[2];
    uint8_t CalcCheckSum= SFM4200GenerarCheckSum(data);
    return (CalcCheckSum==revCheckSum);
}

uint8_t SFM4200GenerarCheckSum(const uint8_t* data){
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

sfm * SFM4200Get(int id){
    uint8_t i = 0;
    for(i=0; i < STACK_SIZE; i++)
    {
        if(sfm4200Stack[i].id ==    id) {    
            return &sfm4200Stack[i];
        }
    }   
    return NULL;
}
