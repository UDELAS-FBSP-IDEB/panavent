#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include <stdint.h> 
#include "comun.h"
#include "../panavent.utilities.X/utilities.h"
#include "../panavent.utilities.X/pid.h"

typedef enum {
    APP_CMD_NONE = 0,
    APP_CMD_PROTOCOLO_ERROR_CODE = 1,
    APP_CMD_PROTOCOLO_PING = 2,
    APP_CMD_PROTOCOLO_ACK = 3,
    APP_CMD_PROTOCOLO_NACK = 4,
    APP_CMD_VENTILACION_PARAMETROS = 5,
    APP_CMD_VENTILACION_DATA = 6,
    APP_CMD_PROCESOS_INSTRUCCION = 7,
    APP_CMD_PROCESOS_CALIBRACION = 8,
    APP_CMD_PROCESOS_CONSTANTES_PID = 9,
    APP_CMD_PROCESOS_GET_CALIBRACION = 10,
    APP_CMD_PROCESOS_GET_CONSTANTES_PID = 11,
    APP_CMD_SINGLE_DATA = 12,
    APP_CMD_PROCESOS_SET_PWM = 13
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
    VENTILACION_MVC = 1,
    VENTILACION_MPC = 2
} Ventilacion_Modo;

typedef enum {
    VENTILACION_SQR = 1,
    VENTILACION_SIN = 2,
    VENTILACION_DES = 3
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
} AppVentilacionParams;

typedef struct {
    uint8_t ciclo;
    uint8_t fase; 
    float flujo;
    float flujoProximal;
    float tempFlujo;
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
} AppVentilacionData;

typedef struct {
    uint8_t indice;
    float valor;
    float m;
    float b;
} AppCalibracionData;

typedef struct {
    uint8_t indice;
    PIDconstantes constantes;
} AppConstantesPID;

typedef struct {
    bool oxigeno;
    float porcentaje;
} AppPWMData;

typedef void (*AppOnVentilacionParams)(const AppVentilacionParams *args);
typedef void (*AppOnConstantesPID)(const AppConstantesPID *args);
typedef void (*AppOnCalibracionData)(const AppCalibracionData *args);
typedef void (*AppOnPWMData)(const AppPWMData *args);
typedef void (*AppOnInstruccion)(uint8_t instruccion);

typedef struct {
    AppOnVentilacionParams onVentilacionParams;
    AppOnConstantesPID onConstantesPID;
    AppOnCalibracionData onCalibracionData;
    AppOnPWMData onPWMData;
    AppOnInstruccion onInstruccion;
    bool (*sendVentilacionData)(AppVentilacionData data);  
    bool (*sendData)(float data);  
    bool (*sendError)(ErrorCode e);  
    void (*inicializar)(void);
} App;

extern App app;

void App_inicializar(void);
bool App_sendVentilacionData(AppVentilacionData data);
bool App_sendData(float data);
bool App_sendError(ErrorCode e);

#endif