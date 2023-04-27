
#ifndef LEDC_TIMER_H
#include "ledc_timer.h"
#define LEDC_TIMER_H
#endif 

//namespace ledc_singleton{

ledc_timer::ledc_timer(ledc_mode_t mode, ledc_timer_bit_t duty_resolution, uint32_t freq, ledc_clk_cfg_t clk_cfg){
    timer_conf.speed_mode         =   mode;
    timer_conf.timer_num          =   ledc_singleton::Timer::instance()->getTimer(mode);
    timer_conf.duty_resolution    =   duty_resolution;
    timer_conf.freq_hz            =   freq;
    timer_conf.clk_cfg            =   clk_cfg;
    
    ledc_timer_config(&timer_conf);
}

ledc_timer::~ledc_timer(void){
    ledc_timer_pause(timer_conf.speed_mode, timer_conf.timer_num);
    ledc_singleton::Timer::instance()->freeTimer(timer_conf.speed_mode, timer_conf.timer_num);
}

esp_err_t ledc_timer::setTimer(uint32_t clock_divider, ledc_timer_bit_t duty_resolution, ledc_clk_src_t clk_src){
    esp_err_t err=ledc_timer_set(
        timer_conf.speed_mode,
        timer_conf.timer_num,
        clock_divider,
        duty_resolution,
        clk_src
    );
    if(err==ESP_OK){
        timer_conf.duty_resolution    = duty_resolution;
        //timer_conf.clk_cfg            = clk_cfg;
        clock_divider=clock_divider;
    }
    debug_d("configured timer %i",timer_conf.timer_num);
    return err;
}

esp_err_t ledc_timer::setTimerFrequency(uint32_t freq){
    esp_err_t err=ledc_set_freq(
        timer_conf.speed_mode,
        timer_conf.timer_num,
        freq
    );
    if(err==ESP_OK){
        timer_conf.freq_hz      = freq;
    }
    return err;
}

esp_err_t ledc_timer::timerReset(void){
    return ledc_timer_rst(timer_conf.speed_mode, timer_conf.timer_num);
}

esp_err_t ledc_timer::timerPause(void){
    return ledc_timer_pause(timer_conf.speed_mode, timer_conf.timer_num);
}

esp_err_t ledc_timer::timerResume(void){
    return ledc_timer_resume(timer_conf.speed_mode, timer_conf.timer_num);
}
/*
ledc_timer::timerBindChannel(ledc_channel_t channel){
    esp_err_t err=ledc_bind_channel_timer(timer_conf.speed_mode, channel, timer_conf.timer_num);
    if(err==ESP_OK)
}
*/
ledc_timer_t ledc_timer::getTimerNumber(void){
    return timer_conf.timer_num;
}

ledc_mode_t ledc_timer::getTimerMode(void){
    return timer_conf.speed_mode;
}

ledc_timer_bit_t ledc_timer::getDutyResolution(void){
    return timer_conf.duty_resolution;
}

uint32_t ledc_timer::getTimerFrequency(void){
    return timer_conf.freq_hz;
}

uint32_t ledc_timer::getClockDivider(void){
    return clock_divider;
}
// } // end namespace ledc_singleton