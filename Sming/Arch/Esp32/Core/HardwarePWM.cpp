/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * HardwarePWM.cpp
 *
 * Original Author: https://github.com/hrsavla
 * Esp32 version:   https://github.com/pljakobs
 *
 * This HardwarePWM library enables Sming framework uses to use the ESP32 ledc PWM api
 * 
 * the ESP32 PWM Hardware is much more powerful than the ESP8266, allowing wider PWM timers (up to 20 bit)
 * as well as much higher PWM frequencies (up to 40MHz for a 1 Bit wide PWM)
 * 
 * Overview:
 * +------------------------------------------------------------------------------------------------+
 * | LED_PWM                                                                                        |
 * |  +-------------------------------------------+   +-------------------------------------------+ |
 * |  | High_Speed_Channels¹                      |   | Low_Speed_Channels                        | |
 * |  |                   +-----+     +--------+  |   |                   +-----+     +--------+  | |
 * |  |                   |     | --> | h_ch 0 |  |   |                   |     | --> | l_ch 0 |  | |
 * |  | +-----------+     |     |     +--------+  |   | +-----------+     |     |     +--------+  | |
 * |  | | h_timer 0 | --> |     |                 |   | | l_timer 0 | --> |     |                 | |
 * |  | +-----------+     |     |     +--------+  |   | +-----------+     |     |     +--------+  | |
 * |  |                   |     | --> | h_ch 1 |  |   |                   |     | --> | l_ch 1 |  | |
 * |  |                   |     |     +--------+  |   |                   |     |     +--------+  | |
 * |  |                   |     |                 |   |                   |     |                 | |
 * |  |                   |     |     +--------+  |   |                   |     |     +--------+  | |
 * |  |                   |     | --> | h_ch 2 |  |   |                   |     | --> | l_ch 2 |  | |
 * |  | +-----------+     |     |     +--------+  |   | +-----------+     |     |     +--------+  | |
 * |  | | h_timer 1 | --> |     |                 |   | | l_timer 1 | --> |     |                 | |
 * |  | +-----------+     |     |     +--------+  |   | +-----------+     |     |     +--------+  | |
 * |  |                   |     | --> | h_ch 3 |  |   |                   |     | --> | l_ch 3 |  | |
 * |  |                   |     |     +--------+  |   |                   |     |     +--------+  | |
 * |  |                   | MUX |                 |   |                   | MUX |                 | |
 * |  |                   |     |     +--------+  |   |                   |     |     +--------+  | |
 * |  |                   |     | --> | h_ch 4 |  |   |                   |     | --> | l_ch 4 |  | |
 * |  | +-----------+     |     |     +--------+  |   | +-----------+     |     |     +--------+  | |
 * |  | | h_timer 2 | --> |     |                 |   | | l_timer 2 | --> |     |                 | |
 * |  | +-----------+     |     |     +--------+  |   | +-----------+     |     |     +--------+  | |
 * |  |                   |     | --> | h_ch 5 |  |   |                   |     | --> | l_ch 5 |  | |
 * |  |                   |     |     +--------+  |   |                   |     |     +--------+  | |
 * |  |                   |     |                 |   |                   |     |                 | |
 * |  |                   |     |     +--------+  |   |                   |     |     +--------+  | |
 * |  |                   |     | --> | h_ch 6 |  |   |                   |     | --> | l_ch 6²|  | |
 * |  | +-----------+     |     |     +--------+  |   | +-----------+     |     |     +--------+  | |
 * |  | | h_timer 3 | --> |     |                 |   | | l_timer 3 | --> |     |                 | |
 * |  | +-----------+     |     |     +--------+  |   | +-----------+     |     |     +--------+  | |
 * |  |                   |     | --> | h_ch 7 |  |   |                   |     | --> | l_ch 7²|  | |
 * |  |                   |     |     +--------+  |   |                   |     |     +--------+  | |
 * |  |                   +-----+                 |   |                   +-----+                 | |
 * |  +-------------------------------------------+   +-------------------------------------------+ |
 * +------------------------------------------------------------------------------------------------+
 * ¹ High speed channels are only available when SOC_LEDC_SUPPORT_HS_MODE is defined as 1
 * ² The ESP32C3 does only support six channels, so 6 and 7 are not available on that SoC
 * 
 * The nomenclature of timers in the high speed / low speed blocks is a bit misleading as the idf api 
 * speaks of "speed mode", which, to me, implies that this would be a mode configurable in a specific timer
 * while in reality, it does select a block of timers. I am considering re-naming that to "speed mode block"
 * in my interface impmenentation.
 * 
 * As an example, I would use
 * setTimerFrequency(speedModeBlock, timer, frequency);
 * 
 * ToDo: see, how this can be implemented to provide maximum overlap with the RP2040 pwm hardware so code does 
 * not become overly SoC specific.
 * 
 * Maximum Timer width for PWM:
 * ============================
 * esp32   SOC_LEDC_TIMER_BIT_WIDE_NUM  (20)
 * esp32c3 SOC_LEDC_TIMER_BIT_WIDE_NUM  (14)
 * esp32s2 SOC_LEDC_TIMER_BIT_WIDE_NUM  (14)
 * esp32s3 SOC_LEDC_TIMER_BIT_WIDE_NUM  (14)
 * 
 * Number of Channels:
 * ===================
 * esp32   SOC_LEDC_CHANNEL_NUM         16 (8?)
 * esp32c3 SOC_LEDC_CHANNEL_NUM         (6)
 * esp32s2 SOC_LEDC_CHANNEL_NUM         (8)
 * esp32s3 SOC_LEDC_CHANNEL_NUM 		 8
 *
 * Some SoSs support a mode called HIGHSPEED_MODE which is essentially another full block of PWM hardware 
 * that adds SOC_LEDC_CHANNEL_NUM channels. 
 * Those Architectures have SOC_LEDC_SUPPORT_HS_MODE defined as 1.
 * In esp-idf-4.3 that's currently only the esp32 SOC 
 * 
 * Supports highspeed mode:
 * ========================
 * esp32 SOC_LEDC_SUPPORT_HS_MODE	(1)
 *
 * ToDo: implement awareness of hs mode availablility
 * ==================================================
 * currently, the code just uses a % 8 operation on the pin index to calculate whether to assign a pin to either
 * high speed or low speed pwm blocks. This doesn't make a whole lot of sense since it makes it impossible
 * for Sming devs to actually use the functionality behind it. 
 * Also, it currently does not reflect the fact that different SOCs have a different number of channels per block
 * (specifically, the esp32c3 only has six channels and no highspeed mode). 
 * I will continue in two ways: 
 * - implement the "vanilla" Sming HardwarePWM interface that will hide the underlying architecture but allow up to 16 
 *   channels on an ESP32 
 * - implement overloads for the relevant functions that allow selecting hs mode where applicable. 
 * 
 * ToDo: implement PWM bit width control
 * =====================================
 * the current HardwarePWM implementation does not care about the PWM timer bit width. To leverage the functionality 
 * of the ESP32 hardware, it's necessary to make this configurable. As the width is per timer and all the Sming defined
 * functions are basically per pin, this needs to be an extension of the overal model, exposing at least timers. 
 * This, too, will require a compatible "basic" interface and an advanced interface that allows assiging pins (channels) 
 * to timers and the configuration of the timers themselves. 
 * The esp_idf does not provide a way to read the bit width configured for a channel, but I think it'll be useful to be able
 * to read back this value, not least to find the value for getMaxDuty() for a channel. It will either have to be stored in the
 * module or maybe read from the hardware directly (LEDC_[HL]STIMERx_CONF_REG & 0x1f)
 * 
 * ToDo: implement an abstraction layer
 * ====================================
 * as it stands now, this impelmentation does not provide a function to synchronize all the PWM channels (HardwarePWM::update())
 * It might be a good idea to provide an intermediary abstraction that handles all the low level PWM functions (such as flexible
 * timer to channel assignments, hs/ls mode awareness, pwm bit width etc) and implements the update() function on that level.
 * 
 * hardware technical reference: 
 * =============================
 * ESP32:   https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#ledpwm
 * ESP32c3: https://www.espressif.com/sites/default/files/documentation/esp32-c3_technical_reference_manual_en.pdf#ledpwm
 * 
 * Overview of the whole ledc-system here: 
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html
 * 
 * the ledc interface also exposes some advanced functions such as fades that are then done in hardware. 
 * ToDo: implement a Sming interface for fades
 * 



 * You can use function setPeriod() to change frequency/period.
 * Calculate the max duty as per the formulae give in ESP8266 SDK
 * Max Duty = (Period * 1000) / 45
 *
 * PWM can be generated on up to 8 pins (ie All pins except pin 16)
 * Created on August 17, 2015, 2:27 PM
 *
 * See also ESP8266 Technical Reference, Chapter 12:
 * http://espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf
 *
 */


