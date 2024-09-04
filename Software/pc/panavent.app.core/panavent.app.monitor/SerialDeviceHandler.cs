using panavent.app.core;
using panavent.app.core.util;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Devices.Enumeration;
using Windows.Devices.SerialCommunication;
using Windows.Foundation;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;

namespace panavent.app.monitor
{
    public  class SerialDeviceHandler :  ISerialDeviceHandler 
    {
        //public static SerialDeviceHandler serialDeviceHandler;
        //private static Object singletonCreationLock = new Object();
        private DeviceInformation deviceInformation;       
        private SerialDevice device;
        private String deviceSelector;
        private DataReader dr;
        private DataWriter dw;
        private SuspendingEventHandler appSuspendEventHandler;
        private EventHandler<Object> appResumeEventHandler;
        private Boolean watcherSuspended;
        private Boolean watcherStarted;
        private DeviceWatcher deviceWatcher;
        private TypedEventHandler<DeviceWatcher, DeviceInformation> deviceAddedEventHandler;
        private TypedEventHandler<DeviceWatcher, DeviceInformationUpdate> deviceRemovedEventHandler;
        private CancellationTokenSource ReadCancellationTokenSource;
        private Object ReadCancelLock = new Object();

        public SerialDeviceHandler(DeviceInformation deviceInfo, String deviceSelector)
        {
            deviceInformation = deviceInfo;
            this.deviceSelector = deviceSelector;
        }
        public async Task<bool> ConnectAsync()
        {
            device = await SerialDevice.FromIdAsync(deviceInformation.Id);
            if (device != null)
            {

                ReadCancellationTokenSource = new CancellationTokenSource(25);

                device.BaudRate = 115200;
                device.DataBits = 8;
                device.Parity = SerialParity.None;
                device.StopBits = SerialStopBitCount.One;
               
               
                
                dr = new DataReader(device.InputStream);
                dw = new DataWriter(device.OutputStream);

                dr.InputStreamOptions = InputStreamOptions.Partial;

                if (appSuspendEventHandler == null || appResumeEventHandler == null)
                {
                    RegisterForAppEvents();
                }

                if (deviceWatcher == null)
                {
                    deviceWatcher = DeviceInformation.CreateWatcher(deviceSelector); 
                    RegisterForDeviceWatcherEvents();
                }

                if (!watcherStarted)
                {
                    StartDeviceWatcher();
                }
                return true;
            }
            else
            {
                return false;
            } 
        }
        public  bool Connected
        {
            get {
                return (device != null);
            }
        }
        public void Disconnect()
        {
            if ( Connected)
            {
                CloseCurrentlyConnectedDevice();
            }

            if (deviceWatcher != null)
            {
                if (watcherStarted)
                {
                    StopDeviceWatcher();

                    UnregisterFromDeviceWatcherEvents();
                }

                deviceWatcher = null;
            }
             

            if (appSuspendEventHandler != null || appResumeEventHandler != null)
            {
                UnregisterFromAppEvents();
            }

            deviceInformation = null;
            deviceSelector = null;
             
        }
        private void UnregisterFromDeviceWatcherEvents()
        {
            deviceWatcher.Added -= deviceAddedEventHandler;
            deviceAddedEventHandler = null;

            deviceWatcher.Removed -= deviceRemovedEventHandler;
            deviceRemovedEventHandler = null;
        }
        private void UnregisterFromAppEvents()
        { 
            App.Current.Suspending -= appSuspendEventHandler;
            appSuspendEventHandler = null; 
            App.Current.Resuming -= appResumeEventHandler;
            appResumeEventHandler = null;
        }
        public async void SendData(byte[] data)
        {
            foreach(var b in data){
                dw.WriteByte(b);
            }
            await dw.StoreAsync();
        }
        public async Task<byte[]> ReadDataAsync(uint numBytes)
        {
            List<byte> data = new List<byte>();
            Task<UInt32> loadAsyncTask;
            uint ReadBufferLength = numBytes;
            UInt32 bytesRead = 0;


            //lock (ReadCancelLock)
            //{

            //}
            if (ReadCancellationTokenSource.IsCancellationRequested)
            {
                ReadCancellationTokenSource.Dispose();
                ReadCancellationTokenSource = new CancellationTokenSource();
            }

            dr.InputStreamOptions = InputStreamOptions.Partial;
            loadAsyncTask = dr.LoadAsync(ReadBufferLength).AsTask(ReadCancellationTokenSource.Token);

            try
            {
                ReadCancellationTokenSource.CancelAfter(500);
                bytesRead = await loadAsyncTask;
            }
            catch (OperationCanceledException)
            {
                //Debug.WriteLine("\nTasks cancelled: timed out.\n");
            }
            



            if (bytesRead > 0)
            {
                byte[] buffer = new byte[bytesRead];
                dr.ReadBytes(buffer);
                data.AddRange(buffer);
            }

            //List<byte> data = new List<byte>();
            //uint bufferLenght = 32;
            //UInt32 bytesRead =  await dr.LoadAsync(bufferLenght);
            //byte[] buffer = new byte[bytesRead];
            //dr.ReadBytes(buffer);
            //data.AddRange(buffer);


            //while (dr.UnconsumedBufferLength > 0)
            //{
            //    data.Add(dr.ReadByte());
            //}
            return data.ToArray();
        }
        private void RegisterForAppEvents()
        {
            appSuspendEventHandler = new SuspendingEventHandler(OnAppSuspension);
            appResumeEventHandler = new EventHandler<Object>(OnAppResume);
            App.Current.Suspending += appSuspendEventHandler;
            App.Current.Resuming += appResumeEventHandler;
        }
        private void OnAppSuspension(Object sender, SuspendingEventArgs args)
        {
            if (watcherStarted)
            {
                watcherSuspended = true;
                StopDeviceWatcher();
            }
            else
            {
                watcherSuspended = false;
            }

            CloseCurrentlyConnectedDevice();
        }
        private void OnAppResume(Object sender, Object args)
        {
            if (watcherSuspended)
            {
                watcherSuspended = false;
                StartDeviceWatcher();
            }
        }
        private  void CloseCurrentlyConnectedDevice()
        {
            if (device != null)
            { 
                device.Dispose(); 
                device = null;                 
            }
        }
        private void StopDeviceWatcher()
        {
            if ((deviceWatcher.Status == DeviceWatcherStatus.Started)
                || (deviceWatcher.Status == DeviceWatcherStatus.EnumerationCompleted))
            {
                deviceWatcher.Stop();
            }

            watcherStarted = false;
        }
        private void StartDeviceWatcher()
        {
            watcherStarted = true;

            if ((deviceWatcher.Status != DeviceWatcherStatus.Started)
                && (deviceWatcher.Status != DeviceWatcherStatus.EnumerationCompleted))
            {
                deviceWatcher.Start();
            }
        }
        private void RegisterForDeviceWatcherEvents()
        {
            deviceAddedEventHandler = new TypedEventHandler<DeviceWatcher, DeviceInformation>(this.OnDeviceAdded);

            deviceRemovedEventHandler = new TypedEventHandler<DeviceWatcher, DeviceInformationUpdate>(this.OnDeviceRemoved);

            deviceWatcher.Added += deviceAddedEventHandler;

            deviceWatcher.Removed += deviceRemovedEventHandler;
        }        
        private void OnDeviceRemoved(DeviceWatcher sender, DeviceInformationUpdate deviceInformationUpdate)
        {
            if (Connected && (deviceInformationUpdate.Id == deviceInformation.Id))
            {
                CloseCurrentlyConnectedDevice();
            }
        }
        private async void OnDeviceAdded(DeviceWatcher sender, DeviceInformation deviceInfo)
        {
            if ((deviceInformation != null) && (deviceInfo.Id == deviceInformation.Id) && !Connected)
            {
                await ConnectAsync();
            }
        }      
    }
}
