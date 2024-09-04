using panavent.app.core;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Xml.Linq;
using Windows.Devices.Enumeration;
using Windows.Devices.SerialCommunication;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;


// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace panavent.app.monitor
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class Inicializar : Page
    { 
      
        public Inicializar()
        {
            this.InitializeComponent();
            var ini  = new InicializarVM();
            ini.Inicializado += InicializarVM_Inicializado;
            DataContext = ini;


        }

        private async void InicializarVM_Inicializado(object sender, InicializadoEventArgs e)
        {
            await Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(CoreDispatcherPriority.Normal,
            () =>
           {
               this.Frame.Navigate(typeof(Monitor));
           }
           );
           
        }

        private void btContinuar_Tapped(object sender, TappedRoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(Monitor));
        }

        private void btReintentar_Tapped(object sender, TappedRoutedEventArgs e)
        {
           ((InicializarVM)DataContext).InicializarBridge();
        }
    }


}
