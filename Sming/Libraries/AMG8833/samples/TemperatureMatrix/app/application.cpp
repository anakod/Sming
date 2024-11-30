#include <SmingCore.h>
#include <Melopero_AMG8833.h>

Melopero_AMG8833 sensor;

namespace
{
const byte interruptPin = 2;

Timer procTimer;
bool state = true;

bool interruptOccurred = false;

//The high and low temperature thresholds. If a temperature that exceeds
//these values is detected an interrupt will be triggered. The temperatures
//are expressed in Celsius degrees.
float highThreshold = 26.5;
float lowThreshold = 10;

void onInterrupt()
{
	interruptOccurred = true;
}

void loop()
{
	//Update the sensors thermistor temperature and print it.
	Serial.print("Updating thermistor temperature ... ");
	int statusCode = sensor.updateThermistorTemperature();
	Serial.println(sensor.getErrorDescription(statusCode));

	Serial.print("Thermistor temp: ");
	Serial.print(sensor.thermistorTemperature);
	Serial.println("°C");

	//Check if an interrupt occurred.
	//The interrupt occurred flag gets set by the interrupt service routine
	//each time an interrupt is triggered.
	if(interruptOccurred) {
		interruptOccurred = false;
		Serial.println("Interrupt triggered!");

		//Update the interrupt matrix to know which pixels triggered the interrupt.
		Serial.print("Updating interrupt matrix ... ");
		statusCode = sensor.updateInterruptMatrix();
		Serial.println(sensor.getErrorDescription(statusCode));

		//Print out the interrupt matrix.
		Serial.println("Interrupt Matrix: ");
		for(int x = 0; x < 8; x++) {
			for(int y = 0; y < 8; y++) {
				Serial.print(sensor.interruptMatrix[y][x]);
				Serial.print(" ");
			}
			Serial.println();
		}
	}
}

} // namespace

void init()
{
	Wire.begin();
	sensor.initI2C();
	// To use Wire1 (I2C-1):
	// Wire1.begin();
	// sensor.initI2C(AMG8833_I2C_ADDRESS_B, Wire1);

	//Reset the device flags and settings and read the returned status code.
	Serial.print("Resetting sensor ... ");
	int statusCode = sensor.resetFlagsAndSettings();

	//Check if there were any problems.
	Serial.println(sensor.getErrorDescription(statusCode));

	//Setting the FPS_MODE this can be: FPS_MODE::FPS_10 or FPS_MODE::FPS_1
	Serial.print("Setting FPS ... ");
	statusCode = sensor.setFPSMode(FPS_MODE::FPS_10);
	Serial.println(sensor.getErrorDescription(statusCode));

	//Setting the threshold values
	Serial.print("Setting threshold values ... ");
	statusCode = sensor.setInterruptThreshold(lowThreshold, highThreshold);
	Serial.println(sensor.getErrorDescription(statusCode));
	Serial.print("low threshold : ");
	Serial.print(lowThreshold);
	Serial.print(" high threshold : ");
	Serial.println(highThreshold);

	//Enable the interrupt.
	Serial.print("Enabling interrupt ... ");
	statusCode = sensor.enableInterrupt();
	Serial.println(sensor.getErrorDescription(statusCode));

	//Configuring the interrupt pin to listen for the interrupt.
	pinMode(interruptPin, INPUT_PULLUP);
	//Attaching our function onInterrupt to the interrupt.
	attachInterrupt(interruptPin, onInterrupt, CHANGE);

	procTimer.initializeMs(100, loop).start();
}
