
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
float cnt_mcp = 0;
float cnt_ventilacion = 0;
float cnt_mezclar = 0;
float cnt_ventilacion_data = 0;
float cnt_ventilacion_params = 0;

uint8_t mcp2ch = 1;
PIDrequest presionPIDreq;
float presionInspM = 1;
float presionInspB = 0;
float presionExpM = 1;
float presionExpB = 0;
uint8_t FLAG = 0x00;
#define MEZCLAR                     CHECKBIT(FLAG,0)
#define CLEAR_MEZCLAR               CLEARBIT(FLAG,0)
#define SET_MEZCLAR                 SETBIT(FLAG,0)
#define READ_MCP_FLAG               CHECKBIT(FLAG,2)
#define CLEAR_READ_MCP_FLAG         CLEARBIT(FLAG,2)
#define SET_MCP                     SETBIT(FLAG,2)  
#define FLAG_TAKE_VTI               CHECKBIT(FLAG,4)
#define CLEAR_FLAG_TAKE_VTI         CLEARBIT(FLAG,4)
#define SET_FLAG_TAKE_VTI           SETBIT(FLAG,4)
#define GET_VENT_PARAMS_FLAG          CHECKBIT(FLAG,5)
#define CLEAR_VENT_DATA_PARAMS_FLAG    CLEARBIT(FLAG,5)
#define SET_VENTILACION_PARAMS      SETBIT(FLAG,5) 
#define SEND_VENT_DATA_FLAG            CHECKBIT(FLAG,6)
#define CLEAR_SEND_VENT_DATA_FLAG      CLEARBIT(FLAG,6)
#define SET_VENTILACION_DATA        SETBIT(FLAG,6)
#define CLEAR_RESET                 CLEARBIT(app.instruccion,0) 

#define IS_RESET_REQ                (app.instruccion == 0x01)
#define IS_VENTILAR_REQ             (app.instruccion == 0x02)
#define IS_PID_TUNNING_REQ          (app.instruccion == 0x03)
#define IS_CAL_REQ                  (app.instruccion == 0x04)

ErrorCode error;
bool pidAire = false; 
bool pidOxigeno = false; 
bool pidConcentracion = false; 

int main(void) {  
    while (1) {
        if (inicializacion()) {
            app.sendError(INIT_OK);  
            CLEAR_RESET;    
            while (!IS_RESET_REQ) {       
                
                if (GET_VENT_PARAMS_FLAG) {  
                        monitorPowerSuply.getInfoCarga();
                        if(app.getCommands()){
                            setParametros(false);
                        } 
                        CLEAR_VENT_DATA_PARAMS_FLAG;
                }
                
                if (SEND_VENT_DATA_FLAG) { 
                       if (IS_PID_TUNNING_REQ){                        
                           app.ventilacion.data.flujo = mezclador.data;
                           app.sendCommand(APP_CMD_SINGLE_DATA); //aprox 3ms
                       }else if(IS_VENTILAR_REQ){
                           app.sendCommand(APP_CMD_VENTILACION_DATA);                            
                       }
                       CLEAR_SEND_VENT_DATA_FLAG;                        
                }                
                
                if(IS_PID_TUNNING_REQ){                    
                    monitorPowerSuply.cerrarValvulaSeguridad(true); 
                    if (app.getCommand(true,APP_CMD_PROCESOS_CONSTANTES_PID,0)) { //14ms o 0.3ms operando
                        setParametros(true);
                        if(app.procesos.constantesPID.indice==4){
                            pidAire = true;
                        }else{
                            pidAire = false;
                        }
                         if(app.procesos.constantesPID.indice==3){
                            pidConcentracion = true;
                        }else{
                            pidConcentracion = false;
                        }
                        if(app.procesos.constantesPID.indice==5){
                            pidOxigeno = true;
                        }else{
                            pidOxigeno = false;
                        }
                    }
                    if  (pidAire){                        
                        mezclador.mezclarAire();                 
                    }  
                    if  (pidOxigeno){
                        mezclador.mezclarOxigeno();
                    }        
                    if  (pidConcentracion){ 
                        if (READ_MCP_FLAG) {  
                            leerMCP2();     
                            mezclador.cl  = app.ventilacion.data.fiO2*3065.9f - 3.6271f; 
                            mezclador.cl = mezclador.cl/100.0f;
                            CLEAR_READ_MCP_FLAG;
                        } 
                        mezclador.mezclar();
                    }
                } else if (IS_VENTILAR_REQ) {                    
                    if (READ_MCP_FLAG) {   
                        leerMCP2();
                        CLEAR_READ_MCP_FLAG;
                    }                                          
                    monitorPowerSuply.cerrarValvulaSeguridad(true);                  
                   
                    if (app.ventilacion.data.fase == 1 && !FLAG_TAKE_VTI) {
                        SET_FLAG_TAKE_VTI;
                    }
                    if (app.ventilacion.data.fase == 2 && FLAG_TAKE_VTI) {
                        app.ventilacion.data.vti = app.ventilacion.data.volumen;
                        CLEAR_FLAG_TAKE_VTI;
                    }
                    mezclador.fm = getFlujoMeta();
                    mezclador.cm = app.ventilacion.parametros.fiO2;
                    if (mezclador.mezclar()) {
                        t2 = cnt_ventilacion*TIMER3_PERIOD;
                        app.ventilacion.data.deltaT = (t2 - t1);
                        app.ventilacion.data.volumen += (app.ventilacion.data.deltaT)*(app.ventilacion.data.flujo / 60)*1000;
                        t1 = t2; 
                    }
                    if (app.ventilacion.data.fase == 1) {
                        valvulaExpiratoria.cerrar();
                    } else if (app.ventilacion.data.fase == 2) {
                        valvulaExpiratoria.controlPeep(app.ventilacion.parametros.peep);
                    }                        
                    if (sfmProxi.leerFlujo()) {
                        app.ventilacion.data.flujo = sfmProxi.flow;
                        app.sendCommand(APP_CMD_SINGLE_DATA);  
                    }    
                    
                } else {
                    monitorPowerSuply.cerrarValvulaSeguridad(false);
                }           
            }
            CLEAR_RESET;
            app.sendError(RESETING);
        } else {
            app.sendError(error);
        }
    }
}

