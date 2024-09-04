
#include "protocolo.h" 

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


//#include "protocolo.h"
//#include "util.h"
//
//enum I2C_HOST_ERROR err;
//
//float t=0;
// 
//bool getDatos(Modulo modulo, Datos * data){   
//    uint8_t buffer[modulo.DataSize];
//    if (I2CMaster.Read(modulo.Direccion,  buffer, modulo.DataSize)){  
//        while(I2CMaster.IsBusy()){}; 
//        err = I2CMaster.ErrorGet();
//        if(err==I2C_HOST_ERROR_NONE){ 
//            if (ValidarCheckSum(buffer[modulo.DataSize-1], buffer, modulo.DataSize-1)){                
//                int i = 0;
//                switch (modulo.Tipo) {
//                  case comando:
//                    
//                    if((_ON(buffer[0]))==false || (_RESET(buffer[0]))==true){
//                         data->cmd  = buffer[0];
//                    }else {
//                        i=1;   
//                        if(_IS_MVENT(buffer[0])){
//                            data->parametros.volumen                =   UnpackFloat(&buffer[i], &i);
//                            data->parametros.fiO2                   =   UnpackFloat(&buffer[i], &i); 
//                            data->parametros.fr                     =   UnpackFloat(&buffer[i], &i);
//                            data->parametros.presion                =   UnpackFloat(&buffer[i], &i);
//                            data->parametros.peep                   =   UnpackFloat(&buffer[i], &i);
//                            data->parametros.pi                     =   UnpackFloat(&buffer[i], &i);
//                            data->parametros.ie                     =   buffer[25];                        
//                            t = 1.0/(data->parametros.fr/60.0); 
//                            data->parametros.ti = t*getFI(data->parametros.ie);
//                            data->parametros.te = t - data->parametros.ti;     
//                            data->parametros.tt = data->parametros.te + data->parametros.ti; 
//                            data->parametros.flujo = (data->parametros.volumen/1000.0)/(data->parametros.ti/60);
//                            data->cmd  = buffer[0];
//                        }else if(_IS_MSERV(buffer[0])){
//                            if (_KPID(buffer[0])){                  
//                                data->parametros.kp                 =   UnpackFloat(&buffer[i], &i); 
//                                data->parametros.ki                 =   UnpackFloat(&buffer[i], &i);
//                                data->parametros.kd                 =   UnpackFloat(&buffer[i], &i);
//                                data->parametros.maxOutput          =   UnpackFloat(&buffer[i], &i); 
//                                data->parametros.minOutput          =   UnpackFloat(&buffer[i], &i);
//                                data->parametros.emaAlpha           =   UnpackFloat(&buffer[i], &i);
//                                data->parametros.tipoParam = buffer[25]; 
//                            }   
//                            if (_ADJ(buffer[0])){     
//                                data->parametros.param              =   UnpackFloat(&buffer[i], &i);  
//                                data->parametros.tipoParam          =   buffer[5]; 
//                            } 
//                            if (_CAL(buffer[0])){     
//                                data->parametros.m                  =   UnpackFloat(&buffer[i], &i); 
//                                data->parametros.b                  =   UnpackFloat(&buffer[i], &i);  
//                                data->parametros.tipoParam          =   buffer[9]; 
//                            }
//                            data->cmd  = buffer[0];
//                        }
//                    } 
//                    return true; 
//                    break;
//                  case inspiratorio:
//                    data->mediciones.presion        =   UnpackFloat(&buffer[i], &i);
//                    data->mediciones.fiO2           =   UnpackFloat(&buffer[i], &i);  
//                    return true;
//                    break;
//                  case ps:
//                    data->mediciones.bateria        =   UnpackFloat(&buffer[i], &i);
//                    data->mediciones.suministro     =   buffer[i];
//                    return true;
//                    break;
//                  default: 
//                    break;
//                } 
//            }
//        }  
//    } 
//    return false;
//};
// 
//
//
//bool EnviarDatos(Modulo modulo, Datos * data){
//    uint8_t buffer[modulo.CommandSize];  
//    
//    bool procesar =true;
//    
//    switch (modulo.Tipo) {
//        case comando:
//            PackFloat(&buffer,    data->mediciones.flujo);
//            PackFloat(&buffer[4], data->mediciones.presion );
//            PackFloat(&buffer[8], data->mediciones.fiO2);
//            PackFloat(&buffer[12],data->mediciones.flujoAire);
//            PackFloat(&buffer[16],data->mediciones.flujoOxigeno);
//            PackFloat(&buffer[20],data->mediciones.presionAire);
//            PackFloat(&buffer[24],data->mediciones.presionOxigeno);
//            PackFloat(&buffer[28],data->mediciones.bateria);
//            PackFloat(&buffer[32],data->mediciones.volumen);  
//            buffer[36] = data->mediciones.suministro;
//            break;
//        case inspiratorio:     
//            buffer[0] = data->comando;  
//            break;
//        default: 
//            procesar=false;
//            break;
//    }
//    
//    if(procesar){
//        buffer[modulo.CommandSize-1] = GenerarCheckSum(buffer,modulo.CommandSize-1); 
//          enum I2C_HOST_ERROR err;
//          if (I2CMaster.Write(modulo.Direccion, buffer, modulo.CommandSize)){  
//              while(I2CMaster.IsBusy()){};  
//              err = I2CMaster.ErrorGet();
//              if(err==I2C_HOST_ERROR_NONE) {
//                  return true;
//              } 
//          }   
//    }
//    
//    return false;
//}
//
//bool Reset(){
//    //TODO;
//    return false;
//};
//
// float getFI(enum IE ie){
//            switch (ie)
//            {
//                case IE11:
//                   return 0.5;
//                   break;
//                case IE12:
//                  return 0.33;
//                  break;
//                case IE13:
//                  return 0.25;
//                  break;
//                case IE14:
//                  return 0.20;
//                  break;
//                case IE21:
//                  return 0.66;
//                  break;
//                case IE31:
//                  return 0.75;
//                  break;
//                case IE41:
//                  return 0.80;
//                  break;
//                default:
//                  return 0.33;
//                  break;
//            }        
//        }