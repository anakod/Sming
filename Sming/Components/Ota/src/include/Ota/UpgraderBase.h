/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Ota.h
 *
 * This header includes all unified Over-The-Air functions.
 *
*/

#pragma once
#include <Storage.h>

namespace Ota
{
class UpgraderBase
{
public:
	using Partition = Storage::Partition;

	virtual ~UpgraderBase()
	{
	}

	/**
	 * @brief Prepares a partition for an upgrade.
	 * 		  The preparation is bootloader and architecture dependant.
	 */
	virtual bool begin(Partition partition, size_t size) = 0;

	/**
	 * @brief Writes chunk of data to the partition set in ``begin()``.
	 */
	virtual size_t write(const uint8_t* buffer, size_t size) = 0;

	/**
	 * @brief Finilizes the partition upgrade.
	 */
	virtual bool end() = 0;

	/**
	* @brief Aborts a partition upgrade
	*/
	virtual bool abort()
	{
		return false;
	}

	/**
	 * @brief Sets the default parition from where the application will be booted on next restart.
	 */
	virtual bool setBootPartition(Partition partition) = 0;

	/**
	 * @brief Gets information about the parition that is set as the default one to boot.
	 * @note The returned parition can be different than the current running partition.
	 */
	virtual Partition getBootPartition(void) = 0;

	/**
	 * @brief Gets information about the parition from which the current application is running.
	 * @note The returned parition can be different than the default boot partition.
	*/
	virtual Partition getRunningPartition(void) = 0;

	/**
	 * @brief Gets the next bootable partition that can be used after successful OTA upgrade
	 */
	virtual Partition getNextBootPartition(Partition startFrom = {}) = 0;

	/**
	 * @brief Gets information about all bootable partitions.
	 */
	Storage::Iterator getBootPartitions()
	{
		return Storage::findPartition(Partition::Type::app);
	}

	// utility functions

	uint8_t getSlot(Partition partition)
	{
		auto s = toString(partition.type(), partition.subType());
		if(!s.startsWith("app/ota")) {
			return 255;
		}

		return s.substring(7).toInt();
	}
};

} // namespace Ota
