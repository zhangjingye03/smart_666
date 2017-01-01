# smart_666 ![travis-ci](https://travis-ci.org/zhangjingye03/smart_666.svg?branch=master)
An arduino program to control or show something in my dear class 6.

# 功能

* 音响控制：开、关、自动

* 倒计时牌：高考/开学测/...~~，后续考虑实现编辑~~（不考虑，这个型号的触摸屏不支持输入法）

* ~~饭卡读取：余额查询、上次消费~~ 没人用这个功能而且吊着个PCB在外面挺难看的，舍弃之。

* 温度显示

* 生日祝福

* 节日庆祝（只做了一个）

* 贪吃蛇小游戏（只用了70行哦）

# 内存占用

使用`platformIO 3.3.0a1`编译

```
AVR Memory Usage
----------------
Device: atmega328p

Program:   15920 bytes (48.6% Full)
(.text + .data + .bootloader)

Data:        663 bytes (32.4% Full)
(.data + .bss + .noinit)
```

# 需求

* Arduino UNO / Genuino UNO / DCCduino x 1

* USart 22DTP x 1 (串口显示屏)

* ~~MFRC-522 x 1 (NFC模块)~~

* DS3231 x 1 (RTC模块、温度传感器)

* CH34x/PL230x x 1 (usb-ttl converter)

* 继电器 x 1 (音箱控制)

# 电路连接图

| Arduino UNO | 22DTP   | DS3231 | 继电器 | 音响连接线 |
|:-----------:|:-------:|:------:|:------:|:---------:|
| 0 (RX)      | TX      |        |        |           |
| 1 (TX)      | RX      |        |        |           |
| 2           | 5V      |        |        |           |
| 4           |         |        | CTRL   |           |
| A4 (SDA)    |         | SDA    |        |           |
| A5 (SCL)    |         | SCL    |        |           |
| 5V          |         | VCC    | VCC    |           |
| GND         | GND     | GND    | GND    |           |
|             |         |        | COM    | 接上游 (*) |
|             |         |        | NC     | 接音响一端 |

上游和音响可调换，但一定要接到继电器上的常闭接口

实际使用场景如下（~~丑~~）

![setup_env](https://raw.githubusercontent.com/zhangjingye03/smart_666/master/setup_env.jpg)

# License

MIT

# Thanks

* adafruit for [RTClib](https://github.com/adafruit/RTClib)

* ~~miguelbalboa and his communities for [rfid](https://github.com/miguelbalboa/rfid)~~
