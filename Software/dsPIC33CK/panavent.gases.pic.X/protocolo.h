
#ifndef PROTOCOLO_H
#define	PROTOCOLO_H

#include "mcc_generated_files/i2c_host/i2c1.h"  

#define CHECKBIT(B, BIT)             ((B >> BIT) & 1)==1
#define CLEARBIT(B,BIT)              B &= ~(1 << BIT)
#define SETBIT(B,BIT)                B |= 1 << BIT
#define TOGGLE(B,BIT)                B ^= 1 << BIT

/*      X       X       XXX         XXX    */
/*      ON      RESET   SUB-MODO    MODO   */



#define _MODO_TIPO(B)                    (B & 0x38) >> 3
#define _RESET_MODO(B)                   (B & 0xC7)  

/*MODO*/
#define _MVC(B)                         (B & 0x07) == 0x01 
#define _MPC(B)                         (B & 0x07) == 0x02   
#define _MSERV(B)                       (B & 0x07) == 0x03   
#define _IS_MVENT(B)                    ((_MVC(B)) || (_MPC(B))) 
#define _IS_MSERV(B)                    _MSERV(B)  

/*MODOS DE VOLUMEN*/
#define _MVCSQR(B)                      _MODO_TIPO(B) == 0x01   
#define _MVCSIN(B)                      _MODO_TIPO(B) == 0x02   
#define _MVCDES(B)                      _MODO_TIPO(B) == 0x03 

/*MODOS DE PRESION*/
#define _MPCDEF(B)                      _MODO_TIPO(B) == 0x01

/*MODOS DE SERVICIO*/
#define _KPID(B)                        _MODO_TIPO(B) == 0x01
#define _ADJ(B)                         _MODO_TIPO(B) == 0x02 
#define _CAL(B)                         _MODO_TIPO(B) == 0x03 

#define _ISACK(B)                       (B==0xFF)
#define _ISOFF(B)                       (B==0x00)
#define _ON(B)                          (!_ISACK(B) && CHECKBIT(B,7) && (_IS_MVENT(B) || _IS_MSERV(B))) 
#define _CLEAR_RESET(B)                 CLEARBIT(B,6) 
#define _RESET(B)                       (!_ISACK(B) && CHECKBIT(B,6) && (_IS_MVENT(B) || _IS_MSERV(B))) 
 

typedef enum 
{
    SFM_O2_OK=1,
    SFM_AIRE_OK=2,
    SFM_PROX_OK=3,
    MCP3426_OK=4,
    MCP3428_OK=5,
    VALVE_AIRE_OK=6,
    CANAL_PCA9543APW_OK = 7,
    DIS_PCA9543APW_OK = 8,
    MCPMODULO1_OK = 9,
    MCPMODULO2_OK = 10,
    VALVE_O2_OK=11,
    VALVE_EXP_OK=12,  
    INIT_OK=13, 
    MOMPS_OK = 14,
    SFM_O2_ERROR=128,    
    SFM_AIRE_ERROR=129,    
    SFM_PROX_ERROR=130,    
    MCP3426_ERROR=131,    
    MCP3428_ERROR=132,    
    VALVE_AIRE_ERROR=133,
    INIT_ERROR=134,    
    CANAL_PCA9543APW_ERROR = 135,   
    DIS_PCA9543APW_ERROR = 136,   
    MCPMODULO1_ERROR = 137,   
    MCPMODULO2_ERROR = 138,    
    VALVE_O2_ERROR=139,    
    VALVE_EXP_ERROR=140,
    RESETING=141,
    MOMPS_ERROR = 142
            
} ErrorCode;

enum IE {
    IE11,
    IE12,
    IE13,
    IE14,
    IE21,
    IE31,
    IE41
};

typedef enum TIPO_PARAM {
    P_FA=0,
    P_FO=1,
    P_PS=2,
    P_FLUJO_PROXIMAL=3,
    P_VI=4,
    P_PEEP=5,
    P_VALVE_AIRE=6,
    P_VALVE_OXI=7,
    P_EXP=8
} TipoParam;
 

typedef struct {
    float volumen;
    float flujo; 
    float fiO2;
    float fr;
    enum IE ie;
    float presion;
    float peep;
    float pi;
    float tt;
    float ti;
    float te;
    
    float kp;
    float ki;
    float kd;
    float maxOutput;
    float minOutput;
    float emaAlpha;
    
    float qi;
    float fi;
    
    float   param;
    TipoParam tipoParam;
    
    float m;
    float b;
    
} Parametros;

typedef struct {
    int8_t ciclo;
    int8_t fase;
    float flujo;
    float tempFlujo; //Temperatura del flujo
    float fiO2;
    float presionInsp;
    float presionExp;
    float presionAire;
    float presionOxigeno;
    float flujoAire;
    float flujoOxigeno;
    float tempAire;
    float tempOxigeno;
    float volumen;
    float vte;
    float vti;
    float bateria;
    float deltaT;
    uint8_t suministro;
} Mediciones;

int PackFloat(void *buf, float x);
float UnpackFloat(const void *buf, int *i);
unsigned char GenerarCheckSum(const unsigned char * data, unsigned char len);
unsigned char ValidarCheckSum(const unsigned char revCheckSum, const unsigned char * data, unsigned char len);
 
#endif	 

