using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    public class Ack : PaqueteI2C
    {
        public override byte[] serializar()
        {
            base.modo = Modo.ACK; 
            return base.getBytes(null);
        }
    }
}
