using panavent.app.core.comandos;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.monitor
{ 

    public class Parametro : INotifyPropertyChanged, ICloneable
    {
       
        public Parametro(string nombre, double minimo, double maximo )
        {
            Nombre = nombre;
            Minimo = minimo;
            Maximo = maximo;
            Format = "{0}";
        }
        double valor; 
        public double Valor { get { return valor; } set { valor = value; RaisePropertyChanged("Valor"); RaisePropertyChanged("Descripcion"); } }
        public string Nombre { get; set; }
        public double Minimo { get; set; }
        public double Maximo { get; set; }
        public double Step { get; set; }
        public double Tick { get; set; }
        public string Format { get; set; }
        public event PropertyChangedEventHandler PropertyChanged;
        protected void RaisePropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(name));
            }
        }
        public override string ToString()
        {
            return String.Format (Format, valor);
        }
        public string Descripcion { get { return String.Format(Format, valor); } }

        public object Clone()
        {
            return MemberwiseClone();
        }
    }
 
  
}
