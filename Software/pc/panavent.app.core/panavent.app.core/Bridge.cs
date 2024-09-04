using System.Collections.Concurrent;
using panavent.app.core.util;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using panavent.app.core.comandos;

namespace panavent.app.core
{
    public class Bridge : RunningComponent
    {
        DateTime? dti=null;
        DateTime? dtt=null;
        private const byte SOH = 0x01;
        private const byte STX = 0x02;
        private static Bridge si;
        private ISerialDeviceHandler dev;   
        private List<byte> input = new List<byte>();
        private ConcurrentQueue<ComandoI2C> output = new ConcurrentQueue<ComandoI2C>();
        public bool ventilando { get; set; }
        public event EventHandler<ErrorCommandEventArgs> ErrorCommandReceived;
        public event EventHandler<VentilacionDataCommandEventArgs> VentilacionDataCommandReceived;
        public event EventHandler<GetCalibracionCommandEventArgs> GetCalibracionCommandReceived;
        public event EventHandler<GetConstantesPIDCommandEventArgs> GetConstantesPIDCommandReceived;
        private Bridge(ISerialDeviceHandler device)
        {
            dev = device;

            //PInvoke.DeviceIoControl()
        }
        public static Bridge GetSerialInterface(ISerialDeviceHandler device)
        {
            if (si == null)
            {
                si = new Bridge(device);
            }
            return si;
        }
        public static Bridge GetSerialInterface()
        {      
            return si;
        }
        public override   void Start()
        {
            if (base.Running == false)
            { 
                if (dev.Connected)
                {
                    base.Start();
                } 
            }
        }
        public override void Stop()
        {
            if (base.Running == true)
            {
                if (dev.Connected)
                {
                    dev.Disconnect();
                } 
                base.Stop();
            }
        }  
       

