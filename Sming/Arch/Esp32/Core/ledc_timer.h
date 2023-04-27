#ifndef LEDC_TIMER_H
#define LEDC_TIMER_H
#include <debug_progmem.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "singleton.h"
#include <hal/ledc_types.h>

#ifndef LEDC_CHANNEL_H
#include "ledc_channel.h"
#endif

class ledc_timer {
    public:
    /**
     * @brief Construct a new ledc timer object
     * 
     * @param mode 
     * @param duty_resolution 
     * @param freq 
     * @param clk_cfg 
     */
        ledc_timer(ledc_mode_t mode, ledc_timer_bit_t duty_resolution, uint32_t freq, ledc_clk_cfg_t clk_cfg);
    /**
     * @brief Destroy the ledc timer object
     * 
     */
        ~ledc_timer(void);

    /**
     * @brief Set the Timer object
     * 
     * @param clock_divider 
     * @param duty_resolution 
     * @param clk_src 
     * 
     * @return esp_err_t
     */
       esp_err_t setTimer(uint32_t clock_divider, ledc_timer_bit_t duty_resolution, ledc_clk_src_t clk_src);

    /**
     * @brief set the Frequency for this timer object
     * 
     * @param frequency
     * @return esp_err_t 
     */
        esp_err_t setTimerFrequency(uint32_t freq);

    /**
     * @brief reset this timer
     * 
     * @return esp_err_t 
     */
        esp_err_t timerReset(void);

    /**
     * @brief pause this timer
     * 
     * @return esp_err_t 
     */
        esp_err_t timerPause(void);

    /**
     * @brief resume this timer
     * 
     * @return esp_err_t 
     */
        esp_err_t timerResume(void);

    /*
     *  esp_err_t timerBindChannel(ledc_channel_t channel);
     */

    /**
     * @brief Get the Timer Number 
     * use this to get the timer number to use in the ledc_channel constructor or ledc_channel.bindTimer()
     * 
     * @return ledc_timer_t Timer Number 
     */
        ledc_timer_t getTimerNumber(void);

    /**
     * @brief Get the Timer Mode 
     * return the timer mode
     * 
     * @return ledc_mode_t Timer Mode
     */
        ledc_mode_t getTimerMode(void);

    /**
     * @brief Get the Duty Resolution 
     * get the resolution in bits configured for this timer
     * 
     * @return ledc_timer_bit_t Timer Resolution
     */
        ledc_timer_bit_t getDutyResolution(void);        
    
    /**
     * @brief Get the Timer Frequency
     * 
     * @return uint32_t Frequency
     */
        uint32_t getTimerFrequency(void);

    /**
     * @brief Get the Clock Divider object
     * 
     * @return uint32_t clock divider
     */
        uint32_t getClockDivider(void);
    private:
        ledc_timer_config_t timer_conf;
        ledc_timer_t timer;
        uint32_t clock_divider;
};

#endif