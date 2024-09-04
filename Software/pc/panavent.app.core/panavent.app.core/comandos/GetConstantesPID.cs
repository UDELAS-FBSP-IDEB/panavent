using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace panavent.app.core.comandos
{
    public class GetConstantesPID : ComandoI2C
    {
        private byte indice;
        public GetConstantesPID()
        {
            _appCommandType = AppCommandType.APP_CMD_PROCESOS_GET_CONSTANTES_PID;
        }

        public GetConstantesPID(byte[] buffer)
        {
            if (buffer != null && buffer.Length >= 8 && (AppCommandType)buffer[2] == AppCommandType.APP_CMD_PROCESOS_GET_CONSTANTES_PID)
            {
                SequenceNumber = (UInt16)(((UInt16)buffer[4] << 8) | (UInt16)buffer[3]);
                indice =  buffer[6];
            }
        }

        public byte Indice { get { return indice; } }

        public override byte[] Serializar()
        {
            byte[] buffer = { (byte)indice };
            return GetBytes(buffer);
        }
    }
}



