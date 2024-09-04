using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
     

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
    public enum TipoParam
    {
        PID_IN_AIR_FLOW = 0,
        PID_IN_O2_FLOW = 1,
        P_PS = 2,
        PID_PROXIMAL_FLOW = 3,
        P_VI = 4,
        PID_PEEP = 5,
        P_VALVE_AIRE = 6,
        P_VALVE_OXI = 7,
        P_VALVE_EXP = 8,
        P_LP = 9
    }
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
}
