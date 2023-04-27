#ifndef LEDC_CHANNEL_H
#define LEDC_CHANNEL_H
#include <debug_progmem.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "singleton.h"
#include <hal/ledc_types.h>

#ifndef LEDC_TIMERS_H
#include "ledc_timer.h"
#endif
/**
 * @brief ledc_channel class
 * this class is primarily intended to be used by the Sming HardwarePWM implementation for ESP32 class MCUs. 
 * while it should be possible to use it directly in user code, it is not generally advised as the plan is to 
 * expose the full esp32 ledc_ interface through the HardwarePWM interface.
 * 
 * If you chose to use this directly, though, the singleton class at the heart of this *should* at least make sure
 * that hardware is correctly shared between HardwarePWM use and direct use as both channels and timers are 
 * managed by the Timer and Channel classes, respectively.
 */
class ledc_channel{
    public:
    /**
     * @brief Construct a new ledc channel object
     * 
     * @param mode speed mode (see ledc_mode_t typedef for options)
     * @param gpio the pin to attach to the channel 
     * @param timer the timer to use for this channel. Get the timer by instanciating a ledc_timer object and calling the ledc_timer.getTimer() member function
     * @param duty the intial duty cycle
     * @param hpoint the hpoint as defined by the esp-idf (max: 0xfff)
     */
        ledc_channel(ledc_mode_t mode, int gpio, ledc_timer_t timer, uint32_t duty, int hpoint);

    /**
     * @brief Construct a new ledc channel object - reduced interface using default hpoint
     * 
     * @param mode speed mode (see ledc_mode_t typedef for options)
     * @param gpio the pin to attach to the channel 
     * @param timer the timer to use for this channel. Get the timer by instanciating a ledc_timer object and calling the ledc_timer.getTimer() member function
     * @param duty the intial duty cycle
     */
        ledc_channel(ledc_mode_t mode, int gpio, ledc_timer_t timer, uint32_t duty);
    
    /**
     * @brief Construct a new ledc channel object - reduced interface using default hpoint and duty
     * 
     * @param mode speed mode (see ledc_mode_t typedef for options)
     * @param gpio the pin to attach to the channel 
     * @param timer the timer to use for this channel. Get the timer by instanciating a ledc_timer object and calling the ledc_timer.getTimer() member function
     */
        ledc_channel(ledc_mode_t mode, int gpio, ledc_timer_t timer);
        
    /**
     * @brief Construct a new ledc channel object - reduced interface using default hpoint, duty and timer. 
     * The timer will be automatically assigned using the ledc_timer.getTimer() function. 
     * Caution: if you are using this to extend the sming HardwarePWM function, this constructor will use a new timer
     * for every channel generated which will break the functionality of HardwarePWM to, for example, setting the PWM 
     * frequency for all channels.
     * 
     * @param mode speed mode (see ledc_mode_t typedef for options)
     * @param gpio the pin to attach to the channel 
     */
        ledc_channel(ledc_mode_t mode, int gpio);
        
    /**
     * @brief destructor for the channel. Frees the channel to be then re-used by other instances
     * 
     */
        ~ledc_channel();
    
        //esp_err_t channelConfig(const ledc_channel_config_t *ledc_conf);

    /**
     * @brief update the duty cycle of the channel
     * call this function after you have set the duty cycle either by calling setDutyWithHpoint or setDuty as those two functions
     * only set the duty cycle value but not enact it yet. This allows multiple channels to be synchronized.
     * 
     * @return esp_err_t 
     */

        esp_err_t updateDuty(void);
    
    /**
     * @brief Set the Pin 
     * if the channel has been created with one of the reduced interface constructors without setting the actual pin, the pin can be configured here
     * 
     * @param gpio_num 
     * @return esp_err_t 
     */
        esp_err_t setPin(int gpio_num);

    /**
     * @brief stop the PWM on this channel and set the output to 0
     * 
     * @return esp_err_t 
     */
        esp_err_t stop(){
            return stop(0);
        };

    /**
     * @brief stop the PWM on this channel and set the output to idle_level
     * 
     * @param idle_level 
     * @return esp_err_t 
     */
        esp_err_t stop(uint32_t idle_level);
    
    /**
     * @brief Set the Duty cycle With Hpoint 
     * 
     * @param duty 
     * @param hpoint 
     * @return esp_err_t 
     */
        esp_err_t setDutyWithHpoint(uint32_t duty, uint32_t hpoint);

    /**
     * @brief Set the Duty cycle
     * 
     * @param duty 
     * @return esp_err_t 
     */
        esp_err_t setDuty(uint32_t duty);

    /**
     * @brief Get the Hpoint configured for this channel
     * 
     * @return int 
     */
        int getHpoint(){
            return channel_config.hpoint;
        };

    /**
     * @brief Get the current Duty cycle
     * 
     * @return uint32_t 
     */
        uint32_t getDuty(){
            return channel_config.duty;
        };

    /**
     * @brief Get the assigned gpio Pin 
     * 
     * @return int the Pin
     */
        int getPin(){
            return channel_config.gpio_num;
        }
    /**
     * @brief bind the channel object to an existing timer
     * 
     * @param timer 
     * @return esp_err_t 
     */
        esp_err_t bindChannelTimer(ledc_timer_t timer);

        /* omit fades for now...
        esp_err_t setFade(uint32_t duty, ledc_duty_direction_t fade_direction, uint32_t step_num, uint32_t duty_cycle_num, uint32_t duty_scale);
        esp_err_t setFadeWithStep(uint32_t target_duty, uint32_t scale, uint32_t cycle_num);
        esp_err_t setFadeWithTime(uint32_t target_duty, int max_fade_time_ms);
        esp_err_t fadeStart(ledc_fade_mode_t fade_mode)
        esp_err_t fadeFuncInstall(int intr_alloc_flags);
        esp_err_t fadeFuncUninstall();
        esp_err_t ledc_isr_register(void (*fn)(void *), void *arg, int intr_alloc_flags, ledc_isr_handle_t *handle );
        */
    private:
        ledc_channel_config_t channel_config;

};

#endif