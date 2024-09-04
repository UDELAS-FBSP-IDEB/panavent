using System;

namespace panavent.app.core.comandos
{
    public class Ping : ComandoI2C
    {
        public Ping() { _appCommandType = AppCommandType.APP_CMD_PROTOCOLO_PING;  }

        public Ping(byte[] buffer)
        {
            if (buffer != null && buffer.Length >= 8 && (AppCommandType)buffer[2] == AppCommandType.APP_CMD_PROTOCOLO_PING )
            {
                SequenceNumber = (UInt16)(((UInt16)buffer[4] << 8) | (UInt16)buffer[3]); 
            }
        }
        public override byte[] Serializar()
        {
            return base.GetBytes(null);
        }
    }
}
