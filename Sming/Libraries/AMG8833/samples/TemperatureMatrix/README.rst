Temperature Matrix
==================

A simple demonstrating how to handle interrupts with the Melopero AMG8833 sensor.
The sensor communicates through I2C. Connection scheme::
 
	Microcontroller ----------- Melopero AMG8833
		       3.3V ----------- VIN
		       GND  ----------- GND
		       SCL  ----------- SCL
		       SDA  ----------- SDA
	 a valid INT Pin----------- INT
