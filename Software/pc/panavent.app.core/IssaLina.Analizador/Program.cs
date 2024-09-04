using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IssaLina.Analizador
{
    internal class Program
    {
        static void Main(string[] args)
        {
            char sepCa = ';';


            try
            {
                String[] files = Directory.EnumerateFiles($"{System.IO.Directory.GetCurrentDirectory()}\\data").ToArray();
                foreach (string file in files)
                {

                    var lines = File.ReadAllLines(file);

                    var offset = 0; //lines.Take(1000).Select(a => Double.Parse(a.Split(sepCa)[1])).Average();
                                    //offset = 0;
                    double maximo = 0;
                    int maximoIndex = 0;
                    int j = 0;

                    Path.GetFileNameWithoutExtension(file);

                    double meta = Double.Parse(Path.GetFileNameWithoutExtension(file));
                    if (file.Contains("mA"))
                    {
                        meta = meta / 1000;
                    }
                    int k = 0;

                    foreach (string line in lines)
                    {
                        //double valor = Double.Parse(line.Split(sepCa)[1]) - offset;
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
                    //double std02 = steadyStateValue * 0.02;
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

                    //Console.WriteLine("startRiseTime: " + startRiseTimeIx);
                    //Console.WriteLine("endRiseTime: " + endRiseTimeIx);

                    //steadyStateValue = steadyStateValue / 10;

                    Console.WriteLine("{0}, {1}, {2}, {3}, {4}",
                        meta,
                       100 * ((meta - steadyStateValue) / meta),
                        steadyStateValue,
                        (endRiseTime - startRiseTime).TotalMilliseconds,
                        (endSettlingTime - startSettlingTime).TotalMilliseconds);

                }

            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }


            Console.ReadLine();
        }
    }
}