        protected override async void RunNow()
        {
            try
            {
                while (CanContinue())
                {
                    List<byte> inputData = new List<byte>();
                    try
                    {

                        inputData.AddRange(await dev.ReadDataAsync(1));
                    }
                    catch (Exception e)
                    {
                        Debug.WriteLine("TASK EXCEPTION: {0}.\n", e.Message);
                    }

                    foreach (var b in inputData.ToArray())
                    {
                        if (input.Count == 0 && b == SOH)
                        {
                            input.Add(b);
                        }
                        else if (input.Count == 1 && b == STX)
                        {
                            input.Add(b);
                        }
                        else if (input.Count >= 2)
                        {
                            input.Add(b);
                            if (input.Count >= 8)
                            {
                                if (input[5] + 8 == input.Count)
                                {
                                    if (ComandoI2C.ValidarCheckSum(input.ToArray()))
                                    {
                                        switch ((AppCommandType)input[2])
                                        {
                                            case AppCommandType.APP_CMD_PROTOCOLO_ERROR_CODE:
                                                Debug.WriteLine("Error recibido");
                                                var error = new Error(input.ToArray());
                                                OnErrorCommandReceived(error.ErrorCode, error.Descripcion);
                                                EnqueuePacket(new Ack() { SequenceNumber = error.SequenceNumber });
                                                break;
                                            case AppCommandType.APP_CMD_PROTOCOLO_PING:
                                                var ping = new Ping(input.ToArray());
                                                Debug.WriteLine("Ping recibido");
                                                EnqueuePacket(new Ack() { SequenceNumber = ping.SequenceNumber });
                                                break;
                                            case AppCommandType.APP_CMD_VENTILACION_DATA:
                                                var data = new VentilacionData(input.ToArray());
                                                OnVentilacionDataCommandReceived(data);
                                                EnqueuePacket(new Ack() { SequenceNumber = data.SequenceNumber });
                                                break;
                                            case AppCommandType.APP_CMD_PROCESOS_GET_CALIBRACION:
                                                var getCalibracion = new GetCalibracion(input.ToArray()); 
                                                OnGetCalibracionCommandReceived(getCalibracion.Indice, getCalibracion.SequenceNumber );
                                                break;
                                            case AppCommandType.APP_CMD_PROCESOS_GET_CONSTANTES_PID:
                                                Debug.WriteLine("APP_CMD_PROCESOS_GET_CONSTANTES_PID");
                                                var getConstantesPID = new GetConstantesPID(input.ToArray());
                                                OnGetConstantesPIDCommandReceived(getConstantesPID.Indice, getConstantesPID.SequenceNumber ); 
                                                break;
                                            case AppCommandType.APP_CMD_PROTOCOLO_ACK:
                                                Debug.WriteLine("ACK recibido"); 
                                                break;
                                            case AppCommandType.APP_CMD_SINGLE_DATA:
                                                var singleData = new SingleData (input.ToArray());
                                                Debug.WriteLine(string.Format ("{0},{1:f}", DateTime.Now.ToString("hh:mm:ss:ffffff"),  singleData.Data));
                                                
                                                var ventData = new VentilacionData();
                                                ventData.Flujo = singleData.Data;
                                                OnVentilacionDataCommandReceived(ventData);

                                                break;
                                            default:
                                                Debug.WriteLine("Paquete desconocido");
                                                EnqueuePacket(new Nack());
                                                break;
                                        }
                                        input.Clear();
                                    }
                                    else
                                    {
                                        Debug.WriteLine("Paquete deformado");
                                        input.Clear();
                                    }
                                }
                                else {
                                    if (input[5] + 8 < input.Count) {
                                        Debug.WriteLine("Paquete deformado");
                                        input.Clear();
                                    }
                                }
                            }
                        }
                        else
                        {
                            input.Clear();
                        }
                    }

                    if (output.TryDequeue(out ComandoI2C pack))
                    {
                        byte[] data = pack.Serializar();
                        dev.SendData(data);
                        //Debug.Write("Paquete enviado: ");
                        //Debug.WriteLine(pack.AppCommandType);
                    }

                }
            }
            catch(Exception ex)
            {
                Debug.WriteLine(ex);
            }
          
        }
        private void OnVentilacionDataCommandReceived(VentilacionData data)
        {
            VentilacionDataCommandReceived?.Invoke(this, new VentilacionDataCommandEventArgs() {  Data = (VentilacionData)data });
        }
        private   void OnErrorCommandReceived(ErrorCode errorCode, string message)
        {
           
            ErrorCommandReceived?.Invoke(this, new ErrorCommandEventArgs() { ErrorCode = errorCode, Message = message });

        }
        private void OnGetCalibracionCommandReceived(byte indice, UInt16 sequenceNumber)
        {
            GetCalibracionCommandReceived?.Invoke(this, new GetCalibracionCommandEventArgs() { Indice = indice, SequenceNumber = sequenceNumber  }); 
        }
        private void OnGetConstantesPIDCommandReceived(byte indice, UInt16 sequenceNumber)
        {
            GetConstantesPIDCommandReceived?.Invoke(this, new GetConstantesPIDCommandEventArgs () { Indice = indice, SequenceNumber = sequenceNumber });
        }
        public void EnqueuePacket(ComandoI2C pack)
        {
            output.Enqueue(pack); 
        } 
    }

    public class GetCalibracionCommandEventArgs : EventArgs
    {
        public UInt16 SequenceNumber { get; set; }
        public byte Indice { get; set; }
    }

    public class GetConstantesPIDCommandEventArgs : EventArgs
    {
        public UInt16 SequenceNumber { get; set; }
        public byte Indice { get; set; }
    }

    public class VentilacionDataCommandEventArgs : EventArgs
    { 
        public VentilacionData Data { get; set; }
    }

    public class ErrorCommandEventArgs : EventArgs
    {
        public ErrorCode ErrorCode { get; set; }
        public string Message { get; set; }
    }

}