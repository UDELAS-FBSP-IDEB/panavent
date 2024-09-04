#ifndef APP_H
#define	APP_H 
#include <stdbool.h>
#include <stdint.h>
#include "comun.h"
#include "../panavent.utilities.X/pid.h"
#include "../panavent.utilities.X/utilities.h"

#define SOH 0x01
#define STX 0x02
#define ETX 0x03  

typedef enum {    
    APP_CMD_NONE=0,
    APP_CMD_PROTOCOLO_ERROR_CODE=1,
    APP_CMD_PROTOCOLO_PING=2,
    APP_CMD_PROTOCOLO_ACK = 3,
    APP_CMD_PROTOCOLO_NACK=4, 
    APP_CMD_VENTILACION_PARAMETROS=5, 
    APP_CMD_VENTILACION_DATA=6,
    APP_CMD_PROCESOS_INSTRUCCION = 7,
    APP_CMD_PROCESOS_CALIBRACION = 8,
    APP_CMD_PROCESOS_CONSTANTES_PID=9,
    APP_CMD_PROCESOS_GET_CALIBRACION = 10,
    APP_CMD_PROCESOS_GET_CONSTANTES_PID=11,
             APP_CMD_SINGLE_DATA = 12
} AppCommandType;
 
typedef enum {
    IE11,
    IE12,
    IE13,
    IE14,
    IE21,
    IE31,
    IE41
} Ventilacion_IE;

typedef enum {
    VENTILACION_MVC=1,
    VENTILACION_MPC =2
} Ventilacion_Modo;

typedef enum {
    VENTILACION_SQR=1,
    VENTILACION_SIN =2,
    VENTILACION_DES =3
} Ventilacion_FormaOnda;

typedef struct {
    Ventilacion_Modo modo;
    Ventilacion_FormaOnda formaOnda;
    float volumen;
    float flujo;
    float fiO2;
    float fr;
    Ventilacion_IE ie;
    float presion;
    float peep;
    float pi;
    float tt;
    float ti;
    float te; 
    float qi;
    float fi; 
} AppCommand_Ventilacion_Parametros;

typedef struct {
    uint8_t ciclo;
    uint8_t fase;
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
    float nivelBateria;
    float deltaT;
    bool enSuministro;
} AppCommand_Ventilacion_Data;

typedef struct {
    uint8_t indice;
    float valor; 
    float m;
    float b;
} AppCommand_Procesos_Calibracion;

typedef struct {
    uint8_t    indice;
    PIDconstantes constantes;  
} AppCommand_Procesos_ConstantesPID;


typedef struct {
    AppCommand_Procesos_ConstantesPID    constantesPID;
    AppCommand_Procesos_Calibracion calibracion;  
} AppCommand_Procesos;

typedef struct {
    AppCommand_Ventilacion_Parametros    parametros;
    AppCommand_Ventilacion_Data data;  
} AppCommand_Ventilacion;


typedef struct {
    AppCommandType command;
    uint16_t sequenceNumber;
} AppCommand_Received;


typedef struct {  
    uint8_t addr;
    uint8_t instruccion;    
    AppCommand_Received received;    
    AppCommand_Procesos procesos;
    AppCommand_Ventilacion ventilacion;    
    bool (*sendCommandAndWait)(AppCommandType cmd, AppCommandType waitCmd);
    bool (*getCommands)(); 
    bool (*getCommand)(bool waitForCmd, AppCommandType waitCmdType, uint16_t waitCmdSequenceNumber);  
    bool (*sendCommand)(AppCommandType cmd);
    bool (*sendCommandAndWaitWithArgs)(AppCommandType cmd, AppCommandType waitCmd, uint8_t * command, uint8_t len);
    bool (*sendError)(ErrorCode e);
    void (*inicializar)();
} App;

extern App app;

bool App_SendCommandAndWait(AppCommandType cmd, AppCommandType waitCmd);
bool App_getCommands(); 
bool App_getCommand(bool waitForCmd, AppCommandType waitCmdType, uint16_t waitCmdSequenceNumber); 
void App_inicializar(); 
void App_TimeOutTimerCallBack();
bool App_sendCommand(AppCommandType cmd);
bool App_sendCommandWithArgs(AppCommandType cmd, uint8_t * command, uint8_t len);
bool App_sendCommandAndWaitWithArgs(AppCommandType cmd, AppCommandType waitCmd, uint8_t * command, uint8_t len);
bool App_sendError(ErrorCode e);
    
#endif	