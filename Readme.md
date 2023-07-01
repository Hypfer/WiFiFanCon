# WiFiFanCon

Control 12V PC Fans via Wi-Fi and [Tasmota](https://github.com/arendst/Tasmota)

[<img src="./img/board.jpg" width=650>](./img/board.jpg)
[<img src="./img/tasmota_home.png">](./img/board.jpg)

## Features

- Support of up to four 4-Pin PWM 12V PC Fans
- Individual RPM Monitoring for all 4 Fans
- Full control via a Webinterface
- Optional MQTT support with Home Assistant Autodiscovery utilizing [hct](https://github.com/fmtr/hct)
- Extension headers to allow for customization with e.g. additional temperature probes
- Very easy beginner-friendly THT soldering
- Fits snugly into an isolated 45ct junction box

## Where?

Head over to the [./pcb](./pcb) for the pcb.

For the firmware, install [Tasmota](https://github.com/arendst/Tasmota) e.g. using their fancy fully browser-based installer, then check out the [./berry](./berry) folder.

## Why?

I do have quite a few heat-producing devices such as servers or Hi-Fi equipment, which are trapped in whatever
furniture from Ikea barely fit them. This achieves a clean look but also kills the hardware due to insufficient ventilation.

My first solution for this was to add a few very silent 12V Noctua NF-A14 PWM PC Fans and run them at 5V with 33% duty cycle using an Arduino nano.
While that worked just fine, after a year, I've noticed quite a lot of dust buildup on the Fans as well as everything else.

Therefore, I was looking for a way to periodically run the Fans at full speed to blow away whatever dust buildup there might be.
How else would you do that if not via MQTT automated by Home Assistant?

## Pictures?

[<img src="./img/box1.jpg" width=650>](./img/box1.jpg)
[<img src="./img/box2.jpg" width=650>](./img/box2.jpg)
[<img src="./img/avr.jpg" width=650>](./img/avr.jpg)

## How?

As it turns out, the ESP32 offers everything required for such a project.
It has Wi-Fi, it can do 25kHz PWM with more than 4 channels and it is also capable of monitoring the Fan speed.
Furthermore, as it is available as a full devkit PCB, flashing, soldering and everything else is very easy.

Therefore, I basically just needed to design a PCB that features a DC Jack, some Fan Headers and connects everything in a neat package.
Because it is only 70x70mm in size, it perfectly fits into these dirt-cheap german junction boxes, which you can get for 45ct each.

## Why not use something else?

While there are other projects on GitHub achieving similar things, they didn't really 100% fit my requirements, which are:

- Set Fanspeed to x%
- Don't care about temperature
- Show up in home assistant
- Be very easy to solder

## License

This hardware is licensed under the
[Creative Commons Attribution-ShareAlike 4.0 International License][cc-by-sa].

[![CC BY-SA 4.0][cc-by-sa-image]][cc-by-sa]

The software is licensed under the Apache-2.0 license.

[cc-by-sa]: http://creativecommons.org/licenses/by-sa/4.0/
[cc-by-sa-image]: ./img/cc-by-sa.png
