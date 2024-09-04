// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using panavent.app.core.comandos;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media.Imaging;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace panavent.app.monitor
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class AjusteParametro : ContentDialog  
    {
        private Parametro pa;
        public event EventHandler<ParametroEventArg> ParametroActualizado;
        public AjusteParametro(Parametro parametro)
        {
            this.InitializeComponent();
            pa = parametro;
            Parametro = (Parametro)parametro.Clone();
            DataContext = Parametro;
            if (pa.Nombre == "Onda")
            {
                txVal.Text = "";
                imgOnda.Height = 48;
                imgOnda.Width = 48;
                imgOnda.Source = new BitmapImage(new Uri(String.Format("ms-appx://{0}", OndaDarkToString()), UriKind.RelativeOrAbsolute));
            }
            else
            {
                txVal.Text = Parametro.ToString();
            }
        }
        public Parametro Parametro { get; private set; }
        private void ContentDialog_PrimaryButtonClick(ContentDialog sender, ContentDialogButtonClickEventArgs args)
        {
            pa.Valor = Parametro.Valor;
           
            OnValorActualizado();
        }
        private void OnValorActualizado()
        {
            ParametroActualizado?.Invoke(this, new ParametroEventArg() { Parametro = Parametro });
        }

        private void sdVt_ValueChanged(object sender, Windows.UI.Xaml.Controls.Primitives.RangeBaseValueChangedEventArgs e)
        {
            if(sdVt.Value < Parametro.Minimo ) { Parametro.Valor = Parametro.Minimo; }
            switch(Parametro.Nombre)
            {
                case "I:E":
                    txVal.Text = IeToString();
                    break;
                case "Onda":
                    txVal.Text = "";
                    imgOnda.Source = new BitmapImage(new Uri(String.Format("ms-appx://{0}", OndaDarkToString()), UriKind.RelativeOrAbsolute));
                    break;
                default:
                    txVal.Text = Parametro.ToString();
                    break;
            }
            
        }

        private string IeToString()
        {
            switch (Parametro.Valor)
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
        private string OndaDarkToString()
        {
            switch (Parametro.Valor)
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

    }

    public class ParametroEventArg : EventArgs 
    {
        public Parametro Parametro { get; set; } 
    }

  
}
