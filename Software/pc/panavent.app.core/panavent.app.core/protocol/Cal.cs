using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    public class Cal : PaqueteI2C
    {
        public Single m;
        public Single b;
        public TipoParam tipoParam;
        public override byte[] serializar()
        {
            base.modo = Modo.MSERV;
            base.subModo = SubModo.SERV_CAL;
            base.on = true;
            List<byte> buff = new List<byte>();
            buff.AddRange(BitConverter.GetBytes(m).Reverse());
            buff.AddRange(BitConverter.GetBytes(b).Reverse());
            buff.Add((byte)tipoParam);
            return base.getBytes(buff.ToArray());
        }
    }
}
