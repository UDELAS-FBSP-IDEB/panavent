using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace panavent.app.core.comandos
{
    public class VentilacionParams : ComandoI2C
    {
        private Single volumen;
        private Single fiO2;
        private Single fr;
        private Single presion;
        private Single peep;
        private Single pausa;
        private IE ie;
        private Modo modo;
        private FormaOnda formaOnda;

        public VentilacionParams()
        {
            _appCommandType = AppCommandType.APP_CMD_VENTILACION_PARAMETROS ;
        }

        public VentilacionParams(byte[] bytes)
        {
            throw new NotImplementedException();
        }

        public Single Volumen
        {
            get { return volumen; }
            set { volumen = value; }
        }

        public Single FiO2
        {
            get { return fiO2; }
            set { fiO2 = value; }
        }

        public Single FR
        {
            get { return fr; }
            set { fr = value; }
        }

        public Single Presion
        {
            get { return presion; }
            set { presion = value; }
        }

        public Single PEEP
        {
            get { return peep; }
            set { peep = value; }
        }

        public Single Pausa
        {
            get { return pausa; }
            set { pausa = value; }
        }

        public IE IE
        {
            get { return ie; }
            set { ie = value; }
        }

        public Modo Modo
        {
            get { return modo; }
            set { modo = value; }
        }

        public FormaOnda FormaOnda
        {
            get { return formaOnda; }
            set { formaOnda = value; }
        }

        public override byte[] Serializar()
        {
            List<byte> buff = new List<byte>();
            buff.AddRange(BitConverter.GetBytes(volumen).Reverse());
            buff.AddRange(BitConverter.GetBytes(fiO2).Reverse());
            buff.AddRange(BitConverter.GetBytes(fr).Reverse());
            buff.AddRange(BitConverter.GetBytes(presion).Reverse());
            buff.AddRange(BitConverter.GetBytes(peep).Reverse());
            buff.AddRange(BitConverter.GetBytes(pausa).Reverse());
            buff.Add((byte)ie);
            buff.Add((byte)modo);
            buff.Add((byte)formaOnda);
            return GetBytes(buff.ToArray());
        }
    }

}
