#ifndef SFM_H
#define SFM_H

#ifndef FCY
#define FCY 30000000
#endif
#include <libpic30.h>

#include "../mcc_generated_files/i2c_host/i2c1.h"
//#include "TimerEventDriver.h" 


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

typedef enum SFMTYPE_SENSOR{
    SFM_4200, 
    SFM_3200, 
    SFM_3300 
} SFMTYPE;
  
enum SFM4200_MODO { 
    TEMP,
    FLOW
};

typedef void (*SFM4200EventHandler)(float val);

typedef struct {  
    uint8_t     id;
    uint8_t     canal; 
    uint32_t    serialNo;
    float    flowOffset;
    float    flowScale;
    uint16_t    tempOffset;
    uint16_t    tempScale;
    float       temp;
    float       flow;
    enum SFM4200_MODO modo; 
    bool (*SetModo)(int id, enum SFM4200_MODO modo);
    float (*LeerFlujo)(int id); 
} sfm;



bool SFM4200SetModo(int id, enum SFM4200_MODO modo);
float SFM4200LeerFlujo(int id);
//float SFM4200LeerTemperatura(int id); 
sfm * SFM4200Inicializar(uint8_t id );
sfm * SFM4200Get(int id); 
bool SFM4200GetParametro(uint16_t comando, uint8_t * data, uint16_t delay_us); 
bool SFM4200ValidarCheckSum(const uint8_t * data);
uint8_t SFM4200GenerarCheckSum(const uint8_t * data);
//void SFM4200TimerEventHandler(int Id);
//void SFM4200Run(int id);
void SFM4200Deinicializar(int id);
bool SFM4200Write(uint16_t comando);
bool SFM4200Read(uint8_t * data);

#endif