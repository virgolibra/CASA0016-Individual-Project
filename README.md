# CASA0016-Individual-Project
This is repo for casa0016 individual project





#### BMP 180 Module

> Temperature, Pressure and Altitude

https://lastminuteengineers.com/bmp180-arduino-tutorial/

https://create.arduino.cc/projecthub/electropeak/determining-the-pressure-and-altitude-using-gy-68-bmp180-and-93fd6f

<img src="img/bmp180Sensor.jpg" width=30% height=30%>

+ Operating Voltage 3.3 V. With a built-in 3.3 regulator, the module can be connected to 5 V power supply.
+ I2C Communication (SCL - SCL, SDA - SDA)
+ Output: Temperature, pressure and altitude.
+ Test Sketch: ```bmpTest.ino``` to read sensor data and print in the serial monitor.

Serial Monitor Output

<img src="img/bmpTestSerialMonitor.jpg" width=50% height=50%>



**Improvements:** The calculation of altitude is based on the measured pressure. The pressure on the sea level can affect the accuracy of estimated altitude. (1 hectopascal (hPa) - 8.5 m). The pressure at sea level is set to 1013.25 hPa as default, which could be updated in real time to get more accurate results.



#### Buzzer

> Low level trigger buzzer

https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/

<img src="img/lowLevelBuzzer.jpg" width=30% height=30%>

+ Operating Voltage: 5 V

+ GPIO (I/O - Pin 5)

+ Test Sketch: ```buzzerTest.ino``` to generate 1 kHz sound every 2 seconds.

   

#### LCD 1602

> Display prompt text and measured values.

<img src="img/lcd1602.png" width=30% height=30%>

+ Operating Voltage: 5 V
+ Communication (RS - 2, EN - 4, D4 - 8, D5 - 9, D6 - 10, D7 - 11)
+ Test Sketch: ```lcdTest.ino``` to test LCD displaying function and scroll the text.



#### TM1637 4-Digit Display

> Used as a desk clock to display the time

<img src="img/segmentDisplay.jpg" width=30% height=30%>

+ Operating Voltage: 5 V
+ Communication (CLK - 12, DIO - 13)
+ Test Sketch: ```tm1637Test.ino``` to test time display

<img src="img/tmDisplay.jpg" width=30% height=30%>



#### MQ-135 Module

> Measure gas ppm

[GitHub - Phoenix1747/MQ135: 📚🌀 Arduino library for the MQ135 air quality sensor. Allows for temperature and humidity corrected readings.](https://github.com/Phoenix1747/MQ135)

https://www.kane.co.uk/knowledge-centre/what-are-safe-levels-of-co-and-co2-in-rooms

