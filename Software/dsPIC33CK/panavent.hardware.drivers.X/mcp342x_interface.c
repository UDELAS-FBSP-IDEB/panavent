#include "mcp342x_interface.h"
#include "mcc_generated_files/i2c_host/i2c1.h"  

 
 
bool Mcp342x_dispararConvercion(uint8_t addr, uint8_t ch){    
    uint8_t configReg = MCP342X_CONFIG_REG;    
    switch (ch){
        case 0x01:
            CLEARBIT(configReg, 5);
            CLEARBIT(configReg, 6);
            break;
        case 0x02:
            SETBIT(configReg, 5);
            CLEARBIT(configReg, 6);
            break;    
        case 0x03:
            CLEARBIT(configReg, 5);
            SETBIT(configReg, 6);
            break; 
        case 0x04:
            SETBIT(configReg, 5);
            SETBIT(configReg, 6);
            break; 
    }     
    SETBIT(configReg, 7);    
    if (I2CMaster.Write(addr,  &configReg, 1)){
       while(I2CMaster.IsBusy()){}  
        if(I2CMaster.ErrorGet()==I2C_HOST_ERROR_NONE){
            return true;
        }  
    }
    return false;    
};

bool Mcp342x_leerConversion(uint8_t addr, uint8_t ch, float *valor){
    uint8_t resData[3];
    ch = ch -1;
    if(I2CMaster.Read(addr, resData, 3)){
      while(I2CMaster.IsBusy()){}  
       enum I2C_HOST_ERROR err = I2CMaster.ErrorGet();
       if(err==I2C_HOST_ERROR_NONE){
            if(!CHECKBIT(resData[2],7) && ((resData[2] >> 5) & ch)==ch ){ 
                uint16_t OutputCode = (((uint16_t)resData[0] << 8) | resData[1]) & 0x3FFF;
                float res = (float)OutputCode*MCP342X_LSB;
                *valor = res; 
                return true; 
            }
       } 
    }
    return false;
};