#include <debug_progmem.h>
//#include <soc/soc_caps.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include <hal/ledc_types.h>
#include <HardwarePWM.h>
#include "singleton.h"
#include "ledc_channel.h"
#include "ledc_timer.h"

namespace{
	uint32_t periodToFrequency(uint32_t period);
	uint32_t frequencyToPeriod(uint32_t freq);

	//ledc_channel_t getChannel(ledc_mode_t);
	//ledc_timer_t getTimer(ledc_mode_t);
} //namespace

#define DEFAULT_RESOLUTION static_cast<ledc_timer_bit_t>(10)
#define DEFAULT_FREQ 400000
#define DEFAULT_CLOCK_SOURCE LEDC_AUTO_CLK

HardwarePWM::HardwarePWM(uint8_t* pins, uint8_t no_of_pins) : channel_count(no_of_pins)
{
	//ledc_timer_config_t ledc_timer;
	//ledc_channel_config_t ledc_channel;
	ledc_mode_t mode;
	#ifdef LEDC_HIGH_SPEED_MODE
		mode=LEDC_HIGH_SPEED_MODE;
	#else
		mode=LEDC_LOW_SPEED_MODE;
	#endif

	debug_d("starting HardwarePWM init");
	periph_module_enable(PERIPH_LEDC_MODULE);
	if((no_of_pins == 0) || (no_of_pins > SOC_LEDC_CHANNEL_NUM))
	{
		return;
	}
	
	/* ToDo: this logic seems broken. What needs to happen:
	 * if LEDC_HIGH_SPEED_MODE, we'll need to check both, HS and LS mode,
	 * else only LS mode
	 */
	#ifdef LEDC_HIGH_SPEED_MODE
		if(Channel::instance()->getFreeChannels(mode)<no_of_pins){
			return;
		}
	#else
		if(ledc_singleton::Channel::instance()->getFreeChannels(mode)<no_of_pins){
			mode=LEDC_LOW_SPEED_MODE; // if low speed mode is available, try it
			if(ledc_singleton::Channel::instance()->getFreeChannels(mode)<no_of_pins){
				return;					// has tried high and low speed mode, not enough channels	
			}
		}
	#endif

	timer = new ledc_timer(mode, (ledc_timer_bit_t) DEFAULT_RESOLUTION, DEFAULT_FREQ, (ledc_clk_cfg_t) DEFAULT_CLOCK_SOURCE);
	//ledc_channel* channel[no_of_pins];
	for(uint8_t i=0;i<no_of_pins;i++){
		channel[i]=new ledc_channel(mode, pins[i], timer->getTimerNumber(), 0);
	}

}

