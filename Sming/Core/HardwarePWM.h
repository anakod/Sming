/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * HardwarePWM.h
 *
 * Original Author: https://github.com/hrsavla
 *
 * This HW_PWM library enables Sming framework user to use ESP SDK PWM API
 * Period of PWM is fixed to 1000ms / Frequency = 1khz
 * Duty at 100% = 22222. Duty at 0% = 0
 * You can use function setPeriod() to change frequency/period.
 * Calculate the Duty as per the formulae give in ESP8266 SDK
 * Duty = (Period *1000)/45
 *
 * PWM can be generated on up to 8 pins (ie All pins except pin 16)
 * Created on August 17, 2015, 2:27 PM
 *
 ****/

/** @defgroup   hw_pwm Hardware PWM functions
 *  @brief      Provides hardware pulse width modulation functions
 *  @{
*/

#pragma once

#include <cstdint>
#include <driver/pwm.h>

#define PWM_BAD_CHANNEL 0xff ///< Invalid PWM channel
#define PWM_BAD_PIN 	0xff ///< Invalid Pin / Pin is not attached to any channel

#if SMING_ARCH==Esp32
#include "../Arch/Esp32/Core/ledc_timer.h"
#include "../Arch/Esp32/Core/ledc_channel.h"
#endif

/// Hardware pulse width modulation
class HardwarePWM
{
public:
	/** @brief  Instantiate hardware PWM object
     *  @param  pins Pointer to array of pins to control
     *  @param  no_of_pins Quantity of elements in array of pins
     */
	HardwarePWM(uint8_t* pins, uint8_t no_of_pins);
	virtual ~HardwarePWM();

	/** @brief  Set PWM duty cycle
     *  @param  pin GPIO to set
     *  @param  duty Value of duty cycle to set pin to
     *  @retval bool True on success
     *  @note   Default frequency is 1khz but can be varied by various function
     */
	bool analogWrite(uint8_t pin, uint32_t duty)
	{
		return setDuty(pin, duty);
	}

	/** @brief  Set PWM duty cycle for a channel
     *  @param  chan Channel to set
     *  @param  duty Value of duty cycle to set channel to
     *  @param  update Update PWM output
     *  @retval bool True on success
     */
	bool setDutyChan(uint8_t chan, uint32_t duty, bool update = true);

	/** @brief  Set PWM duty cycle
     *  @param  pin GPIO to set
     *  @param  duty Value of duty cycle to set pin to
     *  @param  update Update PWM output
     *  @retval bool True on success
     *  @note   This function is used to set the pwm duty cycle for a given pin. If parameter 'update' is false
     *        	then you have to call update() later to update duties.
     */
	bool setDuty(uint8_t pin, uint32_t duty, bool update = true)
	{
		uint8_t chan = getChannel(pin);
		return setDutyChan(chan, duty, update);
	}

	/** @brief  Get PWM duty cycle
	 *  @param  chan Channel to get duty cycle for
	 *  @retval uint32_t Value of PWM duty cycle
	 */
	uint32_t getDutyChan(uint8_t chan);

	/** @brief  Get PWM duty cycle
     *  @param  pin GPIO to get duty cycle for
     *  @retval uint32_t Value of PWM duty cycle
     */
	uint32_t getDuty(uint8_t pin)
	{
		uint8_t chan = getChannel(pin);
		return getDutyChan(chan);
	}

	/** @brief  Set PWM period
     *  @param  period PWM period
     *  @note   All PWM pins share the same period
     */
	void setPeriod(uint32_t period);

	/** @brief  Get PWM period
     *  @retval uint32_t Value of PWM period
     */
	uint32_t getPeriod();

	/** @brief  Get channel number for a pin
     *  @param  pin GPIO to interrogate
     *  @retval uint8_t Channel of GPIO
     */
	uint8_t getChannel(uint8_t pin);

	/** @brief  Get the maximum duty cycle value
     *  @retval uint32_t Maximum permissible duty cycle
     *  @note   Attempt to set duty of a pin above this value will fail
     */
	uint32_t getMaxDuty()
	{
		return maxduty;
	}

	/** @brief  This function is used to actually update the PWM.
	 */
	void update();
	
	/** @brief Get PWM Frequency
	 *  @retval uint32_t Value of Frequency 
	*/
	uint32_t getFrequency(uint8_t pin);

private:
	uint8_t channel_count;
	uint8_t channels[PWM_CHANNEL_NUM_MAX];
	uint32_t maxduty;

	#if SMING_ARCH==Esp32
	ledc_channel* channel[PWM_CHANNEL_NUM_MAX];
	ledc_timer* timer;
	#endif
};

/** @} */
