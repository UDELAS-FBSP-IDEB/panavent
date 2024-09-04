
using System.Globalization;

string sepCa = ",";


try
{
    String[] files = Directory.EnumerateFiles($"C:\\Users\\manue\\OneDrive\\OneDrive - Sistemas Moviles SA\\Documentos\\Academicos\\UDELAS\\docs\\paper\\mediciones_2024\\PIDf\\2024-08-14\\data").ToArray();
    foreach (string file in files)
    {

        var lines = File.ReadAllLines(file);
        lines = lines.Where(line => !string.IsNullOrWhiteSpace(line) && !line.Contains ("ACK recibido,") && line !=",").ToArray();
        var fileName = Path.GetFileNameWithoutExtension(file);
        double meta = 0;
        if (fileName.Contains("mA"))
        {
            fileName = fileName.Replace("mA", "");
            meta = double.Parse(fileName);
            meta = meta / 1000;
        }
        else if (fileName.Contains("lpm"))
        {
            fileName = fileName.Replace("lpm", "");
            meta = double.Parse(fileName);
        }

        bool fin = false;


        while (lines.Length > 0 &&  !fin) {
            var offset = 0;
            double maximo = 0;
            int maximoIndex = 0;
            int j = 0; 
            int k = 0;

            foreach (string line in lines)
            {
                
                double valor = Double.Parse(line.Split(sepCa)[1]) - offset;

                if (valor > maximo)
                {
                    maximo = valor;
                    maximoIndex = j;
                    k = 0;
                }
                else
                {
                    if (maximo > (0.90 * meta))
                    {
                        k += 1;
                        if (k > 10)
                        {
                            break;
                        }
                    }
                }

                j++;
            }

            double maximo90 = maximo * 0.90;

            if (maximo90 > 0)
            {
                int lowerStopIndex = 0;
                j = 0;
                foreach (string line in lines.Take(maximoIndex + 1))
                {
                    double valor = Double.Parse(line.Split(sepCa)[1]) - offset;
                    if (valor < maximo90) { lowerStopIndex = j; }
                    j++;
                }

                int upperStopIndex = 0;
                j = 0;
                foreach (string line in lines.Skip(maximoIndex + 1))
                {
                    double valor = Double.Parse(line.Split(sepCa)[1]) - offset;
                    if (valor > maximo90) { upperStopIndex = maximoIndex + j; } else { break; }
                    j++;
                }

                List<double> meseta = new List<double>();
                double steadyStateValue = Double.Parse(lines.Skip(lowerStopIndex + 1).ToList()[0].Split(sepCa)[1]);
                j = 0;
                foreach (string line in lines.Skip(lowerStopIndex + 1).Take(upperStopIndex - lowerStopIndex + 1))
                {
                    double valor = Double.Parse(line.Split(sepCa)[1]) - offset;
                    meseta.Add(valor);
                    j++;
                }
                steadyStateValue = meseta.Average();

                TimeSpan startRiseTime = new TimeSpan();
                TimeSpan endRiseTime = new TimeSpan();
                TimeSpan startSettlingTime = new TimeSpan();
                TimeSpan endSettlingTime = new TimeSpan();
                double std95 = steadyStateValue * 0.95;
                double std105 = steadyStateValue * 1.05;
                double std90 = steadyStateValue * 0.90;
                double std10 = steadyStateValue * 0.10;
                double std02 = steadyStateValue * 0.02;
                int startRiseTimeIx = 0;
                int endRiseTimeIx = 0;
                j = 0;
                foreach (string line in lines.Take(upperStopIndex))
                {
                    double valor = Double.Parse(line.Split(sepCa)[1]) - offset;
                    if (valor <= std10)
                    {
                        if (!TimeSpan.TryParseExact(line.Split(sepCa)[0], @"hh\:mm\:ss\:ffffff", CultureInfo.InvariantCulture, out startSettlingTime))
                        {
                            startSettlingTime = TimeSpan.FromSeconds(Double.Parse(line.Split(sepCa)[0]));
                        }
                    }
                    if (valor <= std10)
                    {
                        if (!TimeSpan.TryParseExact(line.Split(sepCa)[0], @"hh\:mm\:ss\:ffffff", CultureInfo.InvariantCulture, out startRiseTime))
                        {
                            startRiseTime = TimeSpan.FromSeconds(Double.Parse(line.Split(sepCa)[0]));
                        }
                        startRiseTimeIx = j;
                    }
                    if (valor <= std90)
                    {
                        if (!TimeSpan.TryParseExact(line.Split(sepCa)[0], @"hh\:mm\:ss\:ffffff", CultureInfo.InvariantCulture, out endRiseTime))
                        {
                            endRiseTime = TimeSpan.FromSeconds(Double.Parse(line.Split(sepCa)[0]));
                        }
                        endRiseTimeIx = j;
                    }
                    if (valor >= std95 && valor <= std105)
                    {
                        if (!TimeSpan.TryParseExact(line.Split(sepCa)[0], @"hh\:mm\:ss\:ffffff", CultureInfo.InvariantCulture, out endSettlingTime))
                        {
                            endSettlingTime = TimeSpan.FromSeconds(Double.Parse(line.Split(sepCa)[0]));
                        }
                        break;
                    }
                    j++;
                }

                foreach (string line in lines.Skip(j).ToList())
                {
                    double valor = Double.Parse(line.Split(sepCa)[1]) - offset;
                    if (valor == offset)
                    {
                        break;
                    }
                    j++;
                }

                double overshoot = (maximo - meta) / meta * 100;

                Console.WriteLine("{0}, {1}, {2}, {3}, {4}, {5}",
                            meta,
                            overshoot,
                            100 * ((meta - steadyStateValue) / meta),
                            steadyStateValue,
                            (endRiseTime - startRiseTime).TotalMilliseconds,
                            (endSettlingTime - startSettlingTime).TotalMilliseconds);

                lines = lines.Skip(j).ToArray();
            }
            else {
                fin = true;
            }

        


        }



    }

}
catch (Exception e)
{
    Console.WriteLine(e.Message);
}


Console.ReadLine();