bool inicializacion(void) {
  
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
                        setParametros(false);
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
                            setParametros(false);
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

void setParametros(bool test) {
    
        if (app.received.command == APP_CMD_PROCESOS_CONSTANTES_PID) {
            if (app.procesos.constantesPID.indice >= 0 && app.procesos.constantesPID.indice < 6) {
                mezclador.setConstatesPID(app.procesos.constantesPID.indice, app.procesos.constantesPID.constantes,test);
            }
            if (app.procesos.constantesPID.indice == 6) {
                presionPIDreq.constantes = app.procesos.constantesPID.constantes;
            } 
        }
        if (app.received.command == APP_CMD_PROCESOS_CALIBRACION) {
            if (app.procesos.calibracion.indice >= 0 && app.procesos.calibracion.indice < 6) {
                mezclador.setCalibracion(app.procesos.calibracion.indice, app.procesos.calibracion.m, app.procesos.calibracion.b);
            }
            if (app.procesos.calibracion.indice == 6) {
                presionInspM = app.procesos.calibracion.m;
                presionInspB = app.procesos.calibracion.b;
            }
            if (app.procesos.calibracion.indice == 7) {
                presionExpM = app.procesos.calibracion.m;
                presionExpB = app.procesos.calibracion.b;
            } 
        }
        if (app.received.command == APP_CMD_PROCESOS_INSTRUCCION) {
            app.instruccion = app.instruccion; 
        }
   
}

float getFlujoMeta(void) {
    if (app.ventilacion.data.fase == 1) {
        if (app.ventilacion.parametros.modo == VENTILACION_MVC) {
            if (app.ventilacion.parametros.formaOnda == VENTILACION_SQR) {
                app.ventilacion.parametros.flujo = (app.ventilacion.parametros.volumen / 1000.0) / (app.ventilacion.parametros.ti / 60);
                return app.ventilacion.parametros.flujo;
            } else if (app.ventilacion.parametros.formaOnda == VENTILACION_SIN) {
                app.ventilacion.parametros.fi = 1 / (app.ventilacion.parametros.ti / 60);
                app.ventilacion.parametros.qi = (app.ventilacion.parametros.volumen / 1000.0) / (2 * app.ventilacion.parametros.ti);
                app.ventilacion.parametros.flujo = app.ventilacion.parametros.qi * (0.06) * sin(3.1415926535898 * app.ventilacion.parametros.fi * (cnt_ventilacion * TIMER3_PERIOD));
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

    cnt_mcp++;
    cnt_mezclar++;
    cnt_ventilacion_data++;
    cnt_ventilacion_params++;
   
    
    if (cnt_ventilacion_data * TIMER3_PERIOD >= 0.020) {
        SET_VENTILACION_DATA;
        cnt_ventilacion_data = 0;
    }

    if (cnt_ventilacion_params * TIMER3_PERIOD >= 1) {
        SET_VENTILACION_PARAMS;
        cnt_ventilacion_params = 0;
    }

    if (cnt_mezclar * TIMER3_PERIOD >= 0.001) {
        SET_MEZCLAR;
        cnt_mezclar = 0;
    }

    if (cnt_mcp * TIMER3_PERIOD >= 0.250) {
        SET_MCP;
        cnt_mcp = 0;
    }

    if (IS_VENTILAR_REQ) {
        cnt_ventilacion++;
        if (cnt_ventilacion * TIMER3_PERIOD <= app.ventilacion.parametros.ti) {
            app.ventilacion.data.fase = 1;
        } else if ((cnt_ventilacion * TIMER3_PERIOD > app.ventilacion.parametros.ti) && (cnt_ventilacion * TIMER3_PERIOD <= app.ventilacion.parametros.tt)) {
            app.ventilacion.data.fase = 2;
        } else if (cnt_ventilacion * TIMER3_PERIOD > app.ventilacion.parametros.tt) {            
            app.ventilacion.data.ciclo++;
            app.ventilacion.data.volumen = 0;
            cnt_ventilacion = 0;
        }
    }

}

void leerMCP2() {
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
