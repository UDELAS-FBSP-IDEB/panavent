#include "comm.h" 
#define FCY 60000000UL
#include <libpic30.h> 
#include "../panavent.hardware.drivers.X/mcc_generated_files/i2c_host/i2c1.h"
#include "../panavent.hardware.drivers.X/timer3Controller.h"
#include "../panavent.utilities.X/utilities.h"

#define APP_TIMEOUT_NUM 100
#define BUFFER_SIZE   64
#define DBUFFER_SIZE    BUFFER_SIZE*2
static uint8_t tmpInputData[DBUFFER_SIZE];
volatile unsigned int appWaitTimeOutCounter;
static uint16_t sequenceNumber;
static uint8_t tmpDataLen=0;

static void Comm_TimeOutTimerCallBack();
static bool Comm_readInputChunk(uint8_t *i2cInputData);
static void Comm_appendTmpData(const uint8_t *i2cInputData);
static void Comm_clearInputCmd(uint8_t *inputCmd, int *a);
static void Comm_preservePartialData(int x, int a);
static void Comm_preserveFromIndex(int startIndex);

Comm comm = {  
    .addr = 0x2E,
    .onCommandReceived = NULL, 
    .inicializar = &Comm_inicializar, 
    .sendCommandAndWait = &Comm_SendCommandAndWait,
    .sendCommandAndWaitWithArgs = &Comm_sendCommandAndWaitWithArgs,
    .sendCommandWithArgs = &Comm_sendCommandWithArgs,
    .getCommand = &Comm_getCommand,
    .getCommands = &Comm_getCommands
};

void Comm_inicializar(CommOnCommandReceived callback){
    comm.onCommandReceived = callback;
    timer3Controller.inicializar();
    timer3Controller.agregarCallback(&Comm_TimeOutTimerCallBack,TIMER3_PERIOD);
}

static void Comm_TimeOutTimerCallBack(){
    appWaitTimeOutCounter++;
}

bool Comm_getCommands() {
   return Comm_getCommand(false,0, 0);
}

bool Comm_sendCommandAndWaitWithArgs(uint8_t cmd, uint8_t waitCmd, uint8_t * command, uint8_t len) {
    
    if (Comm_sendCommandWithArgs(cmd, command, len)) {
        uint16_t cmdSequenceNumber = sequenceNumber-1;
        appWaitTimeOutCounter=0;
        while (appWaitTimeOutCounter < APP_TIMEOUT_NUM) {
            __delay_ms(20);
            if (comm.getCommand(true,waitCmd,cmdSequenceNumber)){
                return true;
            }; 
        }
        return false;
    } else {
        return false;
    }
}

bool Comm_SendCommandAndWait(uint8_t cmd, uint8_t waitCmd) {
    uint8_t args[] = {0};
    if (Comm_sendCommandWithArgs(cmd, args, 0)) {
        uint16_t cmdSequenceNumber = sequenceNumber-1;
        appWaitTimeOutCounter=0;
        while (appWaitTimeOutCounter < APP_TIMEOUT_NUM) {
            __delay_ms(10);
             if (comm.getCommand(true,waitCmd,cmdSequenceNumber)){
                return true;
            };
            //if (comm.received.command == waitCmd && comm.received.sequenceNumber == cmdSequenceNumber) return true; 
        }
        return false;
    } else {
        return false;
    }
}
  
