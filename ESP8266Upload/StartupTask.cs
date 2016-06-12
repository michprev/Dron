using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Http;
using Windows.ApplicationModel.Background;
using Windows.Storage.Streams;
using Windows.Devices.SerialCommunication;
using Windows.Devices.Enumeration;
using Windows.Devices.Gpio;
using System.Diagnostics;
using Windows.Foundation;
using System.Threading.Tasks;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

namespace ESP8266Upload
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

                GpioController gpioController = await GpioController.GetDefaultAsync();

                GpioPin rst = gpioController.OpenPin(2);
                GpioPin ch_pd = gpioController.OpenPin(3);

                rst.Write(GpioPinValue.Low);
                ch_pd.Write(GpioPinValue.Low);

                /*await Task.Delay(500);

                rst.Write(GpioPinValue.High);
                ch_pd.Write(GpioPinValue.High);*/

                string s = SerialDevice.GetDeviceSelector();

                var dev = (await DeviceInformation.FindAllAsync(s)).First();

                this.serial = await SerialDevice.FromIdAsync(dev.Id);
                this.serial.BaudRate = 115200;
                this.serial.DataBits = 8;
                this.serial.Parity = SerialParity.None;
                this.serial.StopBits = SerialStopBitCount.One;

                this.serialWriter = new DataWriter(this.serial.OutputStream);
                this.serialReader = new DataReader(this.serial.InputStream);
                this.serialReader.InputStreamOptions = InputStreamOptions.Partial;

                //this.serialWriter.WriteString("AT\r\n");
                //await this.serialWriter.StoreAsync();

                await StartFlash(0, 0);

                while (true)
                {
                    await serialReader.LoadAsync(1);
                    Debug.WriteLine((int)serialReader.ReadByte());
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
        }

        public IAsyncAction SendCommand(byte command, IEnumerable<byte> pBody, int checksum)
        {
            List<byte> body = pBody.ToList();

            // replace 0xC0 and 0xDB with 0xDB 0xDC, 0xDB 0xDD resp.
            for (int i = 0; i < body.Count; i++)
            {
                if (body[i] == 0xC0)
                {
                    body.RemoveAt(i);
                    body.Insert(i, 0xDB);
                    body.Insert(i + 1, 0xDC);
                }
                else if (body[i] == 0xDB)
                {
                    body.RemoveAt(i);
                    body.Insert(i, 0xDB);
                    body.Insert(i + 1, 0xDD);
                }
            }

            List<byte> data = new List<byte>();
            data.Add(0xC0);
            data.Add(0x00);
            data.Add(command);
            data.AddRange(BitConverter.GetBytes(body.Count)); // body size
            data.AddRange(BitConverter.GetBytes(checksum)); // checksum
            data.AddRange(body);
            data.Add(0xC0);

            this.serialWriter.WriteBytes(data.ToArray());
            return this.serialWriter.StoreAsync().AsTask().AsAsyncAction();
            /*return this.serialWriter.StoreAsync().AsTask().ContinueWith(async (s) =>
            {
                await serialReader.LoadAsync(1);
                if (serialReader.ReadByte() != 0x79) throw new Exception("ACK expected");
            }).Unwrap().AsAsyncAction();*/
        }

        public IAsyncAction StartFlash(int size, int offset)
        {
            int eraseSize = 0;
            int blocks = 0;
            int blockSize = 0x400;

            List<byte> body = new List<byte>();
            body.AddRange(BitConverter.GetBytes(eraseSize));
            body.AddRange(BitConverter.GetBytes(blocks));
            body.AddRange(BitConverter.GetBytes(blockSize));
            body.AddRange(BitConverter.GetBytes(offset));

            return SendCommand(0x02, body, 0).AsTask().AsAsyncAction();
        }
    }
}
