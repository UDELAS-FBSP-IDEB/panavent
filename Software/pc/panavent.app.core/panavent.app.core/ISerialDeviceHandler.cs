using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;

namespace panavent.app.core
{
    public interface ISerialDeviceHandler
    {
        bool Connected { get; }
        Task<bool> ConnectAsync();
        void Disconnect();
        void SendData(byte[] data);
        Task<byte[]> ReadDataAsync(uint numBytes);
    }
}
