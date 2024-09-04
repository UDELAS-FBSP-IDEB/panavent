using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    /*      X       X       XXX         XXX    */
    /*      ON      RESET   SUB-MODO    MODO   */
    public abstract class PaqueteI2CHeader
    {
        public PaqueteI2CHeader() { }
        public bool on { get; set; }
        public bool reset { get; set; }
        public SubModo subModo { get; set; }
        public Modo modo { get; set; }
        public byte getByte()
        {
            return (byte)((on ? 1 : 0) << 7 | (reset ? 1 : 0) << 6 | (byte)subModo  | (byte)modo);
        }
    }

    public enum SubModo
    {
        MVC_SQR = 0x08,
        MVC_SIN = 0x10,
        MVC_DES = 0x18,
        MPC_DEF = 0x08,
        SERV_KPID = 0x08,
        SERV_ADJ = 0x10,
        SERV_CAL = 0x18
    }
    public enum Modo
    {
        ACK = 0xFF,
        MVC = 0x01,
        MPC = 0x02,
        MSERV = 0x03
    }



}
