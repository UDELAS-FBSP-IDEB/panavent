using panavent.app.core;
using panavent.app.core.comandos;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.ServiceModel.Channels;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace panavent.app.monitor
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Monitor : Page
    {
        MonitorVM mo = new MonitorVM();
        MediaElement me = new MediaElement();
        IRandomAccessStream stream;


        private static bool run = false;
        private static float Kp;
        private static float Ki;
        private static float Kd;
        private static float Alpha;
        private static float min;
        private static float max;
        private static byte index;
        private static float target;
        private static System.Timers.Timer myTimer;
        private static Bridge bridge;

        public Monitor()
        {
            this.InitializeComponent();
            DataContext = mo;
            tbIE.Text = IeToString(mo.Ie.Valor);
            imgOnda.Source = new BitmapImage(new Uri(String.Format("ms-appx://{0}", OndaToString(mo.Onda.Valor)), UriKind.RelativeOrAbsolute));

            //ApplicationViewTitleBar formattableTitleBar = ApplicationView.GetForCurrentView().TitleBar;
            //formattableTitleBar.ButtonBackgroundColor = Colors.Transparent;
            //CoreApplicationViewTitleBar coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            //coreTitleBar.ExtendViewIntoTitleBar = true;

            bPresion.Visibility= Visibility.Collapsed;
            mo.AlarmaGenerada += Mo_AlarmaGenerada;
            MostrarVentanaModo();
            _ = cargarAudiosAsync();


            List<String> _enumval = new List<String>() { "Valvula Oxigeno", "Valvula Aire", "Flujo proximal", "Concentracion O2", "Flujo Aire", "Flujo Oxigeno" };
            lbParams.ItemsSource = _enumval.ToList();
            lbParams.SelectedIndex = 0;
            bridge = Bridge.GetSerialInterface();
            myTimer = new System.Timers.Timer(3000); // Intervalo de 1 segundo
            myTimer.Elapsed += OnTimedEvent;
            myTimer.AutoReset = true;
            myTimer.Enabled = true;



        }

        private static void OnTimedEvent(Object source, ElapsedEventArgs e)
        {
            
            
            if (run)
            {
                
                ConstantesPID pid = new ConstantesPID(
                index, Kp,
                Ki,
                Kd,
                max,
                min,
                Alpha);

                myTimer.Enabled = false;
                pid.TestTarget = target;
                bridge.EnqueuePacket(pid);
                Thread.Sleep(3000);
                pid.TestTarget = 0;
                bridge.EnqueuePacket(pid);
                myTimer.Enabled = true;
                
            }

        }
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var bridgee = Bridge.GetSerialInterface();
            run = false;
            Thread.Sleep(10);
            Instruccion instruccion = new Instruccion(0x03);
            bridgee.EnqueuePacket(instruccion);
            Thread.Sleep(10);
            index = (byte)lbParams.SelectedIndex;
            Kp = float.Parse(txKp.Text);
            Ki = float.Parse(txKi.Text);
            Kd = float.Parse(txKd.Text);
            max = float.Parse(txMax.Text);
            min = float.Parse(txMin.Text);
            Alpha = float.Parse(txEmaAlpha.Text);
            target = float.Parse(txTestValue.Text);

            ConstantesPID pid = new ConstantesPID(
               index, Kp,
               Ki,
               Kd,
               max,
               min,
               Alpha);
            pid.TestTarget = target;
            //bridge.EnqueuePacket(pid);

            run = true;
        }
        private void lbParams_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
        }

        async Task cargarAudiosAsync() {
          

            var folder = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFolderAsync("Audios");
            var file = await folder.GetFileAsync("audio1.mp3");
            stream = await file.OpenAsync(Windows.Storage.FileAccessMode.Read);
         
        }

        private void Mo_AlarmaGenerada(object sender, AlarmaEventArgs e)
        {
            _ = Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
               () =>
               {  
                 
                   me.SetSource(stream, "");
                   me.Play();
               }
                );

           
        }

        private void btPid_Click(object sender, RoutedEventArgs e)
        {
            var D = new PIDTunning();
            _ = D.ShowAsync();
        }

        private void Rectangle_Tapped(object sender, TappedRoutedEventArgs e)
        {
          
            var D = new AjusteParametro(mo.VolumenTidal);
            D.ParametroActualizado += ParametroActualizado;
            _ = D.ShowAsync();
        }
        

        private void rectFR_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var D = new AjusteParametro(mo.FrecuenciaRespiratoria);
            D.ParametroActualizado += ParametroActualizado;
            _ = D.ShowAsync();
        }

        private void rectC_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var D = new AjusteParametro(mo.Concentracion);
            D.ParametroActualizado += ParametroActualizado;
            _ = D.ShowAsync();
        }

        private void rectPeep_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var D = new AjusteParametro(mo.Peep);
            D.ParametroActualizado += ParametroActualizado;
            _ = D.ShowAsync();
        }

        private void rectIE_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var D = new AjusteParametro(mo.Ie);
            D.ParametroActualizado += IeActualizado;
            _ = D.ShowAsync();
        }

        private void IeActualizado(object sender, ParametroEventArg e)
        {
            tbIE.Text = IeToString(e.Parametro.Valor);
            ParametroActualizado(sender, e);
        }

        private void rectPausa_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var D = new AjusteParametro(mo.Pausa);
            D.ParametroActualizado += ParametroActualizado;
            _ = D.ShowAsync();
        }
        private void rectPresion_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var D = new AjusteParametro(mo.Presion);
            D.ParametroActualizado += ParametroActualizado;
            _ = D.ShowAsync();
        }
        private void rectOnda_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var D = new AjusteParametro(mo.Onda);
            D.ParametroActualizado += OndaActualizado;
            _ = D.ShowAsync();
        }

        private void OndaActualizado(object sender, ParametroEventArg e)
        {
            imgOnda.Source = new BitmapImage(new Uri(String.Format("ms-appx://{0}", OndaToString(e.Parametro.Valor)), UriKind.RelativeOrAbsolute));
            ParametroActualizado (sender,e);
        }
        private void ParametroActualizado(object sender, ParametroEventArg e)
        {
           if (mo.Ventilando)
            {
                mo.Ventilar();
            }
        }

        private string IeToString(double val)
        {
            switch (val)
            {
                case (int)IE.IE11:
                    return "1:1";
                case (int)IE.IE12:
                    return "1:2";
                case (int)IE.IE13:
                    return "1:3";
                case (int)IE.IE14:
                    return "1:4";
                case (int)IE.IE21:
                    return "2:1";
                case (int)IE.IE31:
                    return "3:1";
                case (int)IE.IE41:
                    return "4:1";
                default:
                    return null;
            }
        }
        private string OndaToString(double val)
        {
            switch (val)
            {
                case 0:
                    return "/Assets/square.png";
                case 1:
                    return "/Assets/sine.png";
                case 2:
                    return "/Assets/desa.png";
                default:
                    return null;
            }
        }

        private   void btModos_Click(object sender, RoutedEventArgs e)
        {
             MostrarVentanaModo();
        }

        private async void MostrarVentanaModo()
        {
            var D = new AjusteModo(ref mo);
            await D.ShowAsync();
            imgOnda.Source = new BitmapImage(new Uri(String.Format("ms-appx://{0}", OndaToString(mo.Onda.Valor)), UriKind.RelativeOrAbsolute));
            tbIE.Text = IeToString(mo.Ie.Valor);
            if (mo.Modo == Modo.VENTILACION_MPC )
            {
                bPresion.Visibility = Visibility.Visible;
                bVT.Visibility = Visibility.Collapsed;
                tbModo.Text = "PC";
            }
            else
            {
                bPresion.Visibility = Visibility.Collapsed;
                bVT.Visibility = Visibility.Visible;
                tbModo.Text = "VC";
            }
        }

        private async void btVentilar_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var D = new PIDTunning();
            await D.ShowAsync();
        }
 
    }
}
