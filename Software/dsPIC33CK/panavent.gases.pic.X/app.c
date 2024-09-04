#include "app.h" 
#define FCY 60000000UL
#include <libpic30.h> 
#include "../panavent.hardware.drivers.X/mcc_generated_files/i2c_host/i2c1.h"
#include "../panavent.hardware.drivers.X/timer3Controller.h"

#define BUFFER_SIZE   50
#define DBUFFER_SIZE    BUFFER_SIZE*2
uint8_t tmpInputData[DBUFFER_SIZE];
#define APP_TIMEOUT_NUM 100
volatile unsigned int appWaitTimeOutCounter;
volatile bool waiting;
uint16_t sequenceNumber;
uint8_t tmpDataLen=0;

App app = { 
    .addr = 0x2E,
    .sendCommandAndWait = &App_SendCommandAndWait,
    .sendCommandAndWaitWithArgs = &App_sendCommandAndWaitWithArgs,
    .sendCommand = &App_sendCommand,
    .getCommand = &App_getCommand,
    .getCommands = &App_getCommands,
    .sendError = &App_sendError,
    .inicializar = &App_inicializar
};

void App_inicializar(){
    timer3Controller.inicializar();
    timer3Controller.agregarCallback(&App_TimeOutTimerCallBack,TIMER3_PERIOD);
}
void App_TimeOutTimerCallBack(){
    appWaitTimeOutCounter++;
}
bool App_sendCommandAndWaitWithArgs(AppCommandType cmd, AppCommandType waitCmd, uint8_t * command, uint8_t len) {
    
    if (App_sendCommandWithArgs(cmd, command, len)) {
        uint16_t cmdSequenceNumber = sequenceNumber-1;
        appWaitTimeOutCounter=0;
        while (appWaitTimeOutCounter < APP_TIMEOUT_NUM) {
            __delay_ms(20);
            if (app.getCommand(true,waitCmd,cmdSequenceNumber)){
                return true;
            };
            //if (app.received.command == waitCmd && app.received.sequenceNumber == cmdSequenceNumber) return true;  
        }
        return false;
    } else {
        return false;
    }
}

bool App_SendCommandAndWait(AppCommandType cmd, AppCommandType waitCmd) {
    if (app.sendCommand(cmd)) {
        uint16_t cmdSequenceNumber = sequenceNumber-1;
        appWaitTimeOutCounter=0;
        while (appWaitTimeOutCounter < APP_TIMEOUT_NUM) {
            __delay_ms(10);
             if (app.getCommand(true,waitCmd,cmdSequenceNumber)){
                return true;
            };
            //if (app.received.command == waitCmd && app.received.sequenceNumber == cmdSequenceNumber) return true; 
        }
        return false;
    } else {
        return false;
    }
}

bool App_getCommands() {
    return App_getCommand(false,APP_CMD_NONE, 0);
}

