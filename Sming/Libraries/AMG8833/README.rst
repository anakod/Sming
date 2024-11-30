AMG8833
=======

.. highlight:: c++

Introduction
------------

A library to communicate with the cheap `thermal sensor AMG8833 <https://mediap.industry.panasonic.eu/assets/imported/industrial.panasonic.com/cdbs/www-data/pdf/ADI8000/ADI8000C66.pdf>`_.

Using
----- 

1. Add ``COMPONENT_DEPENDS += AMG8833`` to your application componenent.mk file.
2. Add these lines to your application::

	#include <Melopero_AMG8833.h>
	
	namespace
	{
	  Melopero_AMG8833 sensor;
	
	  // ...
	
	} // namespace
		
	void init()
	{
		Wire.begin();
		sensor.initI2C();
		
		// ...
	}
