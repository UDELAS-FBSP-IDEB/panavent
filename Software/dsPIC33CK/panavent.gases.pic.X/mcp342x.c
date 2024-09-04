 #include "mcp342x.h"

mcp342x mcp342xStack[MCP342X_STACK_SIZE];
int  mcp342xstackIx=0;

mcp342x * inicializarMcp342x(uint8_t addr, uint8_t numCanales){
    mcp342x *mcp = &mcp342xStack[mcp342xstackIx];
    mcp->addr=addr;
    mcp->numCanales = numCanales;
    mcp->LeerConversion = &leerConversion_mcp342x;
    mcp->dispararConversion = &dispararConvercion_mcp342x;
    uint8_t configReg = MCP342X_CONFIG_REG;
    if (I2CMaster.Write(addr,  &configReg, 1)){
        while(I2CMaster.IsBusy()){};  
        if(I2CMaster.ErrorGet()==I2C_HOST_ERROR_NONE){
            mcp342xstackIx++;
            return mcp;
        }  
    }
    return NULL;  
};

bool dispararConvercion_mcp342x(uint8_t addr, uint8_t ch){
    
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
        while(I2CMaster.IsBusy()){};  
        if(I2CMaster.ErrorGet()==I2C_HOST_ERROR_NONE){
            return true;
        }  
    }
    return false;
    
};

bool leerConversion_mcp342x(uint8_t addr, uint8_t ch, float *valor){
    uint8_t resData[3];
    ch = ch -1;
    if(I2CMaster.Read(addr, resData, 3)){
       while(I2CMaster.IsBusy()){};  
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