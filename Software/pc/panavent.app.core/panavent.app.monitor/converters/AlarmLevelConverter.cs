using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Media;

namespace panavent.app.monitor.converters
{
    internal class AlarmLevelConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, string language)
        {
            if (value == null) return null;
            switch ((int)value)
            {
                case 1:
                    return new SolidColorBrush(Windows.UI.Colors.Yellow);
                case 2:
                    return new SolidColorBrush(Windows.UI.Colors.Red); 
                default:
                    return new SolidColorBrush(Windows.UI.Colors.Transparent);
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }
}
