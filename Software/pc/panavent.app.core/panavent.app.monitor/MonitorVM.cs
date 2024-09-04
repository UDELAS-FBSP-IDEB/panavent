using panavent.app.core;
using panavent.app.core.comandos;
using RealTimeGraphX.DataPoints;
using RealTimeGraphX.Renderers;
using RealTimeGraphX.UWP;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
using System.Xml.Linq;
using Windows.ApplicationModel.Core;
using Windows.Devices.Enumeration;
using Windows.Devices.SerialCommunication;
using Windows.Storage;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;

namespace panavent.app.monitor
{
    public class MonitorVM : ParametrosVentilacion, INotifyPropertyChanged
    {

        private double x = 0;

        private Bridge bridge;
        private Analyzer analyzer;
        private double pip;
        private double vte;
        private double ftotal;
        private double volMinExp;
        private double bateria;
        private bool configurado;
        private bool enSuministro;
        private int alarmLevel;
        private string alarmMessage;
        public event EventHandler<AlarmaEventArgs> AlarmaGenerada;

        public MonitorVM()
        {
            configurado = false;
            CntrlPresion = new UwpGraphController<DoubleDataPoint, DoubleDataPoint>();
            CntrlPresion.Range.MinimumY = 0;
            CntrlPresion.Range.MaximumY = 60;
            CntrlPresion.Range.MaximumX = 2000;
            CntrlPresion.Renderer = new SimpleLineRenderer<UwpGraphDataSeries>();
            CntrlPresion.Range.AutoY = false;
            CntrlPresion.DataSeriesCollection.Add(new UwpGraphDataSeries()
            {
                Name = "Series",
                Stroke = Colors.Red,
            });

            CntrlFlujo = new UwpGraphController<DoubleDataPoint, DoubleDataPoint>();
            CntrlFlujo.Range.MinimumY = -80;
            CntrlFlujo.Range.MaximumY = 40;
            CntrlFlujo.Range.MaximumX = 2000;
            CntrlFlujo.Renderer = new SimpleLineRenderer<UwpGraphDataSeries>();
            CntrlFlujo.Range.AutoY = false;
            CntrlFlujo.RefreshRate = new TimeSpan(0, 0, 0, 0, 1);
            CntrlFlujo.DataSeriesCollection.Add(new UwpGraphDataSeries()
            {
                Name = "Series",
                Stroke = Colors.DodgerBlue,
            });

            CntrlVolumen = new UwpGraphController<DoubleDataPoint, DoubleDataPoint>();
            CntrlVolumen.Range.MinimumY = 0;
            CntrlVolumen.Range.MaximumY = 1200;
            CntrlVolumen.Range.MaximumX = 2000;
            CntrlVolumen.Renderer = new SimpleLineRenderer<UwpGraphDataSeries>();
            CntrlVolumen.Range.AutoY = false;
            CntrlVolumen.DataSeriesCollection.Add(new UwpGraphDataSeries()
            {
                Name = "Series",
                Stroke = Colors.Green,
            });

            Presion = new Parametro("Presión control", 5, 35) { Valor = 10, Step = 1, Tick = 1, Format = "{0} cmH2O" };
            VolumenTidal = new Parametro("Volumen Tidal", 100, 1500) { Valor = 500, Step = 5, Tick = 50, Format = "{0} ml" };
            FrecuenciaRespiratoria = new Parametro("Frecuencia Respiratoria", 8, 35) { Valor = 12, Step = 1, Tick = 1, Format = "{0} r/min" };
            Concentracion = new Parametro("Concentracion", 21, 100) { Valor = 21, Step = 1, Tick = 1, Format = "{0}%" };
            Peep = new Parametro("PEEP", 0, 20) { Valor = 0, Step = 1, Tick = 1, Format = "{0} cmH2O" };
            Ie = new Parametro("I:E", 0, 6) { Valor = 1, Step = 1, Tick = 1, Format = "{0}" };
            Pausa = new Parametro("Pausa", 0, 2) { Valor = 0, Step = 0.01, Tick = 0.05, Format = "{0}s" };
            Onda = new Parametro("Onda", 0, 2) { Valor = 0, Step = 1, Tick = 1, Format = "{0}" };

            analyzer = new Analyzer();
            analyzer.NuevoCiclo += Analyzer_NuevoCiclo;
            InicializarBridge();

            System.Timers.Timer timer = new System.Timers.Timer(1);
            timer.Elapsed += OnTimerEvent;
            timer.Start();

            System.Timers.Timer timer2 = new System.Timers.Timer(6000);
            timer2.Elapsed += OnTimer2Event;
            timer2.Start();

            PIP = 0;
            VTE = 0;
            Ftotal = 0;
            VolMinExp = 0;
            Bateria = 0;


        }
        private void Analyzer_NuevoCiclo(object sender, CicloEventArgs e)
        {
            PIP = analyzer.MaxPresion;
            VTE = analyzer.Vte;
            Ftotal = analyzer.FTotal;
            VolMinExp = analyzer.VolMinExp;
            CntrlPresion.Range.MaximumY = 60;
            //CntrlFlujo.Range.MaximumY = analyzer.MaxFlujo + 10;
            //CntrlVolumen.Range.MaximumY = analyzer.Vti + 100;
        }
        public bool Ventilando { get; set; }
        public double PIP { get { return pip; } set { pip = value; RaisePropertyChanged("PIP"); } }
        public double VolMinExp { get { return volMinExp; } set { volMinExp = value; RaisePropertyChanged("VolMinExp"); } }
        public double Ftotal { get { return ftotal; } set { ftotal = value; RaisePropertyChanged("Ftotal"); } }
        public double VTE { get { return vte; } set { vte = value; RaisePropertyChanged("VTE"); } }
        public double Bateria { get { return bateria; } set { bateria = value; RaisePropertyChanged("Bateria"); } }
        public bool EnSuministro { get { return enSuministro; } set { enSuministro = value; RaisePropertyChanged("EnSuministro"); } }
        public int AlarmLevel { get { return alarmLevel; } set { alarmLevel = value; RaisePropertyChanged("AlarmLevel"); } }
        public string AlarmMessage { get { return alarmMessage; } set { alarmMessage = value; RaisePropertyChanged("AlarmMessage"); } }
        ConcurrentQueue<VentilacionDataCommandEventArgs> medicionesQueque = new ConcurrentQueue<VentilacionDataCommandEventArgs>();
        public UwpGraphController<DoubleDataPoint, DoubleDataPoint> CntrlPresion { get; set; }
        public UwpGraphController<DoubleDataPoint, DoubleDataPoint> CntrlVolumen { get; set; }
        public UwpGraphController<DoubleDataPoint, DoubleDataPoint> CntrlFlujo { get; set; }
        public void Ventilar()
        {                   
            VentilacionParams parametros  = new VentilacionParams ();
            parametros.Presion = (float)Presion.Valor;
            parametros.FiO2 = (float)Concentracion.Valor / 100f;
            parametros.FR  = (float)FrecuenciaRespiratoria.Valor;
            parametros.PEEP  = (float)Peep.Valor;
            parametros.Pausa = (float)Pausa.Valor;
            parametros.IE = (IE)Ie.Valor;
            parametros.Modo = Modo;
            parametros.Volumen = (float)VolumenTidal.Valor;
            parametros.FormaOnda = FormaOnda.VENTILACION_SQR;
            Ventilando = true;
            bridge.EnqueuePacket(parametros);
            Instruccion instruccion = new Instruccion(0x02);
            bridge.EnqueuePacket(instruccion);

        }
        private void OnTimerEvent(object sender, ElapsedEventArgs e)
        {
            if (Ventilando )
            {
                CntrlPresion.PushData(x, 0);
                CntrlFlujo.PushData(x, 0);
                CntrlVolumen.PushData(x, 0);
                x += 1;
                if (x > CntrlPresion.Range.MaximumX)
                {
                    x = 0;
                    try
                    {
                        ((LimGraphSurface)CntrlPresion.Surface).ClonePoints();
                        CntrlPresion.Clear();
                        ((LimGraphSurface)CntrlFlujo.Surface).ClonePoints();
                        CntrlFlujo.Clear();
                        ((LimGraphSurface)CntrlVolumen.Surface).ClonePoints();
                        CntrlVolumen.Clear();
                    }
                    catch (Exception)
                    {

                    }

                };
            }
        }
        bool trgAlarm;
        private void OnTimer2Event(object sender, ElapsedEventArgs e)
        {
            //ProcesarAlarma(Alarma.SuministroOxigeno);
            trgAlarm = true;
        }
        void ProcesarAlarma(Alarma alarma)
        {
            switch (alarma)
            {
                case Alarma.SuministroOxigeno:
                    AlarmLevel = 2;
                    AlarmMessage = "No se detecta suministro de OXIGENO o presión de OXIGENO baja!";
                    break;
                case Alarma.SuministroAire:
                    AlarmLevel = 2;
                    AlarmMessage = "No se detecta suministro de AIRE o presión de AIRE baja!";
                    break;
                case Alarma.InterrupcionSuministroElectrico:
                    AlarmLevel = 1;
                    AlarmMessage = "Atención: suministro eléctrico interrumpido. Funcionando a batería.";
                    break;

            }
            if (alarma != Alarma.None)
            {
                if (trgAlarm)
                {
                    //OnAlarmaGenerada(alarma);
                    trgAlarm = false;
                }
            }
        }
        List<Alarma> CalcularAlarmaPorMediciones(VentilacionData mediciones)
        {
            List<Alarma> alarmas = new List<Alarma>();
            if (mediciones.PresionOxigeno < 35)
            {
                alarmas.Add(Alarma.SuministroOxigeno);
            }
            if (mediciones.PresionAire < 35)
            {
                alarmas.Add(Alarma.SuministroAire);
            }
            if (mediciones.Suministro == false)
            {
                alarmas.Add(Alarma.InterrupcionSuministroElectrico);
            }
            return alarmas;
        }            
        private void InicializarBridge()
        {
            bridge = Bridge.GetSerialInterface(Comun.serialDeviceHandler);
            bridge.VentilacionDataCommandReceived += Bridge_MedicionRecibida;
        }
        private void Bridge_MedicionRecibida(object sender, VentilacionDataCommandEventArgs e)
        {
            if (1==1)
            {
                analyzer.AddMediciones(e.Data);
                x += 1;
                CntrlPresion.PushData(x, e.Data.Presion);
                CntrlFlujo.PushData(x, e.Data.Flujo);
                CntrlVolumen.PushData(x, analyzer.Volumen);
                
                CntrlFlujo.Range.MaximumX = 2000; 
                CntrlFlujo.Range.AutoY = true; 
                CntrlFlujo.Range.MaximumY  = 35;

                var alarmas = CalcularAlarmaPorMediciones(e.Data);
                if (alarmas.Count() > 0)
                {
                    ProcesarAlarma(alarmas.OrderByDescending(a => a).First());
                }
                else
                {
                    AlarmLevel = 0;
                    AlarmMessage = "";
                }
                //CntrlFlujo.Range.AutoY = true;
                //CntrlFlujo.Range.MinimumY = -5;
                if (x > CntrlPresion.Range.MaximumX)
                {
                    x = 0;
                    try
                    {
                        ((LimGraphSurface)CntrlPresion.Surface).ClonePoints();
                        CntrlPresion.Clear();
                        ((LimGraphSurface)CntrlFlujo.Surface).ClonePoints();
                        CntrlFlujo.Clear();
                        ((LimGraphSurface)CntrlVolumen.Surface).ClonePoints();
                        CntrlVolumen.Clear();
                    }
                    catch (Exception)
                    { }

                };
            }
            Bateria = e.Data.Bateria;
            EnSuministro = e.Data.Suministro;
        }
        private void ConfigurarBridge()
        {
            if (configurado == false)
            {
                


                 
                 

                //Kpid kpVI = new Kpid(TipoParam.P_VI)
                //{
                //    kp = (float)0.01,
                //    ki = (float)0.032,
                //    kd = (float)0
                //};

                 

                //Cal cal = new Cal
                //{
                //    tipoParam = TipoParam.PID_PROXIMAL_FLOW,
                //    b = (float)-0.0145336,
                //    m = (float)1.085668
                //};

                //Cal calVI = new Cal
                //{
                //    tipoParam = TipoParam.P_VI,
                //    b = (float)14.97,
                //    m = (float)1.112
                //};

                //Cal calPS = new Cal
                //{
                //    tipoParam = TipoParam.P_PS,
                //    b = -1.76f,
                //    m = 1f
                //};

                //Cal calLP = new Cal
                //{
                //    tipoParam = TipoParam.P_LP,
                //    b = -0.2519f,
                //    m = 0.7315f
                //};

                //bridge.enqueuePacket(kpAire);
                //bridge.enqueuePacket(kpOxi);
                //bridge.enqueuePacket(kpProxi);
                //bridge.enqueuePacket(kpPEEP);
                //bridge.enqueuePacket(kpPS);
                //bridge.enqueuePacket(kpVI);
                //bridge.enqueuePacket(cal);
                //bridge.enqueuePacket(calVI);
                //bridge.enqueuePacket(calPS);
                //bridge.enqueuePacket(calLP);
                //configurado = true;
            }
        }
        public event PropertyChangedEventHandler PropertyChanged;
        protected async void RaisePropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                await Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
               () =>
               {
                   PropertyChanged(this, new PropertyChangedEventArgs(name));
               }
               );

            }
        }

        private void OnAlarmaGenerada(Alarma alarma)
        {
            AlarmaGenerada?.Invoke(this, new AlarmaEventArgs() { alarma = alarma });
        }

    }

    public class AlarmaEventArgs : EventArgs
    {
        public Alarma alarma;
    }

    public enum Alarma
    {
        None,
        InterrupcionSuministroElectrico = 3,
        NivelBateriaBajo = 100,
        SuministroOxigeno = 101,
        SuministroAire = 102
    }

}
