using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core.protocol
{
    public class Mediciones /*: ICloneable*/
    {
        public Single flujo;
        public Single presion;
        public Single fiO2;
        public Single flujoAire;
        public Single flujoOxigeno;
        public Single presionAire;
        public Single presionOxigeno;
        public Single bateria;
        public Single volumen;
        public Single vti;
        public Single deltaT;
        public bool suministro;
        public int fase;
        public int ciclo;
        //public object Clone()
        //{
        //    return MemberwiseClone();
        //}
    }
}
