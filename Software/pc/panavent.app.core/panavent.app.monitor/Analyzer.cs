using MathNet.Numerics;
using panavent.app.core.comandos;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace panavent.app.core
{
    public class Analyzer
    {
        float tiEms = 0;
        private int ciclo = 0;
        private int ixFase = -1;
        private int prevFase = 0;
        private int prevCiclo = 0;
        private List<VentilacionData> mediciones = new List<VentilacionData>();
        private Stopwatch sw= new Stopwatch();
        private object vlock = new object();  

        public float MaxPresion { get; set; }
        public float MinPresion { get; set; }
        public float MaxFlujo { get; set; }
        public float MinFlujo { get; set; }
        public float Vte { get; set; }
        public float Vti { get; set; }
        public float Volumen { get; set; }
        public float FTotal { get; set; }
        public float VolMinExp { get; set; }

        public event EventHandler<CicloEventArgs> NuevoCiclo;
         
        public void AddMediciones(VentilacionData med)
        {
            lock (vlock)
            {
                if (sw.IsRunning == false) sw.Start();
                if ((prevCiclo != med.Ciclo) && (med.Fase == 1) && mediciones.Count > 0)
                {
                    var inspFase = mediciones.Take(ixFase + 1);
                    var espFase = mediciones.Skip(ixFase + 1);
                    if (inspFase.Count() > 0 && espFase.Count() > 0)
                    {

                        float ems = (float)sw.ElapsedMilliseconds;
                        MaxPresion = inspFase.Select(a => a.Presion).Max();
                        MaxFlujo = inspFase.Select(a => a.Flujo).Max();

                        MinPresion = espFase.Select(a => a.Presion).Min();
                        MinFlujo = espFase.Select(a => a.Flujo).Min();

                        float txsInsp = tiEms / inspFase.Count();
                        float expEms = ems - tiEms;
                        float txsEsp = expEms / espFase.Count();
                        Vte = Math.Abs(espFase.Sum(a => (a.Flujo / 60) * txsEsp));
                        Vti = Math.Abs(inspFase.Sum(a => (a.Flujo / 60) * txsInsp)); 
                        if (ems > 0) FTotal = 60000 * (1 / ems);
                        Debug.WriteLine(String.Format("Peep: {0}", espFase.Select(a => a.Presion).Min()));
                        VolMinExp = Vte * FTotal / 1000;

                        ciclo++;

                        sw.Restart();
                        OnNuevoCiclo(med.Ciclo);
                        Volumen = 0;
                        mediciones.Clear();
                    }

                }
                mediciones.Add(med);
                float txs = (float)sw.ElapsedMilliseconds / mediciones.Count();
                Volumen += (med.Ciclo / 60) * txs;
                if ((prevFase == 1) && (med.Fase == 2))
                {
                    ixFase = mediciones.Count - 1;
                    tiEms = (float)sw.ElapsedMilliseconds;
                }
                prevFase = med.Fase;
                prevCiclo = med.Ciclo;
            } 
        }

        private void OnNuevoCiclo(long ciclo)
        {
            NuevoCiclo?.Invoke(this, new CicloEventArgs() { Ciclo = ciclo });
        }

    }
       
    public class CicloEventArgs : EventArgs
    {
        public long Ciclo { get; set; }  
    }



}
