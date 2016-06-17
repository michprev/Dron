using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Http;
using Windows.ApplicationModel.Background;
using Windows.Foundation;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Diagnostics;
using Windows.Devices.SerialCommunication;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.Devices.Enumeration;
using System.Threading.Tasks;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

namespace STM32Upload
{
    public sealed class StartupTask : IBackgroundTask
    {
        private BackgroundTaskDeferral def;

        private SerialDevice serial;
        private DataReader serialReader;
        private DataWriter serialWriter;

        public async void Run(IBackgroundTaskInstance taskInstance)
        {
            try
            {
                this.def = taskInstance.GetDeferral();

                string s = SerialDevice.GetDeviceSelector();

                var dev = (await DeviceInformation.FindAllAsync(s)).First();

                this.serial = await SerialDevice.FromIdAsync(dev.Id);
                this.serial.BaudRate = 57600;
                this.serial.DataBits = 8;
                this.serial.Parity = SerialParity.Even;
                this.serial.StopBits = SerialStopBitCount.One;

                this.serialWriter = new DataWriter(this.serial.OutputStream);
                this.serialReader = new DataReader(this.serial.InputStream);
                this.serialReader.InputStreamOptions = InputStreamOptions.Partial;


                // send init command

                await SendInit();

                // get available commands

                await GetCommands();

                // get device ID

                await GetPID();

                // mass erase

                await MassErase();

                StorageFile file = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx:///Gyro.bin"));
                var buffer = await FileIO.ReadBufferAsync(file);

                Queue<byte> data = new Queue<byte>();

                using (DataReader dataReader = DataReader.FromBuffer(buffer))
                {
                    byte[] tmp = new byte[buffer.Length];
                    dataReader.ReadBytes(tmp);

                    foreach (byte b in tmp)
                    {
                        data.Enqueue(b);
                    }
                }

                int c = data.Count;
                int address = 0x08000000;

                while (data.Count > 256)
                {
                    await WriteMemory(address, data.Take(256).ToArray());

                    await Task.Delay(100);

                    byte[] check = (await ReadMemory(address)).ToArray();


                    address += 256;

                    for (int i = 0; i < 256; i++)
                    {
                        if (check[i] != data.Dequeue())
                        {
                            Debug.WriteLine("problem {0}", check[i]);
                        }
                    }


                    Debug.WriteLine("Write: {0:0}%", 100 - (double)data.Count / c * 100);
                }

                while (data.Count != 256)
                {
                    data.Enqueue(0xFF);
                }
                await WriteMemory(address, data.Take(256).ToArray());

                Debug.WriteLine("Write: 100%", 100 - (double)data.Count / c * 100);

                await Go(0x08000000);

                this.serial.BaudRate = 9600;
                this.serial.Parity = SerialParity.None;

                Debug.WriteLine("Starting debugging log");
                Debug.WriteLine("************************");

                Task t = new Task(async () =>
                {
                    while (true)
                    {
                        await serialReader.LoadAsync(1);
                        Debug.Write((int)serialReader.ReadByte());
                    }
                });

                t.Start();

                //this.serialWriter.WriteString("AT\r\n");
                //await this.serialWriter.StoreAsync();

                /*await Task.Delay(1000);

                for (int i = 0; i < 12; i++)
                {
                    this.serialWriter.WriteString("AT+GMR\r\n");
                    await this.serialWriter.StoreAsync();

                    await Task.Delay(1000);
                }*/

                while (true) ;
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex);
            }
        }

        public IAsyncAction SendInit()
        {
            this.serialWriter.WriteByte(0x7F);
            return this.serialWriter.StoreAsync().AsTask().ContinueWith(async (s) =>
            {
                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");
            }).Unwrap().AsAsyncAction();
        }

        public IAsyncAction GetCommands()
        {
            serialWriter.WriteBytes(new byte[] { 0x00, 0x00 ^ 0xFF });
            return serialWriter.StoreAsync().AsTask().ContinueWith(async (s) =>
            {
                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");

                await serialReader.LoadAsync(1);
                int length = serialReader.ReadByte() + 1;

                byte[] data = new byte[length];

                await serialReader.LoadAsync((uint)length);
                serialReader.ReadBytes(data);

                Debug.WriteLine("Bootloader version: 0x{0:X2}", data[0]);
                Debug.WriteLine("Commands:");

                for (int i = 1; i < length; i++)
                {
                    Debug.WriteLine("\t0x{0:X2}", data[i]);
                }

                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");
            }).Unwrap().AsAsyncAction();
        }

