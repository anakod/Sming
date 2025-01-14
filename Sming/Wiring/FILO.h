/* $Id: FILO.h 1151 2011-06-06 21:13:05Z bhagman $
||
|| @author         Alexander Brevig <abrevig@wiring.org.co>
|| @url            http://wiring.org.co/
|| @contribution   Brett Hagman <bhagman@wiring.org.co>
||
|| @description
|| | A simple FILO / stack class, mostly for primitive types but can be used
|| | with classes if assignment to int is allowed.
|| | This FILO is not dynamic, so be sure to choose an appropriate size for it.
|| |
|| | Wiring Common API
|| #
||
|| @license Please see cores/Common/License.txt.
||
*/

#pragma once

#include "Countable.h"

template <typename T, unsigned rawSize> class FILO : public Countable<T>
{
public:
	const unsigned size; // speculative feature, in case it's needed

	FILO();

	T pop();			  // get next element
	bool push(T element); // add an element
	T peek() const;		  // get the next element without releasing it from the FILO
	void flush();		  // reset to default state

	bool empty() const
	{
		return numberOfElements == 0;
	}

	bool full() const
	{
		return numberOfElements >= rawSize;
	}

	// how many elements are currently in the FILO?
	unsigned int count() const override
	{
		return numberOfElements;
	}

	const T& operator[](unsigned int index) const override
	{
		return raw[index]; /* unsafe */
	}

	T& operator[](unsigned int index) override
	{
		return raw[index]; /* unsafe */
	}

private:
	unsigned numberOfElements;
	unsigned nextIn;
	unsigned nextOut;
	T raw[rawSize];
};

template <typename T, unsigned rawSize> FILO<T, rawSize>::FILO() : size(rawSize)
{
	flush();
}

template <typename T, unsigned rawSize> bool FILO<T, rawSize>::push(T element)
{
	if(count() >= rawSize) {
		return false;
	}
	raw[numberOfElements++] = element;
	return true;
}

template <typename T, unsigned rawSize> T FILO<T, rawSize>::pop()
{
	if(numberOfElements > 0) {
		return raw[--numberOfElements];
	}
	return raw[0];
}

template <typename T, unsigned rawSize> T FILO<T, rawSize>::peek() const
{
	if(numberOfElements > 0) {
		return raw[numberOfElements - 1];
	}
	return raw[0];
}

template <typename T, unsigned rawSize> void FILO<T, rawSize>::flush()
{
	nextIn = nextOut = numberOfElements = 0;
}
