
#include <math.h>
#include "main.h"
#include <xc.h>  
#define FCY 60000000UL
#include <libpic30.h> 
#include "comun.h"
#include "app.h"
#include "monitorPowerSupply.h"
#include "../panavent.hardware.drivers.X/mcc_generated_files/system/system.h"
#include "../panavent.hardware.drivers.X/timer3Controller.h"
#include "../panavent.hardware.drivers.X/pca9543apw_interface.h"
#include "../panavent.hardware.drivers.X/mcp342x_interface.h"
#include "../panavent.mezclador.X/mezclador.h"  
#include "../panavent.valexp.X/valvulaExpiratoria.h"
#include "../panavent.hardware.drivers.X/mcc_generated_files/pwm/sccp4.h"
#include "../panavent.hardware.drivers.X/sfm_interface.h" 

float t1 = 0;
float t2 = 0;

#define TICKS_SEND_DATA     20      // 20 ms
#define TICKS_GET_COMMAND   1000    // 1 s 
#define TICKS_MCP           250     // 250 ms
 
volatile uint16_t  cnt_timing_mcp = 0;
volatile uint16_t  cnt_timing_ventilacion = 0; 
volatile uint16_t  cnt_timing_send_data = 0;
volatile uint16_t  cnt_timing_get_command = 0;

static uint8_t pidTuningIndex = 0xFF;
uint8_t mcp2ch = 1;
PIDrequest presionPIDreq;
float presionInspM = 1;
float presionInspB = 0;
float presionExpM = 1;
float presionExpB = 0;
volatile uint8_t FLAG = 0x00;
 
#define TIMING_MCP_FLAG                     CHECKBIT(FLAG,2)
#define TIMING_CLEAR_MCP_FLAG               CLEARBIT(FLAG,2)
#define TIMING_SET_MCP_FLAG                 SETBIT(FLAG,2)  
#define CAPTURAR_VTI_FLAG                   CHECKBIT(FLAG,4)
#define CLEAR_CAPTURAR_VTI_FLAG             CLEARBIT(FLAG,4)
#define SET_CAPTURAR_VTI_FLAG               SETBIT(FLAG,4)
#define TIMING_GET_APP_COMMAND_FLAG         CHECKBIT(FLAG,5)
#define TIMING_CLEAR_APP_COMMAND_FLAG       CLEARBIT(FLAG,5)
#define TIMING_SET_COMMAND_FLAG             SETBIT(FLAG,5) 
#define TIMING_SEND_DATA_FLAG               CHECKBIT(FLAG,6)
#define TIMING_CLEAR_SEND_DATA_FLAG         CLEARBIT(FLAG,6)
#define TIMING_SET_SEND_DATA_FLAG           SETBIT(FLAG,6) 

 

ErrorCode error; 
 
volatile MainState gState = ST_BOOT; 

int main(void) {
    while (1) {  
        if(gState == ST_BOOT){
            if (inicializacion()) {
                app.sendError(INIT_OK);
                cambiarEstado(ST_IDLE);
            } else {
                app.sendError(error);
                __delay_ms(1000);
            }
        }else{
            getAppCommands();
            sendAppData();
            switch (gState) {  
                case ST_IDLE: 
                    monitorPowerSuply.cerrarValvulaSeguridad(false);
                    break; 
                case ST_VENTILANDO: 
                    ejecutarVentilacion();
                    break; 
                case ST_PID_TUNING: 
                    ejecutarPidTuning();
                    break; 
                case ST_MANUAL_PWM: 
                    ejecutarManualPWM();
                    break;  
                case ST_RESET: 
                    ejecutarReset();
                    break;  
                default:
                    //Estado prohibido
                    ejecutarReset();
                    break;
            }
        } 
    }
}
 
static void cambiarEstado(MainState nuevoEstado) {
    if (gState == nuevoEstado) return; 
    onExitEstado((MainState)gState);
    gState = nuevoEstado;
    onEnterEstado(nuevoEstado);
}

