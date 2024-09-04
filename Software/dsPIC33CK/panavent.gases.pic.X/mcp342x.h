#ifndef MCP342X_H
#define	MCP342X_H
#include "mcc_generated_files/i2c_host/i2c1.h"  
#include "protocolo.h"
#define MCP342X_STACK_SIZE  2
#define MCP342X_CONFIG_REG 0x00; //0x04 //0x94; 
#define MCP3426_ADDR 0x68
#define MCP3428_ADDR 0x6E
#define MCP342X_LSB 0.001 //0.00025

typedef struct {
    uint8_t addr;
    uint8_t numCanales;
    float   ultimaConversion;
    bool (*dispararConversion)(uint8_t addr, uint8_t ch);
    bool (*LeerConversion)(uint8_t addr, uint8_t ch, float * valor);
} mcp342x;

mcp342x * inicializarMcp342x(uint8_t addr, uint8_t numCanales);
bool dispararConvercion_mcp342x(uint8_t addr, uint8_t ch);
bool leerConversion_mcp342x(uint8_t addr, uint8_t ch, float *valor);

#endif	