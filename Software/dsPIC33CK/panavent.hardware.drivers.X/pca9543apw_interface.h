#ifndef PCA9543APW_H
#define	PCA9543APW_H
#include <xc.h>
#include <stdbool.h> 

typedef struct { 
    uint8_t addr;
    bool (*setCanal)(uint8_t canal);
    bool (*getCanal)(uint8_t canal);
    bool (*disable)();
} PCA9543APW;

bool PCA9543APW_setCanal(uint8_t addr, uint8_t canal);
bool PCA9543APW_getCanal(uint8_t addr, uint8_t canal);
bool PCA9543APW_disable(uint8_t addr);

#endif