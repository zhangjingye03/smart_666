# smart_666
An arduino program to control or show something in my dear class 6.

# 功能

* 音响控制：开、关、自动

* 倒计时牌：高考/调研考/...，后续考虑实现编辑

* 饭卡读取：余额查询、上次消费

* 温度显示

# 需求

* Arduino UNO / Genuino UNO / DCCduino x 1

* USart 22DTP x 1 (串口显示屏)

* MFRC-522 x 1 (NFC模块)

* DS3231 x 1 (RTC模块、温度传感器)

* CH34x/PL230x x 1 (usb-ttl converter)

# 电路连接图

参照`rfid`和`RTClib`库的注释

# License

MIT

# Thanks

* adafruit for [RTClib](https://github.com/adafruit/RTClib)

* miguelbalboa and his communities for [rfid](https://github.com/miguelbalboa/rfid)

# 备注

为实现温度检测，RTClib库已作改动但未上传。详见`RTClib`的[#62](https://github.com/adafruit/RTClib/pull/62)
