using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace panavent.app.core.comandos
{
    public class Calibracion : ComandoI2C
    {
        private Single m;
        private Single b; 
        private byte indice;

        public Calibracion(byte indice, Single m, Single b)
        {
            _appCommandType = AppCommandType.APP_CMD_PROCESOS_CALIBRACION;
            this.indice = indice;
            this.m = m;
            this.b = b;
        }

        public Calibracion(byte[] buffer)
        {
            throw new NotImplementedException();
        }

        public Single Indice { get { return indice; } }
        public Single M { get { return m; } }
        public Single B { get { return b; } }

        public override byte[] Serializar()
        {
            List<byte> buff = new List<byte>();
            buff.AddRange(BitConverter.GetBytes(m).Reverse());
            buff.AddRange(BitConverter.GetBytes(b).Reverse());
            buff.Add(indice);
            return GetBytes(buff.ToArray());
        }
    }
}
