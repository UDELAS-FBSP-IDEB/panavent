using System;



namespace panavent.app.core.comandos
{
    public class Error : ComandoI2C
    {
        protected  ErrorCode errorCode;
        protected string descripcion;
        public Error() { 
            _appCommandType = AppCommandType.APP_CMD_PROTOCOLO_ERROR_CODE; 
        }

        public Error(byte[] buffer)
        {
            if (buffer != null && buffer.Length >= 8 && (AppCommandType)buffer[2] == AppCommandType.APP_CMD_PROTOCOLO_ERROR_CODE)
            {
                SequenceNumber= (UInt16)(((UInt16)buffer[4] << 8) | (UInt16)buffer[3]);
                errorCode = (ErrorCode)buffer[6];
                descripcion = String.Format("{0} - {1}", DateTime.Now, errorCode.ToString());                
            }
        }
        public ErrorCode ErrorCode { get { return errorCode;  } }
        public string Descripcion { get { return descripcion; } }
       
        public override byte[] Serializar()
        {            
            return GetBytes(null);
        }
    }
}
