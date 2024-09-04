
using System.Collections.Generic;
using System;
using System.Linq;

namespace panavent.app.core.comandos
{
    public abstract class ComandoI2C
    {
        protected AppCommandType _appCommandType;
        public ComandoI2C() { }
        public AppCommandType AppCommandType { get { return _appCommandType; } }
        public UInt16 SequenceNumber { get; set; }
        protected byte[] GetBytes(byte[] data)
        {
            List<byte> buff = new List<byte>();
            buff.Add(0x01);
            buff.Add(0x02);
            buff.Add((byte)AppCommandType);
            buff.Add(((byte)SequenceNumber));
            buff.Add(((byte)(SequenceNumber >> 8)));
            if (data == null) {
                buff.Add(0);
            } else {
                buff.Add((byte)(data.Length));
                buff.AddRange(data);
            }
            buff.Add(0x03);
            buff.Add(GenerarCheckSum(buff.ToArray()));
            return buff.ToArray();
        }
        public abstract byte[] Serializar();
        public  static byte GenerarCheckSum(byte[] data)
        {
            byte crc = 0;
            byte crc_bit;
            int ix;
            for (ix = 0; ix < data.Length; ix++)
            {
                crc ^= (data[ix]);
                for (crc_bit = 8; crc_bit > 0; --crc_bit)
                {
                    if ((crc & 0x80) == 0x80)
                        crc = (byte)((crc << 1) ^ 0x31);
                    else
                        crc = (byte)(crc << 1);
                }
            }
            return crc;
        }
        public static bool ValidarCheckSum(byte[] data)
        {
            byte calcCheckSum = GenerarCheckSum(data.Take(data.Length -1).ToArray());
            if (calcCheckSum == data[data.Length - 1])
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    public enum AppCommandType
    {
        APP_CMD_PROTOCOLO_ERROR_CODE = 1,
        APP_CMD_PROTOCOLO_PING = 2,
        APP_CMD_PROTOCOLO_ACK = 3,
        APP_CMD_PROTOCOLO_NACK = 4,
        APP_CMD_VENTILACION_PARAMETROS = 5,
        APP_CMD_VENTILACION_DATA = 6,
        APP_CMD_PROCESOS_INSTRUCCION = 7,
        APP_CMD_PROCESOS_CALIBRACION = 8,
        APP_CMD_PROCESOS_CONSTANTES_PID = 9,
        APP_CMD_PROCESOS_GET_CALIBRACION = 10,
        APP_CMD_PROCESOS_GET_CONSTANTES_PID = 11,
        APP_CMD_SINGLE_DATA = 12
    }

    public enum ErrorCode
    {
        NO_ERROR = 0,
        INIT_OK = 1,
        SFM_O2_ERROR = 128,
        SFM_AIRE_ERROR = 129,
        SFM_PROX_ERROR = 130,
        MCP3426_ERROR = 131,
        MCP3428_ERROR = 132,
        VALVE_AIRE_ERROR = 133,
        INIT_ERROR = 134,
        CANAL_PCA9543APW_ERROR = 135,
        DIS_PCA9543APW_ERROR = 136,
        MCPMODULO1_ERROR = 137,
        MCPMODULO2_ERROR = 138,
        VALVE_O2_ERROR = 139,
        VALVE_EXP_ERROR = 140,
        RESETING = 141,
        MOMPS_ERROR = 142,
        PING_ERROR = 143
    }
    //public enum TipoParam
    //{
    //    PID_IN_AIR_FLOW = 0,
    //    PID_IN_O2_FLOW = 1,
    //    P_PS = 2,
    //    PID_PROXIMAL_FLOW = 3,
    //    P_VI = 4,
    //    PID_PEEP = 5,
    //    P_VALVE_AIRE = 6,
    //    P_VALVE_OXI = 7,
    //    P_VALVE_EXP = 8,
    //    P_LP = 9
    //}
    public enum IE
    {
        IE11,
        IE12,
        IE13,
        IE14,
        IE21,
        IE31,
        IE41
    };

    public enum Modo {
        VENTILACION_MVC = 1,
        VENTILACION_MPC = 2
    }

    public enum FormaOnda {
        VENTILACION_SQR = 1,
        VENTILACION_SIN = 2,
        VENTILACION_DES = 3
    }

}
