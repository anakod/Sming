/*
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * SPIBase.h
 *
 *  Created on: Mar 2, 2016
 *      Author: harry-boe
 *
 */

#pragma once

#include "SPISettings.h"
#include <stddef.h>

/**
 * @defgroup base_spi SPI support classes
 * @brief    Provides SPI support
 * @{
 */

/*
 * @brief Base class/interface for SPI implementations
 */
class SPIBase
{
public:
	virtual ~SPIBase()
	{
	}

	/**
	 * @brief Initialize the SPI bus by setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low, and SS high.
	 */
	virtual void begin() = 0;

	/**
	 * @brief Disable the SPI bus (leaving pin modes unchanged).
	 */
	virtual void end() = 0;

	/**
	 * @brief Initialize the SPI bus using the defined SPISettings.
	 */
	virtual void beginTransaction(SPISettings& mySettings) = 0;

	/**
	 * @brief Stop using the SPI bus. Normally this is called after de-asserting the chip select, to allow other libraries to use the SPI bus.
	 */
	virtual void endTransaction() = 0;

	/**
	 * @name Send/receive some data
	 * @{
	 *
	 * SPI transfer is based on a simultaneous send and receive: the received data is returned in receivedVal (or receivedVal16).
	 * In case of buffer transfers the received data is stored in the buffer in-place (the old data is replaced with the data received).
	 *
	 * 		receivedVal = SPI.transfer(val)
	 * 		receivedVal16 = SPI.transfer16(val16)
	 * 		SPI.transfer(buffer, size)
	 */

	/**
	 * @brief Send/receive one bytes of data
	 * @param val The byte to send
	 * @retval uint8_t The received byte
	 */
	virtual uint8_t transfer(uint8_t val) = 0;

	/**
	 * @brief Send/receive one 16-bit word of data
	 * @param val The word to send
	 * @retval uint8_t The received word
	 */
	virtual uint16_t transfer16(uint16_t val) = 0;

	/**
	 * @brief Send/receive a variable-length block of data
	 * @param buffer IN: The data to send; OUT: The received data
	 * @param size Number of bytes to transfer
	 */
	virtual void transfer(uint8_t* buffer, size_t size) = 0;

	/** @} */

	/**
	 * @brief  Default settings used by the SPI bus
	 * until reset by beginTransaction(SPISettings)
	 *
	 * Note: not included in std Arduino lib
	 */
	SPISettings SPIDefaultSettings;
};

/** @} */
