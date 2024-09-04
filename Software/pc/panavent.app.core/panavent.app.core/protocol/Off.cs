using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    public class Off : PaqueteI2C
    {
        public override byte[] serializar()
        {
            base.on = false; 
            return base.getBytes(null);
        }
    }
}