bool App_getCommand(bool waitForCmd, AppCommandType waitCmdType, uint16_t waitCmdSequenceNumber) {
    
    bool exitoso=false;
    uint8_t i2cInputData[BUFFER_SIZE];
    
    if (I2CMaster.Read(app.addr, i2cInputData, BUFFER_SIZE)) {
        while (I2CMaster.IsBusy()) {
        }
        enum I2C_HOST_ERROR err = I2CMaster.ErrorGet();
        if (err == I2C_HOST_ERROR_NONE) {
                                   
            int h=0; 
            while((h + tmpDataLen)<DBUFFER_SIZE && h<BUFFER_SIZE){
                tmpInputData[h + tmpDataLen] =i2cInputData[h];
                h++;
            }
            
            uint8_t inputCmd[DBUFFER_SIZE];
            int x=0;
            int a=0;
            for (int j = 0; j < DBUFFER_SIZE; j++) {
                if(a==0 && tmpInputData[j] == SOH){
                    x=j;
                    inputCmd[a] = tmpInputData[j];                     
                    a++;
                } else if(a==1 && tmpInputData[j] == STX){
                    inputCmd[a] = tmpInputData[j];
                    a++;
                } else if (a>=2){
                    inputCmd[a] = tmpInputData[j];
                    a++;
                    if(a>=8){
                        if ((inputCmd[5] + 8) == a){
                            if (ValidarCheckSum(inputCmd[a - 1], inputCmd, a - 1)) {
                                a=0;                              
                                int i = 6;
                                app.received.command = inputCmd[2];
                                app.received.sequenceNumber = (uint16_t) ((inputCmd[4] << 8) | inputCmd[3]);
                                exitoso = true;
                                if (app.received.command == APP_CMD_PROCESOS_INSTRUCCION) {
                                    app.instruccion = inputCmd[i];
                                    app.sendCommand(APP_CMD_PROTOCOLO_ACK);
                                } else if (app.received.command == APP_CMD_VENTILACION_PARAMETROS) {
                                    app.ventilacion.parametros.volumen = UnpackFloat(&inputCmd[i], &i);
                                    app.ventilacion.parametros.fiO2 = UnpackFloat(&inputCmd[i], &i);
                                    app.ventilacion.parametros.fr = UnpackFloat(&inputCmd[i], &i);
                                    app.ventilacion.parametros.presion = UnpackFloat(&inputCmd[i], &i);
                                    app.ventilacion.parametros.peep = UnpackFloat(&inputCmd[i], &i);
                                    app.ventilacion.parametros.pi = UnpackFloat(&inputCmd[i], &i);
                                    app.ventilacion.parametros.ie = inputCmd[30];
                                    app.ventilacion.parametros.modo = inputCmd[31];
                                    app.ventilacion.parametros.formaOnda = inputCmd[32];
                                    float t = 1.0 / (app.ventilacion.parametros.fr / 60.0);
                                    switch (app.ventilacion.parametros.ie) {
                                        case IE11:
                                            app.ventilacion.parametros.ti = t * 0.5;
                                            break;
                                        case IE12:
                                            app.ventilacion.parametros.ti = t * 0.33;
                                            break;
                                        case IE13:
                                            app.ventilacion.parametros.ti = t * 0.25;
                                            break;
                                        case IE14:
                                            app.ventilacion.parametros.ti = t * 0.20;
                                            break;
                                        case IE21:
                                            app.ventilacion.parametros.ti = t * 0.66;
                                            break;
                                        case IE31:
                                            app.ventilacion.parametros.ti = t * 0.75;
                                            break;
                                        case IE41:
                                            app.ventilacion.parametros.ti = t * 0.80;
                                            break;
                                        default:
                                            app.ventilacion.parametros.ti = t * 0.33;
                                            break;
                                    }
                                    app.ventilacion.parametros.te = t - app.ventilacion.parametros.ti;
                                    app.ventilacion.parametros.tt = app.ventilacion.parametros.te + app.ventilacion.parametros.ti;
                                    app.sendCommand(APP_CMD_PROTOCOLO_ACK);
                                } else if (app.received.command == APP_CMD_PROCESOS_CONSTANTES_PID) {
                                    app.procesos.constantesPID.constantes.kp = UnpackFloat(&inputCmd[i], &i);
                                    app.procesos.constantesPID.constantes.ki = UnpackFloat(&inputCmd[i], &i);
                                    app.procesos.constantesPID.constantes.kd = UnpackFloat(&inputCmd[i], &i);
                                    app.procesos.constantesPID.constantes.maxOutput = UnpackFloat(&inputCmd[i], &i);
                                    app.procesos.constantesPID.constantes.minOutput = UnpackFloat(&inputCmd[i], &i);
                                    app.procesos.constantesPID.constantes.emaAlpha = UnpackFloat(&inputCmd[i], &i);
                                    app.procesos.constantesPID.constantes.testTarget =   UnpackFloat(&inputCmd[i], &i);
                                    app.procesos.constantesPID.indice = inputCmd[34];
                                    app.sendCommand(APP_CMD_PROTOCOLO_ACK);
                                } else if (app.received.command == APP_CMD_PROCESOS_CALIBRACION) {                             
                                    app.procesos.calibracion.m = UnpackFloat(&inputCmd[i], &i);
                                    app.procesos.calibracion.b = UnpackFloat(&inputCmd[i], &i);
                                    app.procesos.calibracion.indice = inputCmd[14];
                                    app.sendCommand(APP_CMD_PROTOCOLO_ACK);
                                } else if (app.received.command == APP_CMD_PROTOCOLO_ACK) {
                                    //nada
                                } else {
                                    app.sendCommand(APP_CMD_PROTOCOLO_NACK);
                                    exitoso = false;
                                } 
                                
                                if(waitForCmd && app.received.command == waitCmdType &&  app.received.sequenceNumber== waitCmdSequenceNumber){                                    
                                    return true;
                                }
                                 
                            }else if ((inputCmd[5] + 8) < a){
                                while(a>0){         //clear                       
                                    a--;
                                    inputCmd[a] = 0;
                                }  
                            }
                        }else if ((inputCmd[5] + 8) < a){
                            while(a>0){         //clear                       
                                a--;
                                inputCmd[a] = 0;
                            }  
                        }
                    }
                }
            }
            
            if(a>0){
                int m=0;
                while(x<DBUFFER_SIZE-1){
                    tmpInputData[m] = tmpInputData[x];
                    x++;
                    m++;
                }
                tmpDataLen=m+1;
                if(tmpDataLen==DBUFFER_SIZE) { tmpDataLen = 0;}
            }
            
            return exitoso;
            
        }
    }



    return false;
};