static void onEnterEstado(MainState estado) {
    switch (estado) { 
        case ST_VENTILANDO:
            resetVariablesVentilacion();
            break; 
        case ST_MANUAL_PWM:
            mezclador.startManualPWM();
            break; 
        case ST_PID_TUNING:
            pidTuningIndex = 0xFF;
            break; 
        default:
            break;
    }
}

static void onExitEstado(MainState estado) {
    switch (estado) {
        case ST_MANUAL_PWM:
            mezclador.stopManualPWM();
            break; 
        case ST_PID_TUNING:
            pidTuningIndex = 0xFF;
            break;
        default:
            break;
    }
}

static void getAppCommands(void) {
    if (TIMING_GET_APP_COMMAND_FLAG) {
        monitorPowerSuply.getInfoCarga();
        
        /*ToDo: el comando y estado de calibracion no esta implementado.*/
        if (app.getCommands()) {
            switch (app.instruccion) {
                case 0x01: cambiarEstado(ST_RESET); break;
                case 0x02: cambiarEstado(ST_VENTILANDO); break;
                case 0x03: cambiarEstado(ST_PID_TUNING); break;
                case 0x05: cambiarEstado(ST_MANUAL_PWM); break;
                case 0x06: cambiarEstado(ST_IDLE); break;
            }
            onCommandReceived(false);
        }

        TIMING_CLEAR_APP_COMMAND_FLAG;
    }
}

static void ejecutarVentilacion(void){
    
    if (TIMING_MCP_FLAG) {   
        leerMCP2();
        TIMING_CLEAR_MCP_FLAG;
    }   
    
    monitorPowerSuply.cerrarValvulaSeguridad(true);         
    
    if (app.ventilacion.data.fase == 1) {
        if(!CAPTURAR_VTI_FLAG) SET_CAPTURAR_VTI_FLAG;
        valvulaExpiratoria.cerrar();
    } else if (app.ventilacion.data.fase == 2) {
        if(CAPTURAR_VTI_FLAG) {
            app.ventilacion.data.vti = app.ventilacion.data.volumen;
            CLEAR_CAPTURAR_VTI_FLAG;
        }
        valvulaExpiratoria.controlPeep(app.ventilacion.parametros.peep);
    }   
 
    
    mezclador.fm = getFlujoMeta();
    mezclador.cm = app.ventilacion.parametros.fiO2;
    if (mezclador.mezclar()) {
        app.ventilacion.data.flujo = mezclador.data;
        t2 = (float)cnt_timing_ventilacion * TIMER3_PERIOD;
        app.ventilacion.data.deltaT = (t2 - t1);
        app.ventilacion.data.volumen += (app.ventilacion.data.deltaT)*(app.ventilacion.data.flujo / 60)*1000;
        t1 = t2; 
    } 
     
           
    if (sfmProxi.leerFlujo()) {
        app.ventilacion.data.flujoProximal = sfmProxi.flow; 
    }
}
 
static void ejecutarPidTuning(void) {
    monitorPowerSuply.cerrarValvulaSeguridad(true);
    
    if (app.getCommand(true, APP_CMD_PROCESOS_CONSTANTES_PID, 0)) {
        onCommandReceived(true);
        pidTuningIndex = app.procesos.constantesPID.indice;
    }
    switch (pidTuningIndex) {
        case 3:
            if (TIMING_MCP_FLAG) {
                leerMCP2();
                mezclador.cl = (app.ventilacion.data.fiO2 * 3065.9f - 3.6271f) / 100.0f;
                TIMING_CLEAR_MCP_FLAG;
            }
            mezclador.mezclar();
            break; 
        case 4:
            mezclador.mezclarAire();
            break;
        case 5:
            mezclador.mezclarOxigeno();
            break;
    }
}

static void ejecutarManualPWM(void) { 
    monitorPowerSuply.cerrarValvulaSeguridad(true); 
    if (app.getCommand(true, APP_CMD_PROCESOS_SET_PWM, 0)) {
        if (app.procesos.pwm.oxigeno) {
            mezclador.setPWMOxigeno(app.procesos.pwm.porcentaje);
        } else {
            mezclador.setPWMAire(app.procesos.pwm.porcentaje);
        }
    }
}

