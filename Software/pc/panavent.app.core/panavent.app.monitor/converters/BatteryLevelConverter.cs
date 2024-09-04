using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Data;

namespace panavent.app.monitor.converters
{
    public class BatteryLevelConverter : IValueConverter
    {
        object IValueConverter.Convert(object value, Type targetType, object parameter, string language)
        {
            if (value == null) return null;
            switch ((double)value)
            {
                case double i when i >= 0 && i < 10:
                    return "\uE850";
                case double i when i >= 10 && i < 20:
                    return "\uE851";
                case double i when i >= 20 && i < 30:
                    return "\uE852";
                case double i when i >= 0 && i < 40:
                    return "\uE853";
                case double i when i >= 40 && i < 50:
                    return "\uE854";
                case double i when i >= 50 && i < 60:
                    return "\uE854";
                case double i when i >= 60 && i < 70:
                    return "\uE856";
                case double i when i >= 70 && i < 80:
                    return "\uE857";
                case double i when i >= 80 && i < 90:
                    return "\uE858";
                case double i when i >= 90:
                    return "\uE859";
                default:
                    return "\uE996";
            }
        }

        object IValueConverter.ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