HardwarePWM::~HardwarePWM()
{
	for(uint8_t i = 0; i < channel_count; i++) {
		//stop pwm for all pins and set idle level to 0.
		channel[i]->stop();
	}
}

/* Function Name: getChannel
 * Description: This function is used to get channel number for given pin
 * Parameters: pin - Esp8266 pin number
 */
uint8_t HardwarePWM::getChannel(uint8_t pin)
{
	for(uint8_t i = 0; i < channel_count; i++) {
		if(channel[i]->getPin()==pin) {
			//debug_d("getChannel %d is %d", pin, i);
			return i;
		}
	}
	return PWM_BAD_PIN;
}

/* Function Name: getDutyChan
 * Description: This function is used to get the duty cycle number for a given channel
 * Parameters: chan -Esp8266 channel number
 */
uint32_t HardwarePWM::getDutyChan(uint8_t chan)
{
	if(chan == PWM_BAD_CHANNEL) {
		return 0;
	} else {
		return channel[chan]->getDuty();		
	}
}

/* Function Name: setDutyChan
 * Description: This function is used to set the pwm duty cycle for a given channel. If parameter 'update' is false
 *              then you have to call update() later to update duties.
 * Parameters: chan - channel number
 *             duty - duty cycle value
 *             update - update PWM output
 */
bool HardwarePWM::setDutyChan(uint8_t chan, uint32_t duty, bool update)
{
	if(chan == PWM_BAD_CHANNEL) {
		return false;
	} else if(duty <= maxduty) {
		channel[chan]->setDuty(duty);
		if(update) {
			channel[chan]->updateDuty();
		}
		return true;
	} else {
		debug_d("Duty cycle value too high for current period.");
		return false;
	}
}

/* Function Name: getPeriod
 * Description: This function is used to get Period of PWM.
 *				Period / frequency will remain same for all pins.
 *
 */
uint32_t HardwarePWM::getPeriod()
{
	// sming does not know how to handle different frequencies for channels, this will require an extended interface
	// for now, just report the period for group 0 channel 0 
	return frequencyToPeriod(timer->getTimerFrequency());
}

/* Function Name: setPeriod
 * Description: This function is used to set Period of PWM.
 *				Period / frequency will remain same for all pins.
 */
void HardwarePWM::setPeriod(uint32_t period)
{
	timer->setTimerFrequency(periodToFrequency(period));
	update();
}

/* Function Name: update
 * Description: This function is used to actually update the PWM.
 */
void HardwarePWM::update()
{
	for(uint8_t i=0;i<channel_count;i++){
		channel[i]->updateDuty();
	}
}

uint32_t HardwarePWM::getFrequency(uint8_t pin)
{
	return timer->getTimerFrequency();
}

namespace{

	uint32_t periodToFrequency(uint32_t period){
		if(period == 0){
			return 0;
		}else{
			return (1000000 / period);
		}
	}

	uint32_t frequencyToPeriod(uint32_t freq){
		if(freq == 0) {
			return 0;
		} else {
			return (1000000 / freq);
		}
	}

}