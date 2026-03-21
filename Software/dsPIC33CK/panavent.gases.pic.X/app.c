#include "app.h"
#include "comm.h" 
#include <libpic30.h>


static void App_CommOnCommandReceived(uint8_t command, const uint8_t *payload, uint8_t payloadLen);

App app = {
    .onVentilacionParams = NULL,
    .onConstantesPID = NULL,
    .onCalibracionData = NULL,
    .onPWMData = NULL,
    .onInstruccion = NULL,
    .inicializar = &App_inicializar,
    .sendVentilacionData = &App_sendVentilacionData,
    .sendData = &App_sendData,
    .sendError = &App_sendError
};

void App_inicializar(void) {
    comm.inicializar(&App_CommOnCommandReceived);
}

bool App_sendData(float data) {
    uint8_t buffer[4];
    PackFloat(&buffer, data);
    return comm.sendCommandWithArgs(APP_CMD_SINGLE_DATA, buffer, 4);
}

bool App_sendVentilacionData(AppVentilacionData data) {
    uint8_t buffer[51];
    PackFloat(&buffer, data.flujo);
    PackFloat(&buffer[4], data.presionInsp);
    PackFloat(&buffer[8], data.fiO2);
    PackFloat(&buffer[12], data.flujoAire);
    PackFloat(&buffer[16], data.flujoOxigeno);
    PackFloat(&buffer[20], data.presionAire);
    PackFloat(&buffer[24], data.presionOxigeno);
    PackFloat(&buffer[28], data.nivelBateria);
    PackFloat(&buffer[32], data.volumen);
    PackFloat(&buffer[36], data.vti);
    PackFloat(&buffer[40], data.deltaT);
    PackFloat(&buffer[44], data.flujoProximal);
    buffer[48] = data.enSuministro ? 1 : 0;
    buffer[49] = data.fase;
    buffer[50] = data.ciclo;
    return comm.sendCommandWithArgs(APP_CMD_VENTILACION_DATA, buffer, 51);
}

bool App_sendError(ErrorCode e) {
    uint8_t args[] = {e};
    return comm.sendCommandWithArgs(APP_CMD_PROTOCOLO_ERROR_CODE, args, 1);
}

static void App_CommOnCommandReceived(uint8_t command, const uint8_t *payload, uint8_t payloadLen) {
    int i = 0;

    if (command == APP_CMD_PROCESOS_INSTRUCCION) {
        
        if (payloadLen < 1) return;
        
        if (app.onInstruccion != NULL) {
            app.onInstruccion(payload[i++]);
        }

    } else if (command == APP_CMD_VENTILACION_PARAMETROS) {
        
        if (payloadLen < 27) return; // 6 floats + 3 bytes
        
        AppVentilacionParams params;

        params.volumen = UnpackFloat(&payload[i], &i);
        params.fiO2 = UnpackFloat(&payload[i], &i);
        params.fr = UnpackFloat(&payload[i], &i);
        params.presion = UnpackFloat(&payload[i], &i);
        params.peep = UnpackFloat(&payload[i], &i);
        params.pi = UnpackFloat(&payload[i], &i);
        params.ie = (Ventilacion_IE)payload[i++];
        params.modo = (Ventilacion_Modo)payload[i++];
        params.formaOnda = (Ventilacion_FormaOnda)payload[i++];

        if (params.fr <= 0.0f) {
            return;
        } 
        
        float t = 1.0f / (params.fr / 60.0f);

        switch (params.ie) {
            case IE11: params.ti = t * 0.5f;  break;
            case IE12: params.ti = t * 0.33f; break;
            case IE13: params.ti = t * 0.25f; break;
            case IE14: params.ti = t * 0.20f; break;
            case IE21: params.ti = t * 0.66f; break;
            case IE31: params.ti = t * 0.75f; break;
            case IE41: params.ti = t * 0.80f; break;
            default:   params.ti = t * 0.33f; break;
        }

        params.te = t - params.ti;
        params.tt = params.te + params.ti;
         

        if (app.onVentilacionParams != NULL) {
            app.onVentilacionParams(&params);
        }

    } else if (command == APP_CMD_PROCESOS_CONSTANTES_PID) {
        
        if (payloadLen < 29) return; // 7 floats + 1 byte
        
        AppConstantesPID constPID;

        constPID.constantes.kp = UnpackFloat(&payload[i], &i);
        constPID.constantes.ki = UnpackFloat(&payload[i], &i);
        constPID.constantes.kd = UnpackFloat(&payload[i], &i);
        constPID.constantes.maxOutput = UnpackFloat(&payload[i], &i);
        constPID.constantes.minOutput = UnpackFloat(&payload[i], &i);
        constPID.constantes.emaAlpha = UnpackFloat(&payload[i], &i);
        constPID.constantes.testTarget = UnpackFloat(&payload[i], &i);
        constPID.indice = payload[i++];

        if (app.onConstantesPID != NULL) {
            app.onConstantesPID(&constPID);
        }

    } else if (command == APP_CMD_PROCESOS_SET_PWM) {
        
        if (payloadLen < 5) return; // 1 float + 1 byte
        
        AppPWMData pwmData;

        pwmData.porcentaje = UnpackFloat(&payload[i], &i);
        pwmData.oxigeno = (payload[i++] != 0);

        if (app.onPWMData != NULL) {
            app.onPWMData(&pwmData);
        }

    } else if (command == APP_CMD_PROCESOS_CALIBRACION) {
        
         if (payloadLen < 9) return; // 2 floats + 1 byte
         
        AppCalibracionData calData;

        calData.m = UnpackFloat(&payload[i], &i);
        calData.b = UnpackFloat(&payload[i], &i);
        calData.indice = payload[i++];
        calData.valor = 0.0f;

        if (app.onCalibracionData != NULL) {
            app.onCalibracionData(&calData);
        }
    }
}