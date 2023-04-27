#pragma once
#include <stdint.h>
#include <hal/ledc_types.h>
#define PWM_MAX_TIMER 8

namespace ledc_singleton{   
template <typename C> class Singleton
{
public:
	static C* instance()
	{
		if(!_instance)
			_instance = new C();
		return _instance;
	}
	virtual ~Singleton()
	{
		_instance = 0;
	}

private:
	static C* _instance;

protected:
	Singleton()
	{
	}
};

template <typename C> C* Singleton<C>::_instance = 0;

class Timer : public Singleton<Timer>
{
	friend class Singleton<Timer>;

public:
	~Timer(){};

	/**
      * @brief get an unspecified timer 
      * @param none
      * @note as the only timer available eveywhere are low speed, this is low speed
      * @retval ledc_timer_t <the_timer>, 
      *         LEDC_TIMER_MAX if no more timers are available
      */
	ledc_timer_t getTimer()
	{
		return getLSTimer();
	};

	/**
      * @brief get a low speed timer 
      * @param none
      * @retval ledc_timer_t <the_timer>
      *         LEDC_TIMER_MAX if no more timers are available
      */
	ledc_timer_t getLSTimer()
	{
		return getTimer(LEDC_LOW_SPEED_MODE);
	};
      #ifdef LEDC_HIGH_SPEED_MODE
	/**
      * @brief get a high speed timer 
      * @param none
      * @note you are not guaranteed that the SoC has high speed timers
      * @retval ledc_timer_t <the_timer>
      *         LEDC_TIMER_MAX if no more timers are available
      */
	ledc_timer_t getHSTimer()
	{
		return getTimer(LEDC_HIGH_SPEED_MODE);
	};


	/**
      * @brief free a high speed timer
      * @param ledc_timer_t <the_timer>
      * @note using this to free a low speed mode timer will lead to unexpected effects 
      *       because the seemingly freed timer might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeHSTimer(ledc_timer_t timer)
	{
		freeTimer(LEDC_HIGH_SPEED_MODE, timer);
	};

	/**
      * @brief free a high speed timer
      * @param const int <the_timer>
      * @note using this to free a low speed mode timer will lead to unexpected effects 
      *       because the seemingly freed timer might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeHSTimer(const int timer)
	{
		freeTimer(LEDC_HIGH_SPEED_MODE, timer);
	};
      #endif

	/**
      * @brief get a timer of specified speed mode
      * @param ledc_mode_t <requested mode> (LEDC_LOW_SPEED_MODE or LEDC_HIGH_SPEED_MODE)
      * @note you are not guaranteed that the SoC has high speed timers
      *       also, you are required to keep track of the speed mode of the timers yourself.
      * @retval ledc_timer_t <the_timer>
      *         LEDC_TIMER_MAX if no more timers with the specified mode are available
      */
	ledc_timer_t getTimer(ledc_mode_t mode);

	/**
      * @brief get a timer of specified speed mode
      * @param const int <requested mode> (LEDC_LOW_SPEED_MODE or LEDC_HIGH_SPEED_MODE)
      * @note you are not guaranteed that the SoC has high speed timers
      *       also, you are required to keep track of the speed mode of the timers yourself.
      * @retval ledc_timer_t <the_timer>
      *         LEDC_TIMER_MAX if no more timers with the specified mode are available
      */
	ledc_timer_t getTimer(const int mode)
	{
		return getTimer((ledc_mode_t)mode);
	};

