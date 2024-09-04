using panavent.app.core.comandos;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    public abstract class  PaqueteI2C : HeaderI2C 
    {
        protected PaqueteI2C() { }
        protected byte[] getBytes(byte[] data)
        {
           
            List<byte> buff = new List<byte>();
            buff.Add(base.getByte());
            
            if (data != null && data.Length<=25)
            {
                buff.AddRange(data);
            }
            else
            {
                if(data != null)
                {
                    throw new Exception("data no valida");
                } 
            }

            while(buff.Count < 28)
            {
                buff.Add(0x00);
            };
            buff.Add(0);
            buff[28] = ComandoI2C.GenerarCheckSum(buff.ToArray()) ;
            return buff.ToArray();

        } 
        public abstract byte[] serializar();
    }
}
