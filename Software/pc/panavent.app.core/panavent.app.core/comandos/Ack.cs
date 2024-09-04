

namespace panavent.app.core.comandos
{
    public class Ack : ComandoI2C
    {
         
        public Ack() { _appCommandType = AppCommandType.APP_CMD_PROTOCOLO_ACK ; }

        public override byte[] Serializar()
        { 
            return GetBytes(null);
        }
    }
}
