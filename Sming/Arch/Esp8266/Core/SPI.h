/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * SPI.h
 *
 *  Created on: Mar 2, 2016
 *      Author: harry-boe
 *
 */

/** @defgroup hw_spi SPI Hardware support
 *  @brief    Provides hardware SPI support
 */

#pragma once

#include "SPIBase.h"
#include "SPISettings.h"

//#define SPI_DEBUG  1

// for compatibility when porting from Arduino
#define SPI_HAS_TRANSACTION 0

#define SPI_NO 1

/**
 * @brief  Hardware SPI object
 * @addtogroup hw_spi
 * @{
 */

class SPIClass : public SPIBase
{
public:
	bool begin() override;

	/**
	 * @brief Method for compatibility with Arduino API. Provides NOP
	 */
	void end() override
	{
	}

	/** @brief Read one byte from SPI without setting up registers
	 * 	@param	none
	 * 	@retval	byte received
	 *
	 * 	 used for performance tuning when doing continuous reads
	 * 	 this method does not reset the registers , so make sure
	 * 	 that a regular transfer(data) call was performed
	 *
	 * 	 Note: this method is not found on the Arduino API
	 *
	 * 	 USE WITH CARE !!
	 *
	 */
	uint8_t read8();

	uint32_t transfer32(uint32_t val, uint8_t bits = 32) override;

	using SPIBase::transfer;

	void transfer(uint8_t* buffer, size_t numberBytes) override;

protected:
	void prepare(SPISettings& settings) override;
};

/** @brief  Global instance of SPI class */
extern SPIClass SPI;
