using panavent.app.core;
using panavent.app.core.comandos;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.System.Profile;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Documents;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
 
// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace panavent.app.monitor
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class PIDTunning : ContentDialog
    {
        private static Bridge bridge;
        private static  bool run = false;
        private static float Kp ;
        private static float Ki;
        private static float Kd;
        private static float Alpha;
        private static float min;
        private static float max;
        private static byte index;
        private static float target;
        private static System.Timers.Timer myTimer;
        public PIDTunning()
        {
            this.InitializeComponent();
            List<String> _enumval= new List<String>() { "Valvula Oxigeno","Valvula Aire", "Flujo proximal", "Concentracion O2", "Flujo Aire", "Flujo Oxigeno"};
            lbParams.ItemsSource = _enumval.ToList();
            lbParams.SelectedIndex = 0;
            bridge = Bridge.GetSerialInterface();
            myTimer = new System.Timers.Timer(3000); // Intervalo de 1 segundo
            myTimer.Elapsed += OnTimedEvent;
            myTimer.AutoReset = true;
            myTimer.Enabled = true;

            Console.WriteLine("Presiona 'Enter' para detener el timer...");
            Console.ReadLine();
             

        }
        private static void OnTimedEvent(Object source, ElapsedEventArgs e)
        {
            ConstantesPID pid = new ConstantesPID(
            index, Kp,
            Ki,
            Kd,
            max,
            min,
            Alpha);
             
            if (run)
            {
                myTimer.Enabled = false;
                pid.TestTarget = target;
                bridge.EnqueuePacket(pid);
                Thread.Sleep (3000);
                pid.TestTarget = 0;
                bridge.EnqueuePacket(pid);
                myTimer.Enabled = true ;
            }
            else {
                pid.TestTarget = 0;
                bridge.EnqueuePacket(pid);
            }
        }       
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var bridge = Bridge.GetSerialInterface();
            run = false;
            Thread.Sleep(10);
            Instruccion instruccion = new Instruccion(0x03);
            bridge.EnqueuePacket(instruccion); 
            Thread.Sleep(10);
            index = (byte)lbParams.SelectedIndex;
            Kp = float.Parse(txKp.Text);
            Ki = float.Parse(txKi.Text);
            Kd = float.Parse(txKd.Text);
            max = float.Parse(txMax.Text);
            min = float.Parse(txMin.Text);
            Alpha = float.Parse(txEmaAlpha.Text);
            target = float.Parse(txTestValue.Text);
            run = true;   
        }
        private void lbParams_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
        }
    }
}
