using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    public class Adj : PaqueteI2C
    {
        public Single param;
        public TipoParam tipoParam;
        public override byte[] serializar()
        {
            base.modo = Modo.MSERV;
            base.subModo = SubModo.SERV_ADJ;
            base.on = true;
            List<byte> buff = new List<byte>();
            buff.AddRange(BitConverter.GetBytes(param).Reverse());
            buff.Add((byte)tipoParam);
            return base.getBytes(buff.ToArray());
        }
    }
}
