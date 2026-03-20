using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace panavent.app.core.comandos
{
    public class SetPWM : ComandoI2C
    {
        private Single porcentaje; 
        private byte oxigeno; 
        public SetPWM(byte oxigeno, Single porcentaje)
        {
            _appCommandType = AppCommandType.APP_CMD_PROCESOS_SET_PWM;
            this.oxigeno = oxigeno;
            this.porcentaje = porcentaje; 
        }

        public SetPWM(byte[] buffer)
        {
            throw new NotImplementedException();
        }

        public byte Oxigeno { get { return oxigeno; } }
        public Single Porcentaje { get { return porcentaje; } } 

        public override byte[] Serializar()
        {
            List<byte> buff = new List<byte>();
            buff.AddRange(BitConverter.GetBytes(porcentaje).Reverse());
            buff.Add(oxigeno); 
            return GetBytes(buff.ToArray());
        }
    }
}
