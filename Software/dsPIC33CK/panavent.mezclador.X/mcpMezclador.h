#ifndef MCP_MEZCLADOR_H
#define	MCP_MEZCLADOR_H

#include "../panavent.hardware.drivers.X/mcp342x_interface.h"

bool McpMezclador_dispararConvercion(uint8_t ch);
bool McpMezclador_leerConversion(uint8_t ch, float *valor);

extern const Mcp342x mcpMezclador; 

#endif