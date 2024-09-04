
#include "utilities.h" 



float UnpackFloat(const void *buf, int *i){
    const unsigned char *b = (const unsigned char *)buf;
    unsigned long int temp = 0;
    *i += 4;
    temp = (((unsigned long int)b[0] << 24) |
            ((unsigned long int)b[1] << 16) |
            ((unsigned long int)b[2] <<  8) |
             (unsigned long int)b[3]);
    return *((float *) &temp);
}

int PackFloat(void *buf, float x) {
    unsigned char *b = (unsigned char *)buf;
    unsigned char *p = (unsigned char *) &x;
    b[0] = p[3];
    b[1] = p[2];
    b[2] = p[1];
    b[3] = p[0];
    return 4;
};

unsigned char GenerarCheckSum(const unsigned char * data, unsigned char len){
    unsigned char crc = 0;
    unsigned char crc_bit; 
    unsigned int ix;
    for (ix = 0; ix < len; ++ix) {
        crc ^= (data[ix]);
        for (crc_bit = 8; crc_bit > 0; --crc_bit) {
                if (crc & 0x80)
                    crc = (crc << 1) ^ 0x31;
                else
                    crc = (crc << 1);
        }
    }
    return crc;
}

unsigned char ValidarCheckSum(const unsigned char checkSum, const unsigned char* data, unsigned char len){  
    unsigned char CalcCheckSum= GenerarCheckSum(data, len);
    if(CalcCheckSum==checkSum){
        return 1;
    }else{
        return 0;
    }
}


// Inicializa el filtro
void init_filter(MovingAverageFilter* filter, int num_samples) {
    if (num_samples > MAX_SAMPLES) return;  // Evita sobrepasar el límite

    filter->num_samples = num_samples;
    int i;
    for(i = 0; i < num_samples; i++) {
        filter->samples[i] = 0.0f;
    }
    filter->sum = 0.0f;
    filter->index = 0;
    filter->initialized = false;
}

// Procesa una nueva muestra y devuelve la muestra filtrada
float filter_new_sample(MovingAverageFilter* filter, float new_sample) {
    if (!filter->initialized) {
        int i;
        for(i = 0; i < filter->num_samples; i++) {
            filter->samples[i] = new_sample;
        }
        filter->sum = filter->num_samples * new_sample;
        filter->initialized = true;
        return new_sample;
    }

    // Resta la muestra más antigua
    filter->sum -= filter->samples[filter->index];
    // Agrega la nueva muestra
    filter->samples[filter->index] = new_sample;
    filter->sum += new_sample;
    // Calcula el promedio
    float filtered_sample = filter->sum / filter->num_samples;
    // Actualiza el índice
    filter->index = (filter->index + 1) % filter->num_samples;

    return filtered_sample;
}


// Inicializa el filtro pasa bajo
void initLowPassFilter(LowPassFilter *filter, float sample_rate, float cutoff_freq) {
    float w0 = 2 * PI * cutoff_freq;
    float alpha = w0 / (sample_rate + w0);

    filter->a0 = alpha;
    filter->a1 = alpha;
    filter->b1 = 1 - alpha;

    filter->prev_input = 0.0f;
    filter->prev_output = 0.0f;
}

// Procesa la señal de entrada a través del filtro pasa bajo
float processLowPassFilter(LowPassFilter *filter, float input) {
    float output = filter->a0 * input + filter->a1 * filter->prev_input - filter->b1 * filter->prev_output;

    filter->prev_input = input;
    filter->prev_output = output;

    return output;
}
