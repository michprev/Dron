using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Http;
using Windows.ApplicationModel.Background;
using Windows.Devices.Gpio;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

namespace Ultrasonic
{
    public sealed class StartupTask : IBackgroundTask
    {
        private BackgroundTaskDeferral def;

        public async void Run(IBackgroundTaskInstance taskInstance)
        {
            this.def = taskInstance.GetDeferral();

            GpioController controller = await GpioController.GetDefaultAsync();

            GpioPin trigg = controller.OpenPin(23);
            GpioPin echo = controller.OpenPin(24);


        }
    }
}
