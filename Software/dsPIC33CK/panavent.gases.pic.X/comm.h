#ifndef COMM_H
#define	COMM_H 

#include <stdbool.h>
#include <stdint.h>  
#define SOH 0x01
#define STX 0x02
#define ETX 0x03 
 
typedef void (*CommOnCommandReceived)(
    uint8_t command,
    const uint8_t *payload,
    uint8_t payloadLen
);


typedef struct {  
    uint8_t addr;
    CommOnCommandReceived  onCommandReceived; 
    bool (*getCommands)(); 
    bool (*getCommand)(bool waitForCmd, uint8_t waitCmdType, uint16_t waitCmdSequenceNumber);  
    bool (*sendCommandWithArgs)(uint8_t cmd, uint8_t * command, uint8_t len);
    bool (*sendCommandAndWait)(uint8_t cmd, uint8_t waitCmd);
    bool (*sendCommandAndWaitWithArgs)(uint8_t cmd, uint8_t waitCmd, uint8_t * command, uint8_t len);
    void (*inicializar)(CommOnCommandReceived callback);
} Comm;

extern Comm comm;

void Comm_inicializar(CommOnCommandReceived callback);
bool Comm_getCommands(); 
bool Comm_getCommand(bool waitForCmd, uint8_t waitCmdType, uint16_t waitCmdSequenceNumber); 
bool Comm_SendCommandAndWait(uint8_t cmd, uint8_t waitCmd);
bool Comm_sendCommandWithArgs(uint8_t cmd, uint8_t * command, uint8_t len);
bool Comm_sendCommandAndWaitWithArgs(uint8_t cmd, uint8_t waitCmd, uint8_t * command, uint8_t len); 


#endif	