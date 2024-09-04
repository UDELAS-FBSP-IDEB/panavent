using MathNet.Numerics;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.comandos
{
    public class VentilacionData : ComandoI2C
    {
        private float flujo;
        private float presion;
        private float fiO2;
        private float flujoAire;
        private float flujoOxigeno;
        private float presionAire;
        private float presionOxigeno;
        private float bateria;
        private float volumen;
        private float vti;
        private float deltaT;
        private bool suministro;
        private int fase;
        private int ciclo;
        public VentilacionData() { _appCommandType = AppCommandType.APP_CMD_VENTILACION_DATA; }
        public VentilacionData(byte[] bytes)
        {
            _appCommandType  = AppCommandType.APP_CMD_VENTILACION_DATA;
            byte[] data = bytes.Skip(4).ToArray();
            flujo = BitConverter.ToSingle(data.Take(4).Reverse().ToArray(), 0);
            presion = BitConverter.ToSingle(data.Skip(4).Take(4).Reverse().ToArray(), 0);
            fiO2 = BitConverter.ToSingle(data.Skip(8).Take(4).Reverse().ToArray(), 0);
            flujoAire = BitConverter.ToSingle(data.Skip(12).Take(4).Reverse().ToArray(), 0);
            flujoOxigeno = BitConverter.ToSingle(data.Skip(16).Take(4).Reverse().ToArray(), 0);
            presionAire = BitConverter.ToSingle(data.Skip(20).Take(4).Reverse().ToArray(), 0);
            presionOxigeno = BitConverter.ToSingle(data.Skip(24).Take(4).Reverse().ToArray(), 0);
            bateria = BitConverter.ToSingle(data.Skip(28).Take(4).Reverse().ToArray(), 0);
            volumen = BitConverter.ToSingle(data.Skip(32).Take(4).Reverse().ToArray(), 0);
            vti = BitConverter.ToSingle(data.Skip(36).Take(4).Reverse().ToArray(), 0);
            deltaT = BitConverter.ToSingle(data.Skip(40).Take(4).Reverse().ToArray(), 0);
            suministro = data.Skip(44).Take(1).ToArray()[0] == 0x01 ? true : false;
            fase = data.Skip(45).Take(1).ToArray()[0];
            ciclo = data.Skip(46).Take(1).ToArray()[0];
        }

        public float Flujo { get { return flujo; } set { flujo = value ; } }
        public float Presion { get { return presion; } }
        public float FiO2 { get { return fiO2; } }
        public float FlujoAire { get { return flujoAire; } }
        public float FlujoOxigeno { get { return flujoOxigeno; } }
        public float PresionAire { get { return presionAire; } }
        public float PresionOxigeno { get { return presionOxigeno; } }
        public float Bateria { get { return bateria; } }
        public float Volumen { get { return volumen; } }
        public float Vti { get { return vti; } }
        public float DeltaT { get { return deltaT; } }
        public bool Suministro { get { return suministro; } }
        public int Fase { get { return fase; } }
        public int Ciclo { get { return ciclo; } }

        public override byte[] Serializar()
        {
            throw new NotImplementedException();
        }

    }


}

