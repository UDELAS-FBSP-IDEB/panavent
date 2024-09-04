#include "mcpMezclador.h" 


#include "../panavent.hardware.drivers.X/pca9543apw_interface.h"

const Mcp342x mcpMezclador ={
    .addr = MCP3426_ADDR,
    .numCanales = 2,
    .LeerConversion= &McpMezclador_leerConversion,
    .dispararConversion = &McpMezclador_dispararConvercion
};

bool McpMezclador_dispararConvercion(uint8_t ch){
    if (PCA9543APW_setCanal(0x70, 1)){
        Mcp342x_dispararConvercion(mcpMezclador.addr, ch);
        return true;
    }else{
        return false;
    }
};

bool McpMezclador_leerConversion(uint8_t ch, float *valor){
    if (PCA9543APW_setCanal(0x70, 1)){
        return Mcp342x_leerConversion(mcpMezclador.addr, ch, valor);
    }else{
        return false;
    }
};
