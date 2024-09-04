using System;

namespace panavent.app.core.comandos
{
    public class Instruccion : ComandoI2C
    {
        private byte comando;
        public Instruccion(byte comando) { 
            _appCommandType = AppCommandType.APP_CMD_PROCESOS_INSTRUCCION ;
            Comando = comando;
        }

        public Instruccion(byte[] buffer)
        {
            if (buffer != null && buffer.Length >= 8 && (AppCommandType)buffer[2] == AppCommandType.APP_CMD_PROTOCOLO_PING )
            {
                SequenceNumber = (UInt16)(((UInt16)buffer[4] << 8) | (UInt16)buffer[3]); 
            }
        }
        public byte Comando {
            get { return comando; }
            set { comando = value; }
        }

        public override byte[] Serializar()
        {
            byte[] buffer = { (byte)comando };
            return GetBytes(buffer);
        }
    }
}
