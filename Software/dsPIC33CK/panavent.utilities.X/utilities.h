

#ifndef UTILITIES_H
#define	UTILITIES_H

#define CHECKBIT(B, BIT)             ((B >> BIT) & 1)==1
#define CLEARBIT(B,BIT)              B &= ~(1 << BIT)
#define SETBIT(B,BIT)                B |= 1 << BIT
#define TOGGLE(B,BIT)                B ^= 1 << BIT
 


#define MAX_SAMPLES 50  // Define el tamaño máximo para el buffer de muestras

#include <stdbool.h>


#define PI 3.14159265358979323846

typedef struct {
    float a0, a1;
    float b1;
    float prev_input;
    float prev_output;
} LowPassFilter;

typedef enum {
    NO_ERROR = 0,
    INIT_OK = 1,
    SFM_O2_ERROR = 128,
    SFM_AIRE_ERROR = 129,
    SFM_PROX_ERROR = 130,
    MCP3426_ERROR = 131,
    MCP3428_ERROR = 132,
    VALVE_AIRE_ERROR = 133,
    INIT_ERROR = 134,
    CANAL_PCA9543APW_ERROR = 135,
    DIS_PCA9543APW_ERROR = 136,
    MCPMODULO1_ERROR = 137,
    MCPMODULO2_ERROR = 138,
    VALVE_O2_ERROR = 139,
    VALVE_EXP_ERROR = 140,
    RESETING = 141,
    MOMPS_ERROR = 142,
    PING_ERROR = 143,
    GET_PID_PARAM_ERROR = 144,
    GET_CALIBRACION_ERROR = 145
} ErrorCode;


typedef struct {
    float samples[MAX_SAMPLES];
    float sum;
    int num_samples;
    int index;
    bool initialized;
} MovingAverageFilter;

// Prototipos de funciones
void init_filter(MovingAverageFilter* filter, int num_samples);
float filter_new_sample(MovingAverageFilter* filter, float new_sample);


int PackFloat(void *buf, float x);
float UnpackFloat(const void *buf, int *i);
unsigned char GenerarCheckSum(const unsigned char * data, unsigned char len);
unsigned char ValidarCheckSum(const unsigned char revCheckSum, const unsigned char * data, unsigned char len);
void initLowPassFilter(LowPassFilter *filter, float sample_rate, float cutoff_freq);
float processLowPassFilter(LowPassFilter *filter, float input);

#endif	 

