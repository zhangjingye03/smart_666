# smart_666 ![travis-ci](https://travis-ci.org/zhangjingye03/smart_666.svg?branch=master)
An arduino program to control or show something in my dear class 6.

# 功能

* 音响控制：开、关、自动（根据执信的下课铃，响3s即切断，当然需要把RTC时钟与学校的广播系统对齐）

* 倒计时牌：高考/月考/联考/期中考/期末考/调研考/一模/二模/三模/水平测/...， 可以选择预设项目

* ~~饭卡读取：余额查询、上次消费~~ 没人用这个功能而且吊着个PCB在外面挺难看的，舍弃之。
  对执信饭卡感兴趣的同学欢迎查看我的另一个项目[zxcardumper](https://github.com/zhangjingye03/zxcardumper)

* 温度显示

* 生日祝福

* 奋斗提示

* 贪吃蛇小游戏（只用了70多行哦）

* 六六题霸（刷题工具）

# 图片预览

![开机动画](http://i2.muimg.com/4851/dd2ad88d388cc326.jpg)
![主页](http://i2.muimg.com/4851/1d6f4542add2be53.jpg)
![音响控制](http://i2.muimg.com/4851/7ce7b2075dcd8522.jpg)
![自定义考试](http://i2.muimg.com/4851/73e8eaf1ac72b31a.jpg)
![高考倒计时](http://i2.muimg.com/4851/36c598e8467e464b.jpg)
![奋斗提示](http://i2.muimg.com/4851/a0a9348a80c0979a.jpg)
![关于本机](http://i2.muimg.com/4851/3ff38a0ef9835838.jpg)
![高级设置](http://i2.muimg.com/4851/6b2dfc257fc8eabd.jpg)
![倒计时牌设置](http://i2.muimg.com/4851/847bedb3b79690f3.jpg)
![RTC微调](http://i2.muimg.com/4851/e21ad7ade553ae76.jpg)
![贪吃蛇](http://i2.muimg.com/4851/7ea87e9df1fe0d8e.jpg)
![题霸](http://i2.muimg.com/4851/e8a0214f82e51ea2.jpg)

# 内存占用

使用`platformIO 3.4.0a8`编译

```
Linking .pioenvs\uno\firmware.elf
Checking program size
text       data     bss     dec     hex filename
19408       428     409   20245    4f15 .pioenvs\uno\firmware.elf
Building .pioenvs\uno\firmware.hex
```

# 需求

* Arduino UNO / Genuino UNO / DCCduino x 1

* USart 22DTP x 1 (串口显示屏，淘宝有售)

* ~~MFRC-522 x 1 (NFC模块)~~

* DS3231 x 1 (RTC模块、温度传感器、24C32储存模块)

* CH34x/PL230x x 1 (usb-ttl converter，用于调试串口屏)

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

上游和音响可调换，但一定要接到继电器上的常闭接口（确保Arduino死机/断电时音响可以正常工作）

实际使用场景如下（~~丑，而且这是很老的横屏版本~~）

![setup_env](https://raw.githubusercontent.com/zhangjingye03/smart_666/master/setup_env.jpg)

# 生日祝福说明

详见`birthday.h`，当然别忘了制作触摸屏固件中相关人名的48号字库

# 题霸说明

## 制作

按照`example.xlsx`制作题目，另存一份为csv，再使用`convert.php`处理。

P.S. 使用制作工具`convert.php`前请先打开区域和语言设置，将默认分隔符改为键盘左上角的那个玩意儿（这个字符在MarkDown中是关键字，故不在此展示）

## 刷写

`convert.php`处理后将会产生`66tb.bin`（请确保其大小<=4KByte），然后用支持i2c接口的编程器刷入带24C32的DS3231模块

P.S. 理论上DS3231模块上带有的24C32可以自己用烙铁换成24C256等大容量flash，程序使用理论上没有问题

## 区块说明

详见备注文件`tb.txt`、源码`66tb.h`，示例文件`example.csv`以及转换程序`convert.php`

# License

MIT

# Thanks

* adafruit for [RTClib](https://github.com/adafruit/RTClib)

* cyberp for [AT24CX](https://github.com/cyberp/AT24Cx)
