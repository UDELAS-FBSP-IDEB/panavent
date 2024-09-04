using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    public  class Kpid : PaqueteI2C
    {
        public Kpid(TipoParam tp) {
            tipoParam = tp;
        }
        public Single kp;
        public Single ki;
        public Single kd;
        public Single maxOutput;
        public Single minOutput;
        public Single emaAlpha=1;
        public TipoParam tipoParam;
        public override byte[] serializar()
        {
            base.modo = Modo.MSERV;
            base.subModo = SubModo.SERV_KPID;
            base.on = true;
            List<byte> buff = new List<byte>();
            buff.AddRange(BitConverter.GetBytes(kp).Reverse());
            buff.AddRange(BitConverter.GetBytes(ki).Reverse());
            buff.AddRange(BitConverter.GetBytes(kd).Reverse());
            buff.AddRange(BitConverter.GetBytes(maxOutput).Reverse());
            buff.AddRange(BitConverter.GetBytes(minOutput).Reverse());
            buff.AddRange(BitConverter.GetBytes(emaAlpha).Reverse());
            buff.Add((byte)tipoParam);
            return base.getBytes(buff.ToArray());
        }
    }
}