static void ejecutarReset(void) {
    monitorPowerSuply.cerrarValvulaSeguridad(false);
    mezclador.stopManualPWM();
    resetVariablesVentilacion();
    app.sendError(RESETING);
    cambiarEstado(ST_BOOT); 
}

static void resetVariablesVentilacion(void) {
    cnt_timing_send_data = 0;
    cnt_timing_ventilacion =0;
    t1 = 0.0f;
    t2 = 0.0f;
    app.ventilacion.data.volumen = 0.0f;
    app.ventilacion.data.vti = 0.0f;
    app.ventilacion.data.deltaT = 0.0f;
    app.ventilacion.data.fase = 1;
    CLEAR_CAPTURAR_VTI_FLAG;
}

static void sendAppData(void) {
    if (TIMING_SEND_DATA_FLAG) {
        switch (gState) {
            case ST_PID_TUNING:
                app.ventilacion.data.singleData = mezclador.data;
                app.sendCommand(APP_CMD_SINGLE_DATA);
                break;
            case ST_VENTILANDO:
                app.sendCommand(APP_CMD_VENTILACION_DATA);
                break;
            default:
                break;
        } 
        TIMING_CLEAR_SEND_DATA_FLAG;
    }
}
 
static bool inicializacion(void) {
  
    /*inicializa los componentes de hardware*/
    SYSTEM_Initialize();
    app.inicializar();   
   
    /*Inicializa la app*/  
    while(true){
        if (app.sendCommandAndWait(APP_CMD_PROTOCOLO_PING, APP_CMD_PROTOCOLO_ACK)){      
               
            /*timingTimer*/
           timer3Controller.inicializar();
           timer3Controller.agregarCallback(&timingTimerCallBack, TIMER3_PERIOD);


            /*Inicializa al mezclador*/
            error = mezclador.inicializar();  

            if (error == NO_ERROR) {
                uint8_t i = 0;
                while (i < 7) {
                    uint8_t args[] = {i};
                    if (app.sendCommandAndWaitWithArgs(APP_CMD_PROCESOS_GET_CONSTANTES_PID, APP_CMD_PROCESOS_CONSTANTES_PID, args, 1)) {
                        onCommandReceived(false);
                    } else {
                        error = GET_PID_PARAM_ERROR;
                        app.sendError(error);
                        break;
                    }
                    i++;
                }
                if (error == NO_ERROR) {
                    i = 0;
                    while (i < 8) {
                        uint8_t args[] = {i};
                        if (app.sendCommandAndWaitWithArgs(APP_CMD_PROCESOS_GET_CALIBRACION, APP_CMD_PROCESOS_CALIBRACION, args, 1)) {
                            onCommandReceived(false);
                        } else {
                            error = GET_CALIBRACION_ERROR;
                            app.sendError(error);
                            break;
                        }
                        i++;
                    }
                }
            }

            valvulaExpiratoria.inicializar();

            //if (!sfmProxi.inicializar()) {
            //    error = SFM_PROX_ERROR;
            //}

            if (error == NO_ERROR){ 
                return true; 
            };
            
        };
        __delay_ms(10000);
    }
    
    
    return false;
    
};

static void onCommandReceived(bool test) {
    switch (app.received.command) {
        case APP_CMD_PROCESOS_CONSTANTES_PID: {
            uint8_t indice = app.procesos.constantesPID.indice;

            if (indice < 6) {
                mezclador.setConstatesPID(
                    indice,
                    app.procesos.constantesPID.constantes,
                    test
                );
            } else if (indice == 6) {
                presionPIDreq.constantes = app.procesos.constantesPID.constantes;
            }
            break;
        }

        case APP_CMD_PROCESOS_CALIBRACION: {
            uint8_t indice = app.procesos.calibracion.indice;

            if (indice < 6) {
                mezclador.setCalibracion(
                    indice,
                    app.procesos.calibracion.m,
                    app.procesos.calibracion.b
                );
            } else if (indice == 6) {
                presionInspM = app.procesos.calibracion.m;
                presionInspB = app.procesos.calibracion.b;
            } else if (indice == 7) {
                presionExpM = app.procesos.calibracion.m;
                presionExpB = app.procesos.calibracion.b;
            }
            break;
        }

        default:
            break;
    }
}

