# arduino-incubator
simple design for an icubator using common breakout boards
six parameters are set through the matrix keypad:
      "Temp Off","degC",\n
      "Temp On","degC",\n
      "Turn active","Y/N",
      "Humidity","%",
      "Frequency","hrs",
      "turn stop","day".
 
  A tone buzzer is included to sound an alarm should there be a deviation from the set parameters.

the following libraries were used in development.
	paulstoffregen/OneWire@^2.3.5
	milesburton/DallasTemperature@^3.9.1
	chris--a/Keypad@^3.1.1
	adafruit/RTClib@^1.12.5
	adafruit/DHT sensor library@^1.4.2
	paulstoffregen/TimerOne@^1.1
	sstaub/Ticker@^4.2.0
	marcoschwartz/LiquidCrystal_I2C@^1.1.4
	adafruit/Adafruit Unified Sensor@^1.1.4
