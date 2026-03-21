#include <math.h>
#include "main.h"
#include <xc.h>
#define FCY 60000000UL
#include <libpic30.h>
#include "comun.h"
#include "app.h"
#include "comm.h"
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

#define TICKS_SEND_DATA     20
#define TICKS_GET_COMMAND   1000
#define TICKS_MCP           250

volatile uint16_t cnt_timing_mcp = 0;
volatile uint16_t cnt_timing_ventilacion = 0;
volatile uint16_t cnt_timing_send_data = 0;
volatile uint16_t cnt_timing_get_command = 0;

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

 
static AppVentilacionParams ventilacionParams;
static AppVentilacionData ventilacionData;

static volatile bool hayInstruccionPendiente = false;
static volatile uint8_t instruccionPendiente = 0;

static volatile bool hayConstantesPIDPendientes = false;
static volatile bool hayCalibracionPendiente = false;
static volatile bool hayPWMPendiente = false;

static AppConstantesPID constantesPIDRx;
static AppCalibracionData calibracionRx;
static AppPWMData pwmRx;

int main(void) {
    while (1) {
        if (gState == ST_BOOT) {
            if (inicializacion()) {
                app.sendError(INIT_OK);
                cambiarEstado(ST_IDLE);
            } else {
                app.sendError(error);
                __delay_ms(1000);
            }
        } else {
            
            getAppCommands();
            procesarEventosApp();
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
    if (!TIMING_GET_APP_COMMAND_FLAG) return;

    monitorPowerSuply.getInfoCarga();
    comm.getCommands();
    TIMING_CLEAR_APP_COMMAND_FLAG;
}

static void procesarEventosApp(void) {
    if (hayInstruccionPendiente) {
        switch (instruccionPendiente) {
            case 0x01: cambiarEstado(ST_RESET); break;
            case 0x02: cambiarEstado(ST_VENTILANDO); break;
            case 0x03: cambiarEstado(ST_PID_TUNING); break;
            case 0x05: cambiarEstado(ST_MANUAL_PWM); break;
            case 0x06: cambiarEstado(ST_IDLE); break;
            default: break;
        }
        hayInstruccionPendiente = false;
    }

    if (hayConstantesPIDPendientes) {
        procesarConstantesPID(false);
        hayConstantesPIDPendientes = false;
    }

    if (hayCalibracionPendiente) {
        procesarCalibracion();
        hayCalibracionPendiente = false;
    }
}

static void ejecutarVentilacion(void) {
    if (TIMING_MCP_FLAG) {
        leerMCP2();
        TIMING_CLEAR_MCP_FLAG;
    }

    monitorPowerSuply.cerrarValvulaSeguridad(true);

    if (ventilacionData.fase == 1) {
        if (!CAPTURAR_VTI_FLAG) SET_CAPTURAR_VTI_FLAG;
        valvulaExpiratoria.cerrar();
    } else if (ventilacionData.fase == 2) {
        if (CAPTURAR_VTI_FLAG) {
            ventilacionData.vti = ventilacionData.volumen;
            CLEAR_CAPTURAR_VTI_FLAG;
        }
        valvulaExpiratoria.controlPeep(ventilacionParams.peep);
    }

    mezclador.fm = getFlujoMeta();
    mezclador.cm = ventilacionParams.fiO2;

    if (mezclador.mezclar()) {
        ventilacionData.flujo = mezclador.data;
        t2 = (float)cnt_timing_ventilacion * TIMER3_PERIOD;
        ventilacionData.deltaT = (t2 - t1);
        ventilacionData.volumen += (ventilacionData.deltaT) * (ventilacionData.flujo / 60.0f) * 1000.0f;
        t1 = t2;
    }

    if (sfmProxi.leerFlujo()) {
        ventilacionData.flujoProximal = sfmProxi.flow;
    }
}

static void ejecutarPidTuning(void) {
    monitorPowerSuply.cerrarValvulaSeguridad(true);

    if (comm.getCommand(true, APP_CMD_PROCESOS_CONSTANTES_PID, 0)) {
        if (hayConstantesPIDPendientes) {
            procesarConstantesPID(true);
            pidTuningIndex = constantesPIDRx.indice;
            hayConstantesPIDPendientes = false;
        }
    }

    switch (pidTuningIndex) {
        case 3:
            if (TIMING_MCP_FLAG) {
                leerMCP2();
                mezclador.cl = (ventilacionData.fiO2 * 3065.9f - 3.6271f) / 100.0f;
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

    if (comm.getCommand(true, APP_CMD_PROCESOS_SET_PWM, 0)) {
        if (hayPWMPendiente) {
            if (pwmRx.oxigeno) {
                mezclador.setPWMOxigeno(pwmRx.porcentaje);
            } else {
                mezclador.setPWMAire(pwmRx.porcentaje);
            }
            hayPWMPendiente = false;
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
    cnt_timing_ventilacion = 0;
    t1 = 0.0f;
    t2 = 0.0f;
    ventilacionData.volumen = 0.0f;
    ventilacionData.vti = 0.0f;
    ventilacionData.vte = 0.0f;
    ventilacionData.deltaT = 0.0f;
    ventilacionData.ciclo = 0;
    ventilacionData.fase = 1;
    CLEAR_CAPTURAR_VTI_FLAG;
}

static void sendAppData(void) {
    if (!TIMING_SEND_DATA_FLAG) return;

    switch (gState) {
        case ST_PID_TUNING:
            app.sendData(mezclador.data);
            break;
        case ST_MANUAL_PWM:
            app.sendData(mezclador.data);
            break;
        case ST_VENTILANDO:
            app.sendVentilacionData(ventilacionData);
            break;

        default:
            break;
    }

    TIMING_CLEAR_SEND_DATA_FLAG;
}

static bool inicializacion(void) {
    SYSTEM_Initialize();

    app.onVentilacionParams = &Main_OnVentilacionParams;
    app.onConstantesPID = &Main_OnConstantesPID;
    app.onCalibracionData = &Main_OnCalibracionData;
    app.onPWMData = &Main_OnPWMData;
    app.onInstruccion = &Main_OnInstruccion;
    app.inicializar();

    while (true) {
        if (comm.sendCommandAndWait(APP_CMD_PROTOCOLO_PING, APP_CMD_PROTOCOLO_ACK)) {

            timer3Controller.inicializar();
            timer3Controller.agregarCallback(&timingTimerCallBack, TIMER3_PERIOD);

            error = mezclador.inicializar();

            if (error == NO_ERROR) {
                uint8_t i = 0;

                while (i < 7) {
                    uint8_t args[] = {i};
                    hayConstantesPIDPendientes = false;

                    if (comm.sendCommandAndWaitWithArgs(
                            APP_CMD_PROCESOS_GET_CONSTANTES_PID,
                            APP_CMD_PROCESOS_CONSTANTES_PID,
                            args,
                            1)) {

                        if (hayConstantesPIDPendientes) {
                            procesarConstantesPID(false);
                            hayConstantesPIDPendientes = false;
                        } else {
                            error = GET_PID_PARAM_ERROR;
                            app.sendError(error);
                            break;
                        }
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
                        hayCalibracionPendiente = false;

                        if (comm.sendCommandAndWaitWithArgs(
                                APP_CMD_PROCESOS_GET_CALIBRACION,
                                APP_CMD_PROCESOS_CALIBRACION,
                                args,
                                1)) {

                            if (hayCalibracionPendiente) {
                                procesarCalibracion();
                                hayCalibracionPendiente = false;
                            } else {
                                error = GET_CALIBRACION_ERROR;
                                app.sendError(error);
                                break;
                            }
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

            if (error == NO_ERROR) {
                return true;
            }
        }

        __delay_ms(10000);
    }

    return false;
}

static void procesarConstantesPID(bool test) {
    uint8_t indice = constantesPIDRx.indice;

    if (indice < 6) {
        mezclador.setConstatesPID(indice, constantesPIDRx.constantes, test);
    } else if (indice == 6) {
        presionPIDreq.constantes = constantesPIDRx.constantes;
    }
}

static void procesarCalibracion(void) {
    uint8_t indice = calibracionRx.indice;

    if (indice < 6) {
        mezclador.setCalibracion(indice, calibracionRx.m, calibracionRx.b);
    } else if (indice == 6) {
        presionInspM = calibracionRx.m;
        presionInspB = calibracionRx.b;
    } else if (indice == 7) {
        presionExpM = calibracionRx.m;
        presionExpB = calibracionRx.b;
    }
}

static float getFlujoMeta(void) {
    if (ventilacionData.fase == 1) {
        if (ventilacionParams.modo == VENTILACION_MVC) {
            if (ventilacionParams.formaOnda == VENTILACION_SQR) {
                ventilacionParams.flujo = (ventilacionParams.volumen / 1000.0f) / (ventilacionParams.ti / 60.0f);
                return ventilacionParams.flujo;

            } else if (ventilacionParams.formaOnda == VENTILACION_SIN) {
                ventilacionParams.fi = 1.0f / (ventilacionParams.ti / 60.0f);
                ventilacionParams.qi = (ventilacionParams.volumen / 1000.0f) / (2.0f * ventilacionParams.ti);

                {
                    float tVent = (float)cnt_timing_ventilacion * TIMER3_PERIOD;
                    ventilacionParams.flujo =
                        ventilacionParams.qi * 0.06f *
                        sin(3.1415926535898f * ventilacionParams.fi * tVent);
                }

                return ventilacionParams.flujo;

            } else if (ventilacionParams.formaOnda == VENTILACION_DES) {
                return 0.0f;
            }

        } else if (ventilacionParams.modo == VENTILACION_MPC) {
            presionPIDreq.meta = ventilacionParams.presion;
            presionPIDreq.medicion = ventilacionData.presionInsp;

            if (pid.calcular(&presionPIDreq)) {
                return presionPIDreq.calculos.output;
            }
        }

    } else if (ventilacionData.fase == 2) {
        return 0.0f;
    }

    return 0.0f;
}

void timingTimerCallBack() {
    cnt_timing_mcp++;
    cnt_timing_send_data++;
    cnt_timing_get_command++;

    if (cnt_timing_send_data >= TICKS_SEND_DATA) {
        TIMING_SET_SEND_DATA_FLAG;
        cnt_timing_send_data = 0;
    }

    if (cnt_timing_get_command >= TICKS_GET_COMMAND) {
        TIMING_SET_COMMAND_FLAG;
        cnt_timing_get_command = 0;
    }

    if (cnt_timing_mcp >= TICKS_MCP) {
        TIMING_SET_MCP_FLAG;
        cnt_timing_mcp = 0;
    }

    if (gState == ST_VENTILANDO) {
        cnt_timing_ventilacion++;

        {
            float tVent = (float)cnt_timing_ventilacion * TIMER3_PERIOD;

            if (tVent <= ventilacionParams.ti) {
                ventilacionData.fase = 1;
            } else if ((tVent > ventilacionParams.ti) && (tVent <= ventilacionParams.tt)) {
                ventilacionData.fase = 2;
            } else if (tVent > ventilacionParams.tt) {
                ventilacionData.ciclo++;
                ventilacionData.volumen = 0;
                cnt_timing_ventilacion = 0;
            }
        }
    }
}

static void leerMCP2() {
    float volt = 0;

    if (PCA9543APW_setCanal(0x71, 1)) {
        if (Mcp342x_leerConversion(MCP3428_ADDR, mcp2ch, &volt)) {
            float calculatedValue = 10.1972f * (volt - 0.66f) / 0.057f;

            switch (mcp2ch) {
                case 0x01:
                    ventilacionData.presionInsp = (presionInspM * calculatedValue) + presionInspB;
                    break;

                case 0x02:
                    ventilacionData.presionExp = (presionExpM * calculatedValue) + presionExpB;
                    break;

                case 0x03:
                    ventilacionData.fiO2 = volt;
                    break;

                case 0x04:
                    break;
            }

            mcp2ch++;
        }

        if (mcp2ch > 4) mcp2ch = 1;
        Mcp342x_dispararConvercion(MCP3428_ADDR, mcp2ch);
    }
}

/* Callbacks */

static void Main_OnVentilacionParams(const AppVentilacionParams *args) {
    ventilacionParams = *args;
}

static void Main_OnConstantesPID(const AppConstantesPID *args) {
    constantesPIDRx = *args;
    hayConstantesPIDPendientes = true;
}

static void Main_OnCalibracionData(const AppCalibracionData *args) {
    calibracionRx = *args;
    hayCalibracionPendiente = true;
}

static void Main_OnPWMData(const AppPWMData *args) {
    pwmRx = *args;
    hayPWMPendiente = true;
}

static void Main_OnInstruccion(uint8_t instruccion) {
    instruccionPendiente = instruccion;
    hayInstruccionPendiente = true;
}