bool Comm_sendCommandWithArgs(uint8_t cmd, uint8_t * command, uint8_t len) { 
    uint8_t buffer[BUFFER_SIZE];
    if ((uint16_t)len + 8u > BUFFER_SIZE) {
        return false;
    }
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
    if (I2CMaster.Write(comm.addr, buffer, len + 8)) {
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

bool Comm_getCommand(bool waitForCmd, uint8_t waitCmdType, uint16_t waitCmdSequenceNumber) {
    
    /*
     Estructura del frame:
        SOH (1)
        STX (1)
        CMD (1)
        SEQ_L (1)
        SEQ_H (1)
        LEN (1)
        DATA (LEN)
        ETX (1)
        CHK (1)
     */
    
    bool    procesado = false;
    uint8_t i2cInputData[BUFFER_SIZE];
    uint8_t inputCmd[DBUFFER_SIZE] = {0};
    
    int x = 0;  //posición en el buffer donde empezó el frame
    int a = 0;  //longitud actual del frame que estás armando

    //Lee el buffer i2c
    if (!Comm_readInputChunk(i2cInputData)) {
        return false;
    }

    //Se pasa al buffer de reemsamblaje
    Comm_appendTmpData(i2cInputData);

    //Se recorre el buffer de reemsamblaje
    for (int j = 0; j < tmpDataLen; j++) {
        if (a == 0 && tmpInputData[j] == SOH) {
            x = j;
            inputCmd[a] = tmpInputData[j];
            a++;
        } else if (a == 1 && tmpInputData[j] == STX) {
            inputCmd[a] = tmpInputData[j];
            a++;
        } else if (a >= 2) {
            inputCmd[a] = tmpInputData[j];
            a++;

            if (a >= 8) {
                int expectedSize = inputCmd[5] + 8;
                if (expectedSize == a) {
                    //Cuando se alcanzo el final del frame
                    if (inputCmd[a - 2] != ETX) {
                        Comm_clearInputCmd(inputCmd, &a);
                    } else if (ValidarCheckSum(inputCmd[a - 1], inputCmd, a - 1)) {
                        a = 0;
                        procesado = true;
                        
                        uint8_t command = inputCmd[2];
                        uint16_t cmdSequenceNumber = (uint16_t)((inputCmd[4] << 8) | inputCmd[3]);
                         
                        if(command != 3 && command !=4){ //3 y 4 reservados para ACK y NACK, respectivamente.
                            uint8_t args[] = {command};
                            comm.sendCommandWithArgs(3, args, 1); 
                        }
                        
                        //Se invoca el callback
                        if (comm.onCommandReceived != NULL) {
                            comm.onCommandReceived(command, &inputCmd[6], inputCmd[5]);
                        }
                        
                        //Si se solicito espera, se devuelve true de inmediato
                        //y no se sigue leyendo. 
                        if (waitForCmd && command == waitCmdType && cmdSequenceNumber == waitCmdSequenceNumber) {
                            Comm_preserveFromIndex(j + 1);
                            return true;
                        }
                        
                        
                    } else {
                        //Error, se descarta el frame
                        Comm_clearInputCmd(inputCmd, &a);
                    }
                } else if (expectedSize < a) {
                    //Error, se descarta el frame
                    Comm_clearInputCmd(inputCmd, &a);
                }
            }
        }
    }

    Comm_preservePartialData(x, a);
    return procesado;
}

/*Helpers*/


static bool Comm_readInputChunk(uint8_t *i2cInputData) {
    if (!I2CMaster.Read(comm.addr, i2cInputData, BUFFER_SIZE)) {
        return false;
    }

    while (I2CMaster.IsBusy()) {
    }

    return (I2CMaster.ErrorGet() == I2C_HOST_ERROR_NONE);
}

static void Comm_appendTmpData(const uint8_t *i2cInputData) {
    int h = 0;

    while ((h + tmpDataLen) < DBUFFER_SIZE && h < BUFFER_SIZE) {
        tmpInputData[h + tmpDataLen] = i2cInputData[h];
        h++;
    }

    tmpDataLen += h;
}

static void Comm_clearInputCmd(uint8_t *inputCmd, int *a) {
    while (*a > 0) {
        (*a)--;
        inputCmd[*a] = 0;
    }
}

static void Comm_preservePartialData(int x, int a) {
    if (a > 0) {
        int m = 0;
        while (x < tmpDataLen) {
            tmpInputData[m] = tmpInputData[x];
            x++;
            m++;
        }
        tmpDataLen = m;
        if (tmpDataLen == DBUFFER_SIZE) {
            tmpDataLen = 0;
        }
    } else {
        tmpDataLen = 0;
    }
}

static void Comm_preserveFromIndex(int startIndex) {
    if (startIndex >= tmpDataLen) {
        tmpDataLen = 0;
        return;
    }

    int m = 0;
    while (startIndex < tmpDataLen) {
        tmpInputData[m] = tmpInputData[startIndex];
        startIndex++;
        m++;
    }

    tmpDataLen = m;
}