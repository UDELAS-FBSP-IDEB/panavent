using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace panavent.app.core.comandos
{
    public class ConstantesPID : ComandoI2C
    {
        private Single kp;
        private Single ki;
        private Single kd;
        private Single maxOutput;
        private Single minOutput;
        private Single emaAlpha = 1;
        private byte indice; 
        public ConstantesPID(byte indice, Single kp, Single ki, Single kd, Single maxOutput, Single minOutput, Single emaAlpha)
        {
            _appCommandType = AppCommandType.APP_CMD_PROCESOS_CONSTANTES_PID;
            this.indice = indice;
            this.kp = kp;
            this.ki = ki;
            this.kd = kd;
            this.maxOutput = maxOutput;
            this.minOutput = minOutput;
            this.emaAlpha = emaAlpha;
        }

        public ConstantesPID(byte[] buffer)
        {
            throw new NotImplementedException();
        }

        public Single Indice { get { return indice; } }
        public Single Kp { get { return kp; } }
        public Single Ki { get { return ki; } }
        public Single Kd { get { return kd; } }
        public Single MaxOutput { get { return maxOutput; } }
        public Single MinOutput { get { return minOutput; } }
        public Single EmaAlpha { get { return emaAlpha; } }
        public Single TestTarget { get; set; }

        public override byte[] Serializar()
        {
            List<byte> buff = new List<byte>();
            buff.AddRange(BitConverter.GetBytes(kp).Reverse());
            buff.AddRange(BitConverter.GetBytes(ki).Reverse());
            buff.AddRange(BitConverter.GetBytes(kd).Reverse());
            buff.AddRange(BitConverter.GetBytes(maxOutput).Reverse());
            buff.AddRange(BitConverter.GetBytes(minOutput).Reverse());
            buff.AddRange(BitConverter.GetBytes(emaAlpha).Reverse());
            buff.AddRange(BitConverter.GetBytes(TestTarget).Reverse());
            buff.Add(indice);
            return GetBytes(buff.ToArray());
        }
    }
}
