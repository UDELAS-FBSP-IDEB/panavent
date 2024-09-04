using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    public class MPC : PaqueteI2C
    {
        public MPC() { }
        public Single presion;
        public Single fiO2;
        public Single fr;
        public Single peep;
        public Single pausa;
        public IE ie;
        public Modo modo;
        public override byte[] serializar()
        {
            base.modo = Modo.MPC;
            base.subModo = SubModo.MPC_DEF;
            base.on = true;
            List<byte> buff = new List<byte>();
            buff.AddRange(BitConverter.GetBytes((Single)(0.00)).Reverse());
            buff.AddRange(BitConverter.GetBytes(fiO2).Reverse());
            buff.AddRange(BitConverter.GetBytes(fr).Reverse());
            buff.AddRange(BitConverter.GetBytes(presion).Reverse());
            buff.AddRange(BitConverter.GetBytes(peep).Reverse());
            buff.AddRange(BitConverter.GetBytes(pausa).Reverse());
            buff.Add((byte)ie);
            return base.getBytes(buff.ToArray());
        }
    }
}
