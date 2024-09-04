#ifndef COMUN_H
#define	COMUN_H

#define _MODO_TIPO(B)                   (B & 0x38) >> 3
#define _RESET_MODO(B)                  (B & 0xC7)  

/*MODOS DE OPERACION*/
#define _MVC(B)                         (B & 0x07) == 0x01 
#define _MPC(B)                         (B & 0x07) == 0x02   
#define _MSERV(B)                       (B & 0x07) == 0x03   

/*SUB MODOS DE VOLUMEN*/
#define _MVCSQR(B)                      _MODO_TIPO(B) == 0x01   
#define _MVCSIN(B)                      _MODO_TIPO(B) == 0x02   
#define _MVCDES(B)                      _MODO_TIPO(B) == 0x03 

/*SUB MODOS DE PRESION*/
#define _MPCDEF(B)                      _MODO_TIPO(B) == 0x01

/*MODOS DE SERVICIO*/
#define _KPID(B)                        _MODO_TIPO(B) == 0x01
#define _ADJ(B)                         _MODO_TIPO(B) == 0x02 
#define _CAL(B)                         _MODO_TIPO(B) == 0x03 


#define _IS_MVENT(B)                    ((_MVC(B)) || (_MPC(B))) 
#define _IS_MSERV(B)                    _MSERV(B) 

#define _ISACK(B)                       (B==0xFF)
#define _ISOFF(B)                       (B==0x00)
#define _ON(B)                          (!_ISACK(B) && CHECKBIT(B,7) && (_IS_MVENT(B) || _IS_MSERV(B))) 
#define _CLEAR_RESET(B)                 CLEARBIT(B,6) 
#define _RESET(B)                       (!_ISACK(B) && CHECKBIT(B,6) && (_IS_MVENT(B) || _IS_MSERV(B))) 


 

#endif	