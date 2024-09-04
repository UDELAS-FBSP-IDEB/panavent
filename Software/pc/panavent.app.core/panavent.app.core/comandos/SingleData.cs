using System;
using System.Linq;

namespace panavent.app.core.comandos
{
    public class SingleData : ComandoI2C
    {
        private float data; 
        public SingleData(byte[] buffer)
        {
            if (buffer != null && buffer.Length >= 8 && (AppCommandType)buffer[2] == AppCommandType.APP_CMD_SINGLE_DATA)
            {
                SequenceNumber = (UInt16)(((UInt16)buffer[4] << 8) | (UInt16)buffer[3]);
                _appCommandType = AppCommandType.APP_CMD_VENTILACION_DATA; 
                data = BitConverter.ToSingle(buffer.Skip(6).ToArray().Take(4).Reverse().ToArray(), 0);
            }
        }
        public float Data
        {
            get { return data; }
            set { data = value; }
        }

        public override byte[] Serializar()
        {
           
            return GetBytes(null);
        }
    }
}