	/**
      * @brief free an unspecified timer
      * @param ledc_timer_t <the_timer>
      * @note as the only timer available eveywhere are low speed, this is low speed timers
      *       using this to free a high speed mode timer will lead to unexpected effects 
      *       because the seemingly freed timer might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeTimer(ledc_timer_t timer)
	{
		freeLSTimer(timer);
	};

	/**
      * @brief free an unspecified timer
      * @param const int <the_timer>
      * @note as the only timer available eveywhere are low speed, this is low speed timers
      *       using this to free a high speed mode timer will lead to unexpected effects 
      *       because the seemingly freed timer might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeTimer(const int timer)
	{
		freeLSTimer((ledc_timer_t)timer);
	};

	/**
      * @brief free a low speed timer
      * @param ledc_timer_t <the_timer>
      * @note using this to free a high speed mode timer will lead to unexpected effects 
      *       because the seemingly freed timer might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeLSTimer(ledc_timer_t timer)
	{
		freeTimer(LEDC_LOW_SPEED_MODE, timer);
	};

	/**
      * @brief free a low speed timer
      * @param const int <the_timer>
      * @note using this to free a high speed mode timer will lead to unexpected effects 
      *       because the seemingly freed timer might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeLSTimer(const int timer)
	{
		freeTimer(LEDC_LOW_SPEED_MODE, (ledc_timer_t)timer);
	};

	/**
      * @brief free a timer
      * @param ledc_timer_t <the_timer>
      * @param ledc_mode_t <the mode>
      * @note if you are mixing low speed and high speed timers, this is the safest call to use
      * @retval none
      */
	void freeTimer(ledc_mode_t mode, ledc_timer_t timer);

	/**
      * @brief free a timer
      * @param ledc_timer_t <the_timer>
      * @param const int <the mode>
      * @note if you are mixing low speed and high speed timers, this is the safest call to use
      * @retval none
      */
	void freeTimer(const int mode, ledc_timer_t timer)
	{
		freeTimer((ledc_mode_t)mode, timer);
	};

	/**
      * @brief free a timer
      * @param const int <the_timer>
      * @param ledc_mode_t <the mode>
      * @note if you are mixing low speed and high speed timers, this is the safest call to use
      * @retval none
      */
	void freeTimer(ledc_mode_t mode, const int timer)
	{
		freeTimer(mode, (ledc_timer_t)timer);
	};

	/**
      * @brief free a timer
      * @param const int <the_timer>
      * @param const int <the mode>
      * @note if you are mixing low speed and high speed timers, this is the safest call to use
      * @retval none
      */
	void freeTimer(const int mode, const int timer)
	{
		freeTimer((ledc_mode_t)mode, (ledc_timer_t)timer);
	};

protected:
	Timer();
	bool isUsed[LEDC_SPEED_MODE_MAX][LEDC_TIMER_MAX];
};

class Channel : public Singleton<Channel>
{
	friend class Singleton<Channel>;

public:
	~Channel(){};

	/**
      * @brief get an unspecified channel
      * @param none
      * @note as the only channels available eveywhere are low speed, this is low speed
      * @retval ledc_channel_t <the_channel>, 
      *         LEDC_CHANNEL_MAX if no more CHANNELS are available
      */
	ledc_channel_t getChannel()
	{
		return getLSChannel();
	};

	/**
      * @brief get a low speed channel
      * @param none
      * @retval ledc_channel_t <the_channel>, 
      *         LEDC_CHANNEL_MAX if no more low speed CHANNELS are available
      */
	ledc_channel_t getLSChannel()
	{
		return getChannel(LEDC_LOW_SPEED_MODE);
	};

      #ifdef LEDC_HIGH_SPEED_MODE
	/**
      * @brief get a high speed channel
      * @param none
      * @note you are not guaranteed that the SoC has high speed channels
      * @retval ledc_channel_t <the_channel>, 
      *         LEDC_CHANNEL_MAX if no more high speed CHANNELS are available
      */
	ledc_channel_t getHSChannel()
	{
		return getChannel(LEDC_HIGH_SPEED_MODE);
	};

	/**
      * @brief free a high speed channel
      * @param ledc_channel_t <the_channel>
      * @note using this to free a low speed channel will lead to unexpected effects 
      *       because the seemingly freed channel might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeHSChannel(ledc_channel_t channel)
	{
		freeChannel(LEDC_HIGH_SPEED_MODE, channel);
	};

	/**
      * @brief free a high speed channel
      * @param const int <the_channel>
      * @note using this to free a low speed channel will lead to unexpected effects 
      *       because the seemingly freed channel might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeHSChannel(const int channel)
	{
		freeChannel(LEDC_HIGH_SPEED_MODE, (ledc_channel_t)channel);
	};

      #endif 

	/**
      * @brief get a channel with a defined speed mode
      * @param ledc_mode_t mode
      * @note you are not guaranteed that the SoC has high speed channels
      *       also, you are required to keep track of the speed mode of the channels yourself.
      * @retval ledc_channel_t <the_channel>, 
      *         LEDC_CHANNEL_MAX if no more channels with the specified mode are available
      */
	ledc_channel_t getChannel(ledc_mode_t mode);

