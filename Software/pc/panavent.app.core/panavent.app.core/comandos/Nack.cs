
namespace panavent.app.core.comandos
{
    public class Nack : ComandoI2C
    {
        public Nack() { _appCommandType = AppCommandType.APP_CMD_PROTOCOLO_NACK ; }
 

        public override byte[] Serializar()
        {
            
            return GetBytes(null);
        }
    }
}
