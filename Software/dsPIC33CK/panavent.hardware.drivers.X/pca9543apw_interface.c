#include "pca9543apw_interface.h"
#include "mcc_generated_files/i2c_host/i2c1.h"  
  
uint8_t PCA9543APW_addrs[] = {0x70, 0x71};
uint8_t PCA9543APW_currentAddr =0;
uint8_t PCA9543APW_currentCh = 0;
 
bool PCA9543APW_setCanal(uint8_t addr, uint8_t canal){
    if (!(PCA9543APW_currentAddr == addr || PCA9543APW_currentCh == canal)){ 
        int k=0;
        for (k =0; k<2; k++){
            if (PCA9543APW_addrs[k] != addr){
                if (PCA9543APW_disable(PCA9543APW_addrs[k])==false){ 
                    return false;
                }
            }
        } 
        uint8_t wrData = canal;
        if (I2CMaster.Write(addr, &wrData, 1)){
           while(I2CMaster.IsBusy()){}  
            if(I2CMaster.ErrorGet()==I2C_HOST_ERROR_NONE){ 
                return true; 
            } 
        }   
       
        return false; 
    }else{
        return true;
    }
};
bool PCA9543APW_getCanal(uint8_t addr, uint8_t canal){
    enum I2C_HOST_ERROR err;
    uint8_t rdData = 0;
    if(I2CMaster.Read( addr, &rdData, 1)){
       while(I2CMaster.IsBusy()){} 
        err = I2CMaster.ErrorGet();
        if(err==I2C_HOST_ERROR_NONE && (rdData & 0x03)== canal ) return true; 
    }   
    return false;
};
bool PCA9543APW_disable(uint8_t addr){
    uint8_t wrData = 0x00;
       if (I2CMaster.Write(addr, &wrData, 1)){
          while(I2CMaster.IsBusy()){}  
           if(I2CMaster.ErrorGet()==I2C_HOST_ERROR_NONE){ 
               return true;
           }
       }  
    return false;
};