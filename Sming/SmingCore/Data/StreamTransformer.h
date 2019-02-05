/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * @author Slavey Karadzhov <slaff@attachix.com>
 *
 ****/

#ifndef _SMING_CORE_DATA_STREAMTRANSFORMER_H_
#define _SMING_CORE_DATA_STREAMTRANSFORMER_H_

#include "CircularBuffer.h"

/**
 * @brief      Class that can be used to transform streams of data on the fly
 * @ingroup    stream data
 *
 *  @{
 */

/**
 * @brief Callback specification for the stream transformers
 * @note See StreamTransformer::transform() method for details
 */
typedef std::function<size_t(const uint8_t* in, size_t inLength, uint8_t* out, size_t outLength)>
	StreamTransformerCallback;

class StreamTransformer : public IDataSourceStream
{
public:
	StreamTransformer(IDataSourceStream* stream, size_t resultSize = 256, size_t blockSize = 64)
		: sourceStream(stream), resultSize(resultSize), result(new uint8_t[resultSize]), blockSize(blockSize)
	{
	}

	/** @brief Constructor with external callback function
	 *  @deprecated
	 */
	StreamTransformer(IDataSourceStream* stream, const StreamTransformerCallback& callback, size_t resultSize = 256,
					  size_t blockSize = 64) __attribute__((deprecated))
	: transformCallback(callback), sourceStream(stream), resultSize(resultSize), result(new uint8_t[resultSize]),
	  blockSize(blockSize)
	{
	}

	virtual ~StreamTransformer()
	{
		delete[] result;
		delete tempStream;
		delete sourceStream;
	}

	//Use base class documentation
	virtual StreamType getStreamType() const
	{
		return sourceStream->getStreamType();
	}

	/**
	 * @brief Return the total length of the stream
	 * @retval int -1 is returned when the size cannot be determined
	*/
	int available()
	{
		return -1;
	}

	//Use base class documentation
	virtual uint16_t readMemoryBlock(char* data, int bufSize);

	//Use base class documentation
	virtual bool seek(int len);

	//Use base class documentation
	virtual bool isFinished();

	/**
	 * @brief A method that backs up the current state
	 *
	 */
	virtual void saveState(){};

	/**
	 * @brief A method that restores the last backed up state
	 */
	virtual void restoreState(){};

protected:
	/**
	 * @brief Inherited class implements this method to transform a block of data
	 * @param const uint8_t* in source data
	 * @param size_t inLength source data length
	 * @param uint8_t* out output buffer
	 * @param size_t outLength size of output buffer
	 * @retval size_t number of output bytes written
	 * @note Called with `in = nullptr` and `inLength = 0' at end of input stream
	 */
	virtual size_t transform(const uint8_t* in, size_t inLength, uint8_t* out, size_t outLength)
	{
		return (transformCallback == nullptr) ? 0 : transformCallback(in, inLength, out, outLength);
	}

	/** @brief Callback function to perform transformation
	 *  @deprecated The virtual transform() method should be used instead in an inherited class
	 */
	StreamTransformerCallback transformCallback = nullptr;

private:
	IDataSourceStream* sourceStream = nullptr;
	CircularBuffer* tempStream = nullptr;
	uint8_t* result = nullptr;
	size_t resultSize;
	size_t blockSize;
};

/** @} */
#endif /* _SMING_CORE_DATA_STREAMTRANSFORMER_H_ */
