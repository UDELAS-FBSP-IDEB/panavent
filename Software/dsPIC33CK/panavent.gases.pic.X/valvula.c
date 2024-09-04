 

#include "xc.h"
#include <math.h>
#include "valvula.h"


valvula valvulasStack[VALVE_STACK_SIZE];
int valveIx =0;
bool adc1CallBackSet = false; 
bool calConv = false;

valvula * inicializarValvula( 
        const struct PWM_INTERFACE * pwm, 
        enum ADC_CHANNEL ch, 
        enum tipoValvula Tipo, 
        enum estadoValvulaSeg Estado, 
        float CorrienteMinima, 
        float CorrienteMaxima, 
        float ShuntResistor,float SeriesResistor,float VoltajeOperacion){ 
    
    if( valveIx <VALVE_STACK_SIZE){ 
        valvula *v = &valvulasStack[valveIx]; 
        v->run = &runValvula;
        v->stop = &stopValvula;
        v->setCorrienteMeta = &setCorrienteMetaValvula; 
        v->setCorriente = &setCorrienteValvula;
        v->calcularPWM = &calcularPWMValvula;
        v->setPWM = &setPWMValvula;
        v->pwm = pwm; 
        v->ch = ch;
        v->tipo = Tipo;
        v->estado = Estado;
        v->estadoNormal = Estado;
        v->estadoPrevio = Estado;
        v->maxCorriente = CorrienteMaxima;
        v->minCorriente = CorrienteMinima;
        v->shuntResistor = ShuntResistor; 
        v->seriesResistor = SeriesResistor; 
        v->voltajeOperacion = VoltajeOperacion;
        v->_PWMdtc=0;
        v->corrienteMeta = 0;
        valveIx++;
        return  v;
    } else{
        return NULL;
    }
    
}
valvula * getValvula(enum ADC_CHANNEL ch){
    uint8_t i = 0;
    for(i=0; i < VALVE_STACK_SIZE; i++)
    {
        if(valvulasStack[i].ch ==    ch) {    
            return &valvulasStack[i];
        }
    }   
    return NULL;
};
void runValvula(enum ADC_CHANNEL ch){
    valvula *v = getValvula(ch);
    if(v!=NULL){
        if(!v->corriendo){
            if(v->tipo==PROPORCIONAL){ 
                v->pwm->PWM_Initialize();
                v->pwm->PWM_DutyCycleSet(0);
                v->pwm->PWM_Enable();  
            } 
            v->corriendo=true; 
        } 
    }
}; 
void stopValvula(enum ADC_CHANNEL ch){
    valvula *v = getValvula(ch);
    if(v!=NULL){
        v->corriendo=false; 
        v->pwm->PWM_Disable();
        valveIx--;
    }
};
 
void setCorrienteMetaValvula(enum ADC_CHANNEL ch, float corrienteMeta){
    valvula *v = getValvula(ch);
    if(v!=NULL){ 
        v->corrienteMeta = corrienteMeta;  
    }
};
void calcularPWMValvula(enum ADC_CHANNEL ch){
    valvula *v = getValvula(ch);
    if(v->corriendo && v->tipo==PROPORCIONAL){             
        float ea = (v->corrienteMeta-v->corriente);
        float PWMe = 0;
        PWMe = ea/v->maxCorriente;        
        v->_PWMdtc = v->_PWMdtc + PWMe/AMORTIGUADOR;
        v->_PWMdtc = v->_PWMdtc>1? 1 : v->_PWMdtc<0? 0 : v->_PWMdtc;        
        v->pwm->PWM_DutyCycleSet(v->_PWMdtc*FULL_PERIOD_PWM_CCP1PRL_VAL); 
    } 
};

void setPWMValvula(enum ADC_CHANNEL ch, float pct){
    valvula *v = getValvula(ch);
    pct = pct>1? 1 : pct<0? 0 : pct;      
    v->_PWMdtc = pct;
    v->pwm->PWM_DutyCycleSet(v->_PWMdtc*FULL_PERIOD_PWM_CCP1PRL_VAL); 
};

void setCorrienteValvula(enum ADC_CHANNEL ch, float corriente){
    valvula *v = getValvula(ch);
    if(v!=NULL){
        v->corriente = corriente;
        calcularPWMValvula(ch);
    }
}

