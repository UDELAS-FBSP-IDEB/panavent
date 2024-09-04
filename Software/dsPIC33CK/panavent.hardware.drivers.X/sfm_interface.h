#ifndef SFM_H
#define SFM_H
#include <xc.h>  
#include <stdbool.h>  




#define SFM4200_I2C_ADDRESS                     0x40
#define SFM4200_FLOW_SCALE_FACTOR               0x30DE
#define SFM4200_FLOW_OFFSET                     0x30DF
#define SFM4200_TEMP_SCALE_FACTOR               0x31AC
#define SFM4200_TEMP_OFFSET                     0x31AD
#define SFM4200_READ_FLOW_REGISTER              0x1000
#define SFM4200_READ_TEMP_REGISTER              0x1001
#define SFM4200_READ_STATUS_REGISTER            0x1010
#define SFM4200_READ_SERIAL_NUMBER_U            0x31AE
#define SFM4200_READ_SERIAL_NUMBER_L            0x31AF

typedef enum{
    SFM_4200, 
    SFM_3200, 
    SFM_3300 
} SfmType;
  
typedef enum { 
    TEMP,
    FLOW
} SfmModo;
 
typedef struct {  
    uint8_t     id;
    uint8_t     canal; 
    uint32_t    serialNo;
    float       flowOffset;
    float       flowScale;
    uint16_t    tempOffset;
    uint16_t    tempScale;
    float       temp;
    float       flow;
    SfmModo     modo;  
    SfmType     sfmType;    
    bool        (*inicializar)(); 
    bool        (*leerFlujo)(); 
} Sfm;

extern  Sfm sfmOxi;
extern  Sfm sfmAire;
extern  Sfm sfmProxi;
 
bool SFM4200_LeerFlujo(Sfm * sfm); 
bool SFM4200_Inicializar(Sfm * sfm); 
bool SFM4200_GetParametro(uint16_t comando, uint8_t * data, uint16_t delay_us); 
bool SFM4200_ValidarCheckSum(const uint8_t * data);
bool SFM4200_SetModo(Sfm *sfm, SfmModo modo);
bool SFM4200_Write(uint16_t comando);
bool SFM4200_Read(uint8_t * data);
uint8_t SFM4200_GenerarCheckSum(const uint8_t * data); 
void SFM4200_Deinicializar(Sfm * sfm);
 
bool SfmAire_LeerFlujo();
bool SfmAire_Inicializar();
bool SfmOxi_LeerFlujo();
bool SfmOxi_Inicializar();
bool SfmProxi_LeerFlujo();
bool SfmProxi_Inicializar();

#endif