static float getFlujoMeta(void) {
    if (app.ventilacion.data.fase == 1) {
        if (app.ventilacion.parametros.modo == VENTILACION_MVC) {
            if (app.ventilacion.parametros.formaOnda == VENTILACION_SQR) {
                app.ventilacion.parametros.flujo = (app.ventilacion.parametros.volumen / 1000.0) / (app.ventilacion.parametros.ti / 60);
                return app.ventilacion.parametros.flujo;
            } else if (app.ventilacion.parametros.formaOnda == VENTILACION_SIN) {
                app.ventilacion.parametros.fi = 1 / (app.ventilacion.parametros.ti / 60);
                app.ventilacion.parametros.qi = (app.ventilacion.parametros.volumen / 1000.0) / (2 * app.ventilacion.parametros.ti);
                float tVent = (float)cnt_timing_ventilacion * TIMER3_PERIOD;
                app.ventilacion.parametros.flujo = app.ventilacion.parametros.qi * (0.06) * sin(3.1415926535898 * app.ventilacion.parametros.fi * (tVent));
                return app.ventilacion.parametros.flujo;
            } else if (app.ventilacion.parametros.formaOnda == VENTILACION_DES) {
                return 0;
            }
        } else if (app.ventilacion.parametros.modo == VENTILACION_MPC) {
            presionPIDreq.meta = app.ventilacion.parametros.presion;
            presionPIDreq.medicion = app.ventilacion.data.presionInsp;
            if (pid.calcular(&presionPIDreq)) {
                return presionPIDreq.calculos.output;
            }
        }
    } else if (app.ventilacion.data.fase == 2) {
        return 0;
    }
    return 0;
};

void timingTimerCallBack() {

    cnt_timing_mcp++; 
    cnt_timing_send_data++;
    cnt_timing_get_command++;
   
    
    if (cnt_timing_send_data >= TICKS_SEND_DATA) {
        TIMING_SET_SEND_DATA_FLAG;
        cnt_timing_send_data = 0;
    }

    if (cnt_timing_get_command >= TICKS_GET_COMMAND   ) {
        TIMING_SET_COMMAND_FLAG;
        cnt_timing_get_command = 0;
    }
 
    if (cnt_timing_mcp  >= TICKS_MCP) {
        TIMING_SET_MCP_FLAG;
        cnt_timing_mcp = 0;
    }

    // Control del timing de la ventilacion
    if (gState == ST_VENTILANDO) {
        cnt_timing_ventilacion++;
        float tVent = (float)cnt_timing_ventilacion * TIMER3_PERIOD;
        if (tVent <= app.ventilacion.parametros.ti) {
            app.ventilacion.data.fase = 1;
        } else if ((tVent > app.ventilacion.parametros.ti) && (tVent <= app.ventilacion.parametros.tt)) {
            app.ventilacion.data.fase = 2;
        } else if (tVent > app.ventilacion.parametros.tt) {            
            app.ventilacion.data.ciclo++;
            app.ventilacion.data.volumen = 0;
            cnt_timing_ventilacion = 0;
        }
    }

}

static void leerMCP2() {
    float volt = 0;
    if (PCA9543APW_setCanal(0x71, 1)) {
        if (Mcp342x_leerConversion(MCP3428_ADDR, mcp2ch, &volt)) {
            float calculatedValue = 10.1972 * (volt - 0.66) / 0.057;
            switch (mcp2ch) {
                case 0x01:
                    app.ventilacion.data.presionInsp = (presionInspM * calculatedValue) + presionInspB;
                    break;
                case 0x02:
                    app.ventilacion.data.presionExp = (presionExpM * calculatedValue) + presionExpB;
                    break;
                case 0x03:
                    app.ventilacion.data.fiO2 = volt;
                    break;
                case 0x04:
                    // ivexp = volt / (20 * vExp->shuntResistor);
                    break;
            }
            mcp2ch++;
        }
        if (mcp2ch > 4) mcp2ch = 1;
        Mcp342x_dispararConvercion(MCP3428_ADDR, mcp2ch);
    }
}
