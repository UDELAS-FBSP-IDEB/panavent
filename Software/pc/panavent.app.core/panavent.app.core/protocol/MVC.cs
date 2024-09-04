using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    public class MVC : PaqueteI2C
    {
        public MVC(SubModo sub) {
            _sub = sub;
        }
        private SubModo _sub;
        public Single volumen;
        public Single fiO2;
        public Single fr;
        public Single peep;
        public Single pausa;
        public IE ie;
        public SubModo sub {
            get { return _sub; }
        } 
        public override byte[] serializar()
        {
            base.modo = Modo.MVC;
            base.subModo = sub;
            base.on = true;
            List<byte> buff = new List<byte>();
            buff.AddRange(BitConverter.GetBytes(volumen).Reverse());
            buff.AddRange(BitConverter.GetBytes(fiO2).Reverse());
            buff.AddRange(BitConverter.GetBytes(fr).Reverse());
            buff.AddRange(BitConverter.GetBytes((Single)(0.00)).Reverse());
            buff.AddRange(BitConverter.GetBytes(peep).Reverse());
            buff.AddRange(BitConverter.GetBytes(pausa).Reverse());
            buff.Add((byte)ie);
            return base.getBytes(buff.ToArray());
        }
    }
}
