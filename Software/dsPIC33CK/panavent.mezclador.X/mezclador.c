  
#include <xc.h>
#include <stdbool.h>
#include <math.h>    // Para fabs()
#include <stdint.h> 
#include <stddef.h>
#include "mezclador.h" 
#include "mcpMezclador.h" 
#include "../panavent.hardware.drivers.X/mcc_generated_files/pwm/pwm_interface.h"  
#include "../panavent.hardware.drivers.X/mcc_generated_files/adc/adc1.h"
#include "../panavent.hardware.drivers.X/mcc_generated_files/pwm/sccp1.h" 
#include "../panavent.hardware.drivers.X/mcc_generated_files/pwm/sccp2.h" 
#include "../panavent.hardware.drivers.X/timer1Controller.h"
#define FCY 60000000UL
#include <libpic30.h> 


#define VALVE_NUM_MUESTRAS                  0.001/TIMER1_PERIOD  //equiv. # eventos en 1ms 
#define MEZCL_RESISTENCIA_MEDICION          0.1  
#define MEZCL_RES_TOTAL                     52.74
#define MEZCL_VALVE_IMAX                    0.200
#define MEZCL_VALVE_IMIN                    0
#define MEZCL_VALVE_VOLT                    10
#define CANAL_OXI   (AN3+3)
#define CANAL_AIRE  (AN4+3)
#define TIEMPO_CONV_MCP 0.005 //5ms
#define MEZCL_CAL_M_IL 1.0519
#define MEZCL_CAL_B_IL 0

bool leerMCP = false;
uint8_t chMCP = 1;
float tiempoMCP = 0;
Valvula vAire = {.pwm = &PWM2};
Valvula vOxi = {.pwm = &PWM1};
PIDrequest pidRequests[6];

float calMAir;
float calBAir;
float calMOxi;
float calBOxi;
float calMProxi;
float calBProxi;
float calMpAir;
float calBpAir;
float calMpOxi;
float calBpOxi;
float calMCOxi;
float calBCOxi; 
float lastFa=0;
float lastFaIm=0;
float tiempoMZL = 0;
int testIndex= -1;
 

bool airFilterInit=false;
bool o2FilterInit=false;

float o2Time = 0;
bool o2PID = false;

volatile  bool dispararConversion;


MovingAverageFilter o2Filter;
MovingAverageFilter airFilter;
MovingAverageFilter airFlowFilter;
MovingAverageFilter o2FlowFilter;
MovingAverageFilter airFlowFilter2;
LowPassFilter lp_filter;


MovingAverageFilter proxFlowFilter;

Mezclador mezclador = {
    .setCalibracion = &Mezclador_setCalibracion,
    .setConstatesPID = &Mezclador_setConstantesPID,
    .inicializar = &Mezclador_inicializar,
    .mezclar = &Mezclador_mezclar,
    .mezclarAire = &Mezclador_mezclarAire,
    .mezclarOxigeno = &Mezclador_mezclarOxigeno,
    .leerPresiones = &Mezclador_leerPresiones
};