        public IAsyncAction GetPID()
        {
            serialWriter.WriteBytes(new byte[] { 0x02, 0x02 ^ 0xFF });
            return serialWriter.StoreAsync().AsTask().ContinueWith(async (s) =>
            {
                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");

                await serialReader.LoadAsync(1);
                int length = serialReader.ReadByte() + 1;

                byte[] buffer = new byte[length];

                await serialReader.LoadAsync((uint)length);
                serialReader.ReadBytes(buffer);

                buffer = buffer.Reverse().ToArray();

                short device = BitConverter.ToInt16(buffer, 0);

                Debug.WriteLine("PID: 0x{0:X2}", device);
                Debug.WriteLine("RAM: 19 kB");
                Debug.WriteLine("Flash: 128 kB");

                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");
            }).Unwrap().AsAsyncAction();
        }

        public IAsyncOperation<IEnumerable<byte>> ReadMemory(int startAddress)
        {
            serialWriter.WriteBytes(new byte[] { 0x11, 0x11 ^ 0xFF });
            return serialWriter.StoreAsync().AsTask().ContinueWith(async (s) =>
            {
                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");


                byte[] addr = BitConverter.GetBytes(startAddress).Reverse().ToArray();

                serialWriter.WriteBytes(addr);
                serialWriter.WriteByte((byte)(addr[0] ^ addr[1] ^ addr[2] ^ addr[3]));
                await serialWriter.StoreAsync();

                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");

                serialWriter.WriteByte(0xFF);
                serialWriter.WriteByte(0xFF ^ 0xFF);
                await serialWriter.StoreAsync();

                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");

                byte[] buffer = new byte[256];
                await serialReader.LoadAsync(256);
                serialReader.ReadBytes(buffer);

                return buffer.AsEnumerable();
            }).Unwrap().AsAsyncOperation();
        }

        public IAsyncAction WriteMemory(int address, [ReadOnlyArray] byte[] data)
        {
            serialWriter.WriteBytes(new byte[] { 0x31, 0x31 ^ 0xFF });
            return serialWriter.StoreAsync().AsTask().ContinueWith(async (s) =>
            {
                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");

                byte[] addr = BitConverter.GetBytes(address).Reverse().ToArray();

                serialWriter.WriteBytes(addr);
                serialWriter.WriteByte((byte)(addr[0] ^ addr[1] ^ addr[2] ^ addr[3]));
                await serialWriter.StoreAsync();

                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");

                serialWriter.WriteByte(0xFF);
                serialWriter.WriteBytes(data);

                byte checksum = 0xFF;

                for (int i = 0; i < data.Length; i++)
                {
                    checksum ^= data[i];
                }

                serialWriter.WriteByte(checksum);
                await serialWriter.StoreAsync();

                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");
            }).Unwrap().AsAsyncAction();
        }

        public IAsyncAction Go(int address)
        {
            serialWriter.WriteBytes(new byte[] { 0x21, 0x21 ^ 0xFF });
            return serialWriter.StoreAsync().AsTask().ContinueWith(async (s) =>
            {
                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");

                byte[] addr = BitConverter.GetBytes(address).Reverse().ToArray();

                serialWriter.WriteBytes(addr);
                serialWriter.WriteByte((byte)(addr[0] ^ addr[1] ^ addr[2] ^ addr[3]));
                await serialWriter.StoreAsync();

                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");
            }).Unwrap().AsAsyncAction();
        }

        public IAsyncAction MassErase()
        {
            serialWriter.WriteBytes(new byte[] { 0x43, 0x43 ^ 0xFF });
            return serialWriter.StoreAsync().AsTask().ContinueWith(async (s) =>
            {
                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");

                serialWriter.WriteByte(0xFF);
                serialWriter.WriteByte(0xFF ^ 0xFF);
                await serialWriter.StoreAsync();

                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");
            }).Unwrap().AsAsyncAction();
        }
    }
}
