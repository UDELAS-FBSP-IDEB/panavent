 
#ifndef pa_util_h
#define	pa_util_h

int PackFloat(void *buf, float x);
float UnpackFloat(const void *buf, int *i);
unsigned char GenerarCheckSum(const unsigned char * data, unsigned char len);
unsigned char ValidarCheckSum(const unsigned char revCheckSum, const unsigned char * data, unsigned char len);

#endif