ErrorCode Mezclador_inicializar() {

    
   
    init_filter(&o2Filter, 20); 
    init_filter(&airFilter, 20);  
    init_filter(&airFlowFilter, 10);    
    init_filter(&o2FlowFilter, 10);
    init_filter(&proxFlowFilter, 20);
    initLowPassFilter(&lp_filter, 170.0f, 10.0f);
     
    
    calMAir = 1;
    calBAir = 0;
    calMOxi = 1;
    calBOxi = 0;
    calMProxi = 1;
    calBProxi = 0;
    calMpAir = 1;
    calBpAir = 0;
    calMpOxi = 1;
    calBpOxi = 0;
    calMCOxi = 1;
    calBCOxi = 0;

    /*reset switch, sfaire, sfoxi*/
    PORTAbits.RA0 = 0;
    PORTBbits.RB11 = 0;
    PORTBbits.RB10 = 0;
    __delay_ms(50);
    PORTAbits.RA0 = 1;
    PORTBbits.RB11 = 1;
    PORTBbits.RB10 = 1;
    __delay_ms(50);

    /*inicializa sfm oxigeno*/
    if (!sfmOxi.inicializar()) {
        return SFM_O2_ERROR;
    }

    /*inicializa sfm aire*/
    if (!sfmAire.inicializar()) {
        return SFM_AIRE_ERROR;
    }



    /*inicializa el pwm usado por las valvulas proporcionales de aire y o2*/
    PWM1.PWM_Initialize();
    PWM1.PWM_DutyCycleSet(0);
    PWM1.PWM_Enable();
    
    PWM2.PWM_Initialize();
    PWM2.PWM_DutyCycleSet(0);
    PWM2.PWM_Enable();

    /*inicializa el adc usado para obtener la corriente en las valvulas*/
    ADC1.Initialize();
    ADC1.Enable();

    /*Se registra el callback del adc. En este callback se invocara el PID de las valvulas
     con cada conversion*/
    ADC1.adcMulticoreInterface->ChannelCallbackRegister(&Mezclador_conversionCallBack);
    ADCON3Lbits.CNVCHSEL = CANAL_AIRE;
    dispararConversion=true;
 
    /*inicializa el controlador pid, si ya no lo está*/
    pid.inicializar();

    /*inicializa el controlador del timer1 y registra el callback. En este callback
     se dispara la conversión*/
    timer1Controller.inicializar();
    timer1Controller.agregarCallback(&Mezclador_timerCallBack, TIMER1_PERIOD);
    return NO_ERROR;
}


void Mezclador_timerCallBack() {
    if (dispararConversion) {
        dispararConversion = false;
        if (ADCON3Lbits.CNVCHSEL == CANAL_OXI) {
            ADCON3Lbits.CNVCHSEL = CANAL_AIRE;
        } else {  
            ADCON3Lbits.CNVCHSEL = CANAL_OXI;
        }
        ADCON3Lbits.CNVRTCH = 1;    //dispara la conversión, que tomara aprox. 1.833uS
    }
    tiempoMCP += TIMER1_PERIOD; 
    if (tiempoMCP >= TIEMPO_CONV_MCP) {
        leerMCP = true;
    }
    o2Time += TIMER1_PERIOD; 
} 


void Mezclador_conversionCallBack(enum ADC_CHANNEL ch, uint16_t adcVal) {
    
    // Coversion a voltaje y división entre 100 para eliminar la ganancia.
    float volt = 0.00000805860805860806f * (float) adcVal; //0.0008058608058608060000 * 0.01 

    float il = MEZCL_CAL_M_IL * (volt / MEZCL_RESISTENCIA_MEDICION) + MEZCL_CAL_B_IL;

    // Factor común para ambos canales
    float corriente = ((int)(il * 1000) / 1000.0f);  // Redondeo a 3 decimales

    // Selecciona el filtro y el PID request en base al canal
    Valvula *vData;
    PIDrequest *pidReq;
    MovingAverageFilter *filter;
    bool isOxi=false;
    if (ch == (CANAL_OXI - 3)) {
        vData = &vOxi;
        pidReq = &pidRequests[0];
        filter = &o2Filter;
        isOxi=true;
    } else if (ch == (CANAL_AIRE - 3)) {
        vData = &vAire;
        pidReq = &pidRequests[1];
        filter = &airFilter;
        isOxi=false;
    } else {
        return; // Si no coincide con ningún canal esperado, salir de la función
    }

    // Aplicar filtro y actualizar corriente
    vData->corriente = filter_new_sample(filter, corriente);
    pidReq->meta = vData->corrienteMeta;
    pidReq->medicion = vData->corriente;
   
    if (testIndex==1 && !isOxi) {mezclador.data =  vData->corriente;}
    if (testIndex==0 && isOxi) {mezclador.data =  vData->corriente;}
    
    // Calcular y ajustar el ciclo de trabajo PWM
    if (pid.calcular(pidReq)) {
        int pwmDtc = pidReq->calculos.output * FULL_PERIOD_PWM_VAL;
        vData->pwm->PWM_DutyCycleSet(pwmDtc);
    }

    dispararConversion = true;
}


