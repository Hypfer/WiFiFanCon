## The software

The software is implemented in the berry language on top of the Tasmota operating system which does all the heavy
lifting, allows for easy Wi-Fi provisioning, general configuration and most importantly stability due to a battle-tested and mature codebase.

### Installation/Setup

After installing [Tasmota](https://github.com/arendst/Tasmota) and having it connected to your Wi-Fi network, a few additional setup steps have to be done:

#### Configure template and Identity

First, in the Tasmota Webinterface of your ESP32, navigate to **Configuration > Other**. There, paste the following template:
```
{"NAME":"WiFiFanCon","GPIO":[0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"FLAG":0,"BASE":1}
```

Make sure to check the **Activate** checkbox. Also, while you're there, consider setting up the _Device Name_ and _Friendly Name_.
Please note that the code of this project requires you to stick to `[A-Za-z0-9 _-]` for those.
Use of any other characters for those names will likely result in stuff not working correctly.

Click Save and wait for the ESP to reboot.

#### Configure parameters

After it has rebooted successfully, navigate back to the main page and then go to **Consoles > Console** and execute the following commands:

- `PWMFrequency 25000` to set the correct PWM frequency for 12V PC Fans
- `SetOption15 OFF` to prevent Tasmota from treating the PWM as a Light
- `WebSensor1 0` to hide the Counters (an implementation detail) from the Tasmota Webpage
- `PWMRange 1023` to set the PWM range to what is assumed by the code

It's possible that Tasmota will reboot to apply the changes made by some of those commands so make sure to wait for a few seconds between those.
You can always check if they were applied correctly by just running the command without the new desired value:
```
15:36:23.046 CMD: PWMFrequency
15:36:23.053 MQT: stat/tasmota_642EFC/RESULT = {"PWMFrequency":25000}
```

#### Configure GPIOs

Once those parameters are set up, head over to **Configuration > Configure Module** and set up the GPIOs according to your needs.
On the PCB, the four PC fan headers are wired as follows:

- FAN1: PWM GPIO17, Tacho GPIO16
- FAN2: PWM GPIO19, Tacho GPIO18
- FAN3: PWM GPIO22, Tacho GPIO21
- FAN4: PWM GPIO25, Tacho GPIO23

meaning that e.g. GPIO16 in Tasmota would be set up as Counter 1 and GPIO17 in Tasmota would be set up as PWM 1.

**Note**:
If you don't need all four fan headers, don't configure their GPIOs.
The amount of configured PWM GPIOs controls how many Fans will show up in the Web and via MQTT.
Make sure that your IDs configured in Tasmota start at 1 and don't skip any number.
Also, keep in mind that the FAN1 header doesn't strictly have to be PWM/Counter 1 in Tasmota.

#### Upload code

With the GPIOs all set up, navigate to **Consoles > Manage File System** and upload all the .be files from the `src` directory next to this readme.

If you don't need MQTT support then you're done now. Simply reboot the system and enjoy Fan control on the Main Web page.

#### MQTT Setup

If you **do** want MQTT support, there's one final thing you'll need to do beside configuring MQTT Host/Credentials/Parameters in the Tasmota settings.
Head over to the [hct](https://github.com/fmtr/hct) repo, follow the installation instructions, reboot and you're done.

If you connect the WiFiFanCon to Home Assistant via MQTT, don't forget to disable the autoconfigured Counter entities, as they serve no purpose in this setup and will only pollute your `recorder:` component with irrelevant data.

#### Final remarks

If you experience any issues, make sure to check the Log output in the **Consoles > Berry Scripting Console** for error messages etc.