bool App_sendCommand(AppCommandType cmd) {
    if (cmd == APP_CMD_PROTOCOLO_NACK || cmd == APP_CMD_PROTOCOLO_PING) {
        uint8_t args[] = {0};
        return App_sendCommandWithArgs(cmd, args, 0);
    }
    if (cmd == APP_CMD_SINGLE_DATA) {
        uint8_t buffer[4];
        PackFloat(&buffer, app.ventilacion.data.flujo);
        return App_sendCommandWithArgs(cmd, buffer, 4);
    }
    if (cmd == APP_CMD_PROTOCOLO_ACK) {
        uint8_t args[] = {app.received.command};
        return App_sendCommandWithArgs(cmd, args, 1);
    }
    if (cmd == APP_CMD_VENTILACION_DATA) {
        uint8_t buffer[47];
        PackFloat(&buffer, app.ventilacion.data.flujo);
        PackFloat(&buffer[4], app.ventilacion.data.presionInsp);
        PackFloat(&buffer[8], app.ventilacion.data.fiO2);
        PackFloat(&buffer[12], app.ventilacion.data.flujoAire);
        PackFloat(&buffer[16], app.ventilacion.data.flujoOxigeno);
        PackFloat(&buffer[20], app.ventilacion.data.presionAire);
        PackFloat(&buffer[24], app.ventilacion.data.presionOxigeno);
        PackFloat(&buffer[28], app.ventilacion.data.nivelBateria);
        PackFloat(&buffer[32], app.ventilacion.data.volumen);
        PackFloat(&buffer[36], app.ventilacion.data.vti);
        PackFloat(&buffer[40], app.ventilacion.data.deltaT);
        buffer[44] = app.ventilacion.data.enSuministro ? 1 : 0;
        buffer[45] = app.ventilacion.data.fase;
        buffer[46] = app.ventilacion.data.ciclo;
        return App_sendCommandWithArgs(APP_CMD_VENTILACION_DATA, buffer, 47);
    }
    return false;
}

bool App_sendError(ErrorCode e) {
    uint8_t args[] = {e};
    return App_sendCommandWithArgs(APP_CMD_PROTOCOLO_ERROR_CODE, args, 1);
}

bool App_sendCommandWithArgs(AppCommandType cmd, uint8_t * command, uint8_t len) {
    uint8_t buffer[8 + len];
    buffer[0] = SOH;
    buffer[1] = STX;
    buffer[2] = cmd;
    buffer[3] = (uint8_t) (sequenceNumber & 0xFF);
    buffer[4] = (uint8_t) ((sequenceNumber >> 8) & 0xFF);
    buffer[5] = len;
    int k = 0;
    for (; k < len; k++) {
        buffer[6 + k] = *command++;
    }
    buffer[6 + len] = ETX;
    buffer[7 + len] = GenerarCheckSum(buffer, len + 7);
    enum I2C_HOST_ERROR err;
    if (I2CMaster.Write(app.addr, buffer, len + 8)) {
        sequenceNumber++;
        while (I2CMaster.IsBusy()) {
        }
        err = I2CMaster.ErrorGet();
        if (err == I2C_HOST_ERROR_NONE) {
            return true;
        } else {
            k = 0;
            if (k > 0) {
            }
        }
    }
    return false;
}