	/**
      * @brief get a channel with a defined speed mode
      * @param const int mode
      * @note you are not guaranteed that the SoC has high speed channels
      *       also, you are required to keep track of the speed mode of the channels yourself.
      * @retval ledc_channel_t <the_channel>, 
      *         LEDC_CHANNEL_MAX if no more channels with the specified mode are available
      */
	ledc_channel_t getChannel(const int mode)
	{
		return getChannel((ledc_mode_t)mode);
	};

	/**
      * @brief free an unspecified channel
      * @param ledc_channel_t <the_channel>
      * @note as the only channels available eveywhere are low speed, this is low speed channels
      *       using this to free a high speed channel will lead to unexpected effects 
      *       because the seemingly freed channel might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeChannel(ledc_channel_t channel)
	{
		freeLSChannel(channel);
	};

	/**
      * @brief free an unspecified channel
      * @param const int <the_channel>
      * @note as the only channels available eveywhere are low speed, this is low speed channels
      *       using this to free a high speed channel will lead to unexpected effects 
      *       because the seemingly freed channel might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeChannel(const int channel)
	{
		freeLSChannel((ledc_channel_t)channel);
	};

	/**
      * @brief free a low speed channel
      * @param ledc_channel_t <the_channel>
      * @note using this to free a high speed channel will lead to unexpected effects 
      *       because the seemingly freed channel might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeLSChannel(ledc_channel_t channel)
	{
		freeChannel(LEDC_LOW_SPEED_MODE, channel);
	};

	/**
      * @brief free a low speed channel
      * @param const int <the_channel>
      * @note using this to free a high speed channel will lead to unexpected effects 
      *       because the seemingly freed channel might be re-used and thus re-configured later on 
      * @retval none
      */
	void freeLSChannel(const int channel)
	{
		freeChannel(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel);
	};

	/**
      * @brief free a channel
      * @param ledc_mode_t <the mode>
      * @param ledc_channel_t <the_channel>
      * @note if you are mixing low speed and high speed channels, this is the safest call to use
      * @retval none
      */
	void freeChannel(ledc_mode_t mode, ledc_channel_t channel);

	/**
      * @brief free a channel
      * @param const int <the mode>
      * @param ledc_channel_t <the_channel>
      * @note if you are mixing low speed and high speed channels, this is the safest call to use
      * @retval none
      */
	void freeChannel(const int mode, ledc_channel_t channel)
	{
		freeChannel((ledc_mode_t)mode, channel);
	};

	/**
      * @brief free a channel
      * @param ledc_mode_t int <the mode>
      * @param const int <the_channel>
      * @note if you are mixing low speed and high speed channels, this is the safest call to use
      * @retval none
      */
	void freeChannel(ledc_mode_t mode, const int channel)
	{
		freeChannel(mode, (ledc_channel_t)channel);
	};

	/**
      * @brief free a channel
      * @param const int <the mode>
      * @param const int <the_channel>
      * @note if you are mixing low speed and high speed channels, this is the safest call to use
      * @retval none
      */
	void freeChannel(const int mode, const int channel)
	{
		freeChannel((ledc_mode_t)mode, (ledc_channel_t)channel);
	};

	/**
      * @brief return number of free channels in mode
      * @param const int <the mode>
      * @note this is in no way thread safe.
      * @retval number of unassigned channels
      */
      uint8_t getFreeChannels(ledc_mode_t mode);

protected:
	Channel();
	bool isUsed[LEDC_SPEED_MODE_MAX][LEDC_CHANNEL_MAX];
};
} // end namespace ledc_singleton