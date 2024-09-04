// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using panavent.app.core.comandos;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace panavent.app.monitor
{
    
    public sealed partial class AjusteModo : ContentDialog
    {

        MonitorVM mo;
        private List<Modo> modos = new List<Modo>();
        public AjusteModo(ref MonitorVM monitor)
        { 
            this.InitializeComponent();
            modos = new List<Modo>
            {
                Modo.VENTILACION_MVC,
                Modo.VENTILACION_MPC
            };

            cmModos.ItemsSource = modos; 

            ParamVentilacion = new ParametrosVentilacion();
            ParamVentilacion.Modo =  monitor.Modo;

            cmModos.SelectedIndex = modos.IndexOf(ParamVentilacion.Modo);
            ParamVentilacion.Presion = (Parametro)monitor.Presion.Clone();
            ParamVentilacion.VolumenTidal = (Parametro)monitor.VolumenTidal.Clone();
            ParamVentilacion.FrecuenciaRespiratoria = (Parametro)monitor.FrecuenciaRespiratoria.Clone();
            ParamVentilacion.Concentracion = (Parametro)monitor.Concentracion.Clone();
            ParamVentilacion.Peep = (Parametro)monitor.Peep.Clone();
            ParamVentilacion.Ie = (Parametro)monitor.Ie.Clone();
            ParamVentilacion.Pausa = (Parametro)monitor.Pausa.Clone();
            ParamVentilacion.Onda =  (Parametro)monitor.Onda.Clone();
            mo = monitor;
            DataContext = ParamVentilacion;
        }
      

        private void ContentDialog_PrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
            mo.Modo = ParamVentilacion.Modo;
            mo.Presion.Valor = ParamVentilacion.Presion.Valor;
            mo.VolumenTidal.Valor = ParamVentilacion.VolumenTidal.Valor;
            mo.FrecuenciaRespiratoria.Valor = ParamVentilacion.FrecuenciaRespiratoria.Valor;
            mo.Concentracion.Valor = ParamVentilacion.Concentracion.Valor;
            mo.Peep.Valor = ParamVentilacion.Peep.Valor;
            mo.Ie.Valor = ParamVentilacion.Ie.Valor;
            mo.Pausa.Valor = ParamVentilacion.Pausa.Valor;
            mo.Onda.Valor = ParamVentilacion.Onda.Valor;
            mo.Ventilar();
        }

        ParametrosVentilacion ParamVentilacion { get; set; }


        private void cmModos_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ParamVentilacion.Modo = modos[cmModos.SelectedIndex];
            if (ParamVentilacion.Modo == Modo.VENTILACION_MPC )
            {
                tblbVT.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                gVT.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                tblbPresion .Visibility = Windows.UI.Xaml.Visibility.Visible;
                gPresion .Visibility     = Windows.UI.Xaml.Visibility.Visible;
            }
            else
            {
                tblbVT.Visibility = Windows.UI.Xaml.Visibility.Visible ;
                gVT.Visibility = Windows.UI.Xaml.Visibility.Visible;
                tblbPresion.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
                gPresion.Visibility = Windows.UI.Xaml.Visibility.Collapsed;
            }
        }

        private void sdIE_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            txIE.Text = IeToString();
        }

        private void sdFr_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (sdFr.Value < ParamVentilacion.FrecuenciaRespiratoria.Minimo) { ParamVentilacion.FrecuenciaRespiratoria.Valor = ParamVentilacion.FrecuenciaRespiratoria.Minimo; }
            txFr.Text = ParamVentilacion.FrecuenciaRespiratoria.ToString();
        }

        private void sdFio_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (sdFio.Value < ParamVentilacion.Concentracion.Minimo) { ParamVentilacion.Concentracion.Valor = ParamVentilacion.Concentracion.Minimo; }
            txFio.Text = ParamVentilacion.Concentracion .ToString();
        }

        private void sdPeep_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (sdPeep.Value < ParamVentilacion.Peep.Minimo) { ParamVentilacion.Peep.Valor = ParamVentilacion.Peep.Minimo; }
            txPeep.Text = ParamVentilacion.Peep.ToString();
        }

        private void sdPausa_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (sdPausa.Value < ParamVentilacion.Pausa.Minimo) { ParamVentilacion.Pausa.Valor = ParamVentilacion.Pausa.Minimo; }
            txPausa.Text = ParamVentilacion.Pausa.ToString();
        }

        private void sdOnda_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            imgOnda.Source = new BitmapImage(new Uri(String.Format("ms-appx://{0}", OndaToString()), UriKind.RelativeOrAbsolute));
        }

        private void sdVt_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (sdVt.Value < ParamVentilacion.VolumenTidal.Minimo) { ParamVentilacion.VolumenTidal.Valor = ParamVentilacion.VolumenTidal.Minimo; }
            txVT.Text = ParamVentilacion.VolumenTidal.ToString();
        }

        private string IeToString()
        {
            switch (ParamVentilacion.Ie.Valor )
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

        private string OndaToString()
        {
            switch (ParamVentilacion.Onda.Valor )
            {
                case 0:
                    return "/Assets/square_black.png";
                case 1:
                    return "/Assets/sine_black.png";
                case 2:
                    return "/Assets/desa_black.png";
                default:
                    return null;
            }
        }

        private void sdPresion_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if (sdPresion.Value < ParamVentilacion.Presion.Minimo) { ParamVentilacion.Presion.Valor = ParamVentilacion.Presion.Minimo; }
            txPresion.Text = ParamVentilacion.Presion.ToString();
        }
    }
}
