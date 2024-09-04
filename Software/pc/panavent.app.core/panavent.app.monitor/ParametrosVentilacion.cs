using panavent.app.core.comandos;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.monitor
{
    public  class ParametrosVentilacion  
    {
        public Modo Modo { get; set; }
        public Parametro Presion { get; set; }
        public Parametro VolumenTidal { get; set; }
        public Parametro FrecuenciaRespiratoria { get; set; }
        public Parametro Concentracion { get; set; }
        public Parametro Peep { get; set; }
        public Parametro Ie { get; set; }
        public Parametro Pausa { get; set; }
        public Parametro Onda { get; set; }
     
    }
}
