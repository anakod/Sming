#ifndef LEDC_CHANNEL_H
#include "ledc_channel.h"

ledc_channel::ledc_channel(ledc_mode_t mode, int gpio, ledc_timer_t timer, uint32_t duty, int hpoint){
    periph_module_enable(PERIPH_LEDC_MODULE);
    channel_config.speed_mode   =   mode;
    channel_config.gpio_num     =   gpio;
    channel_config.channel      =   ledc_singleton::Channel::instance()->getChannel(mode);
    channel_config.intr_type    =   LEDC_INTR_DISABLE;
    channel_config.timer_sel    =   timer;
    channel_config.duty         =   duty;
    channel_config.hpoint       =   hpoint;
    
    esp_err_t err=ledc_channel_config(&channel_config);
    if(err==ESP_OK){
        bindChannelTimer(timer);
        debug_d("configured channel %d pin %d to user timer %d", channel_config.channel, channel_config.gpio_num, timer->getTimerNum());
    }
}

ledc_channel::ledc_channel(ledc_mode_t mode, int gpio, ledc_timer_t timer, uint32_t duty){
    ledc_channel(mode, gpio, timer, duty, (int) 0);
}

ledc_channel::ledc_channel(ledc_mode_t mode, int gpio, ledc_timer_t timer){
    ledc_channel(mode, gpio, timer, (uint32_t) 0, (int) 0);
}

ledc_channel::ledc_channel(ledc_mode_t mode, int gpio){
    ledc_channel(mode, gpio, ledc_singleton::Timer::instance()->getTimer(mode), (uint32_t) 0, (int) 0);
}

ledc_channel::~ledc_channel(){
    stop((uint32_t)0);
}
/*
ledc_channel::channelConfig(const ledc_channel_config_t *ledc_conf){

}
*/
esp_err_t ledc_channel::updateDuty(void){
    return ledc_update_duty(channel_config.speed_mode, channel_config.channel);
}

esp_err_t ledc_channel::setPin(int gpio_num){
    esp_err_t err = ledc_set_pin(gpio_num, channel_config.speed_mode, channel_config.channel);
    if(err==ESP_OK){
        channel_config.gpio_num=gpio_num;
    }
    return err;
}

esp_err_t ledc_channel::stop(uint32_t idle_level){
    return ledc_stop(channel_config.speed_mode, channel_config.channel, idle_level);
}

esp_err_t ledc_channel::setDutyWithHpoint(uint32_t duty, uint32_t hpoint){
    esp_err_t err = ledc_set_duty_with_hpoint(channel_config.speed_mode, channel_config.channel, duty, hpoint);
    if(err==ESP_OK){
        channel_config.duty=duty;
        channel_config.hpoint=hpoint;
    }
    return err;
}

esp_err_t ledc_channel::setDuty(uint32_t duty){
    esp_err_t err=ledc_set_duty(channel_config.speed_mode, channel_config.channel, duty);
    if(err==ESP_OK){
        channel_config.duty=duty;
    }
    return err;
}

esp_err_t ledc_channel::bindChannelTimer(ledc_timer_t timer){
    esp_err_t err = ledc_bind_channel_timer(channel_config.speed_mode, channel_config.channel, timer);
    if(err==ESP_OK){
        channel_config.timer_sel=timer;
    }
    return err;
}
#endif