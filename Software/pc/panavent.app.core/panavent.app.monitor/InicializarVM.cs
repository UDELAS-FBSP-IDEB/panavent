using ColorCode.Compilation.Languages;
using panavent.app.core;
using panavent.app.core.comandos;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
using Windows.Devices.Enumeration;
using Windows.Devices.SerialCommunication;
using Windows.UI.Core;
using Windows.UI.Xaml.Controls;

namespace panavent.app.monitor
{
    public class InicializarVM: INotifyPropertyChanged
    {
        private object vSyncLock = new object();
        private bool lanzado = false;
        private bool reintentar;
        private bool continuar;
        private string logString;
        public event PropertyChangedEventHandler PropertyChanged;
        public event EventHandler<InicializadoEventArgs> Inicializado;
        public InicializarVM() {
            Reintentar  = false;
            Continuar  = false;
            InicializarBridge();
        }
        public bool Reintentar { get { return reintentar; } set { reintentar = value; RaisePropertyChanged("Reintentar"); } }
        public bool Continuar { get { return continuar; } set { continuar = value; RaisePropertyChanged("Continuar"); } }
        public string LogString { get { return logString; } set { logString = value; RaisePropertyChanged("LogString"); } }
        protected async void RaisePropertyChanged(string name)
        {
            if (PropertyChanged != null)
            {
                await Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
               () =>
               {
                    
                   PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
               }
               );

            }
        }
        public async void InicializarBridge()
        {
            LogString = String.Format("{0}\n", "Inicializando...");
            var selector = SerialDevice.GetDeviceSelector("COM16");
            DeviceInformationCollection serialDeviceInfos = await DeviceInformation.FindAllAsync(selector);
            if (serialDeviceInfos != null && serialDeviceInfos.Count > 0)
            {

                var info = serialDeviceInfos[0];
                Comun.serialDeviceHandler = new SerialDeviceHandler(info, selector);

                if (!Comun.serialDeviceHandler.Connected)
                {
                    var r = await Comun.serialDeviceHandler.ConnectAsync();
                }

                LogString += String.Format("{0}\n", "Esperando conexión...");
                while (!Comun.serialDeviceHandler.Connected) {
                    LogString += String.Format("{0}\n", "Esperando conexión...");
                    Thread.Sleep(1000);
                }

                var bridge = Bridge.GetSerialInterface(Comun.serialDeviceHandler);
                bridge.ErrorCommandReceived += Bridge_ErrorCommandReceived;
                bridge.GetCalibracionCommandReceived += Bridge_GetCalibracionCommandReceived;
                bridge.GetConstantesPIDCommandReceived += Bridge_GetConstantesPIDCommandReceived;
                bridge.Start();

                if (bridge.Running)
                {
                    LogString += String.Format("{0}\n", "Bridge conectado...");
 
                }
                else
                {
                    LogString += String.Format("{0}\n", "Error al iniciar el bridge. 'Start' fallido...");
                    LogString += String.Format("{0}\n", "Presione reintentar...");
                    Reintentar = true;
                    if (Comun.serialDeviceHandler.Connected) { Comun.serialDeviceHandler.Disconnect(); }
                }

            }
            else
            {
                LogString += String.Format("{0}\n", "Error al iniciar el bridge. Conexión al puerto serial fallida...");
                LogString += String.Format("{0}\n", "Presione reintentar...");
                Reintentar = true;
            }
        }
        private void OnTimerEvent(object sender, ElapsedEventArgs e)
        {
           
        }
        private void Bridge_ErrorCommandReceived(object sender, ErrorCommandEventArgs e)
        {
            LogString += String.Format("{0}\n", e.Message);
            if (e.ErrorCode == ErrorCode.INIT_OK)
            {
                LogString += String.Format("{0}\n", "Inicializado...");
                Continuar  = true;
                OnInicializado();
            }
        }

        private void Bridge_GetConstantesPIDCommandReceived(object sender, GetConstantesPIDCommandEventArgs e)
        {

            ConstantesPID constPid = null;
            switch (e.Indice)
            {
                case 0:
                    constPid = new ConstantesPID(0, 5f, 300.185185f, 0.022f, 0, 0, 1); //OK
                    break;
                case 1:
                    constPid = new ConstantesPID(1, 5f, 300.185185f, 0.022f, 0, 0, 1); //OK
                    break;
                case 2:
                    constPid = new ConstantesPID(2, 0.01f, 6, 0.9f, 0, 0, 0.005f); //flujo proximal
                    break;
                case 3:
                    constPid = new ConstantesPID(3, 0.01f, 6, 0.9f, 0, 0, 0.005f); //CORREGIR oxigeno
                    break;
                case 4:
                    //constPid = new ConstantesPID(4, 0.002f, 0.029850746f, 0.00008844f, 0, 0, 1); ; //OK
                    constPid = new ConstantesPID(4, 0.0015f, 0.022f, 0.00003f, 0, 0.19f, 1); ; //OK
                    break;
                case 5:
                    //constPid = new ConstantesPID(5, 0.002f, 0.029850746f, 0.00008844f, 0, 0, 1); //OK
                    constPid = new ConstantesPID(5, 0.0015f, 0.022f, 0.00003f, 0, 0.19f, 1); ; //OK
                    break;
                case 6:
                    constPid = new ConstantesPID(6, 8f, 15f, 0.125f, 0, 0, 1); //presion soporte
                    break;
                case 7:
                    constPid = new ConstantesPID(7, 0.7f, 0.6f, 0f, 1, 0, 0.005f); //peep
                    break;
                default:
                    return;

            }


            if (constPid != null) {
                constPid.SequenceNumber = e.SequenceNumber;
                ((Bridge)sender).EnqueuePacket(constPid);
            } 

        }
        private void Bridge_GetCalibracionCommandReceived(object sender, GetCalibracionCommandEventArgs e)
        {
            Calibracion cal = new Calibracion(e.Indice, 1, 0) { SequenceNumber = e.SequenceNumber };
            switch (e.Indice)
            { 
                case 1: 
                    cal = new Calibracion(e.Indice, 1.05263f, 0) { SequenceNumber = e.SequenceNumber };     
                    break;
            }
             
            ((Bridge)sender).EnqueuePacket(cal);
        }
        private void OnInicializado()
        {
            lock (vSyncLock)
            {
                if (lanzado==false)
                {
                    Inicializado?.Invoke(this, new InicializadoEventArgs());
                    lanzado = true;
                }                
            }
           
        }
    }

    public class InicializadoEventArgs : EventArgs
    {

    }
}
