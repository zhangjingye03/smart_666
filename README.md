# smart_666
An arduino program to control or show something in my dear class 6.

# 功能

* 音响控制：开、关、自动

* 倒计时牌：高考/调研考/...~~，后续考虑实现编辑~~（不考虑，这个型号的触摸屏不支持输入法）

* 饭卡读取：余额查询、上次消费

* 温度显示

# 内存占用

使用`Arduino IDE 1.6.13`编译

```
Sketch uses 17,342 bytes (53%) of program storage space. Maximum is
32,256 bytes.
Global variables use 565 bytes (27%) of dynamic memory, leaving 1,483
bytes for local variables. Maximum is 2,048 bytes.
```

# 需求

* Arduino UNO / Genuino UNO / DCCduino x 1

* USart 22DTP x 1 (串口显示屏)

* MFRC-522 x 1 (NFC模块)

* DS3231 x 1 (RTC模块、温度传感器)

* CH34x/PL230x x 1 (usb-ttl converter)

* 继电器 x 1 (音箱控制)

# 电路连接图

参照`rfid`和`RTClib`库的注释

# License

MIT

# Thanks

* adafruit for [RTClib](https://github.com/adafruit/RTClib)

* miguelbalboa and his communities for [rfid](https://github.com/miguelbalboa/rfid)

# 备注

为实现温度检测，RTClib库已作改动但未上传。详见`RTClib`的[#62](https://github.com/adafruit/RTClib/pull/62)