void Mezclador_leerPresiones(){
    if (leerMCP == true) {
        if (chMCP == 1) {
            if (mcpMezclador.LeerConversion(chMCP, &mezclador.presionAire)) {
                mezclador.presionAire = calMpAir * mezclador.presionAire + calBpAir;
                chMCP = 2;
            }
        } else {
            if (mcpMezclador.LeerConversion(chMCP, &mezclador.presionOxigeno)) {
                mezclador.presionOxigeno = calMpOxi * mezclador.presionOxigeno + calBpOxi;
                chMCP = 1;
            }
        }
        mcpMezclador.dispararConversion(chMCP);
        tiempoMCP = 0;
        leerMCP = false;
    }    
}

bool Mezclador_mezclar() {  
    mezclador.fa = (1 - mezclador.cm) * mezclador.fm / 0.791f;
    mezclador.fo = (mezclador.cm * mezclador.fm)-(0.209f * mezclador.fa);
    mezclador.data = Mezclador_mezclarAire();
    mezclador.data += Mezclador_mezclarOxigeno();  
    return true; 
}
 

float Mezclador_mezclarGas(Sfm *sensor, float *flujo, float calB, int pidIndex, int mezcladorIndex, float *corrienteMeta, bool *filterInit, MovingAverageFilter *filter, float *meta) { 
    if (!sensor->leerFlujo()) {
        pid.resetDeltaTime(&pidRequests[pidIndex]);
        return 0;
    }

    *flujo = (1 * sensor->flow) + calB;

    if (*meta == 0) {
        if (!*filterInit) {
            init_filter(filter, 10);
            *filterInit = true;  
        }
    } else {
        *flujo = filter_new_sample(filter, *flujo);
        *filterInit = false;
    }

    if (testIndex == mezcladorIndex) {
        mezclador.data = *flujo;
    }

    pidRequests[pidIndex].meta = *meta;
    pidRequests[pidIndex].medicion = *flujo;

    if (pid.calcular(&pidRequests[pidIndex])) { 
        *corrienteMeta = pidRequests[pidIndex].calculos.output;  
    } else {
        pid.resetDeltaTime(&pidRequests[pidIndex]);
    }
    
    return *flujo;
}
float Mezclador_mezclarAire(){ 
   return Mezclador_mezclarGas(&sfmAire, &mezclador.flujoAire, calBAir, 4, 4, &vAire.corrienteMeta, &airFilterInit, &airFlowFilter, &mezclador.fa);
}
float Mezclador_mezclarOxigeno(){ 
   return Mezclador_mezclarGas(&sfmOxi, &mezclador.flujoOxigeno, calBOxi, 5, 5, &vOxi.corrienteMeta, &o2FilterInit, &o2FlowFilter, &mezclador.fo);
}
void Mezclador_setConstantesPID(uint8_t index, PIDconstantes constantes, bool test) {
    pidRequests[index].constantes = constantes;
    if(test){
        testIndex= index;
        if (index == 0) {
            vOxi.corrienteMeta = constantes.testTarget; 
        } else if (index == 1) {
            vAire.corrienteMeta = constantes.testTarget; 
        } else if (index == 2) {
            mezclador.fm=constantes.testTarget; 
        } else if (index == 3) { 
            mezclador.cm = pidRequests[index].constantes.emaAlpha; 
            o2Time =0;
            o2PID = false;
            mezclador.fm= constantes.testTarget; 
        } else if (index == 4) {
            mezclador.fa=constantes.testTarget; 
        }else if (index == 5) {
            mezclador.fo=constantes.testTarget; 
        }
    }
}
void Mezclador_setCalibracion(uint8_t index, float M, float B) {
    if (index == 0) { 
        calMOxi = M;
        calBOxi = B;
    } else if (index == 1) {
        calMAir = M;
        calBAir = B;
    } else if (index == 2) {
        calMProxi = M;
        calBProxi = B;
    } else if (index == 3) {
        calMpAir = M;
        calBpAir = B;
    } else if (index == 4) {
        calMpOxi = M;
        calBpOxi = B;
    }else if (index == 5) {
        calMCOxi = M;
        calBCOxi = B;
    }
}

