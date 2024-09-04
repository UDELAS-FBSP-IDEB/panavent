using Microsoft.Toolkit;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Data;

namespace panavent.app.monitor.converters
{
    public class StringFormatConverter : IValueConverter
    { 
        object IValueConverter.Convert(object value, Type targetType, object parameter, string language)
        {
            if (value == null || parameter ==null)
                return null;
            string formatString = parameter as string;
            return String.Format(formatString,  value);
        }

        object IValueConverter.ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
