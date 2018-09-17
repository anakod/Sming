/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * @author: 2018 - Mikee47 <mike@sillyhouse.net>
 *
 * Defines the FlashString structure and associated macros for efficient flash memory string access.
 *
 ****/

/*
 * Note on storing strings in flash memory
 *
 * A string literal (e.g. "string") used in code gets emitted to the .rodata segment by the compiler.
 * That means it gets read into RAM at startup and remains there.
 * To avoid this, and reclaim the RAM, the data must be stored in a different segment. This is done
 * using the PROGMEM macro, defined in FakePgmSpace.h.
 *
 * Once in flash memory, string data must be read into RAM before it can be used. Accessing the flash
 * memory directly is awkard. If locations are not strictly accessed as 4-byte words the system will probably
 * crash; I say 'probably' because sometimes it just behaves weirdly if the RAM address isn't aligned.
 *
 * _FakePgmSpace_ provides the basic mechanisms for storing and reading flash strings, including
 * general-purpose string library functions.  These are well-documented Arduino-compatible routines.
 * Some additions have been made to Sming to cater for the ESP8266 use of these strings.
 *
 * 	F(string_literal) - loads a String object with the given text, which is allocated to flash.
 *		String s = F("test");
 *
 *	_F(string_literal) - Like F() except buffer is allocated on stack. Most useful where nul-terminated data is required.
 *		m_printf(_F("C-style string\n"));
 *
 *	DEFINE_PSTR(_name, _str) - declares a PSTR stored in flash. The variable (_name) points to flash
 *	memory so must be accessed using the appropriate xxx_P function.
 *
 *	LOAD_PSTR(_name, _flash_str) - loads pre-defined PSTR into buffer on stack
 *		static DEFINE_PSTR(test, "This is a test string\n"); // Local scope requires static allocation
 *		m_printf(LOAD_PSTR(test));
 *
 *	PSTR_ARRAY(_name, _str) - creates and loads string into named stack buffer
 *	Ensures loaded string stays in scope, unlike _F()
 *		String testfunc() {
 *			PSTR_ARRAY(test, "This is the test string");
 *			m_printf(test);
 *			...
 *      	return test;
 *      }
 *
 *	The length of a string variable defined using DEFINE_PSTR or PSTR_ARRAY can be obtained using sizeof()
 *	because they define a char[] array. Otherwise a call is required to strlen_P(), which is time-consuming.
 *
 *	FlashString
 *
 *		A 'counted string' defined as a C++ structure with a length field.
 *		Requires only a single flash read to get the length; no strlen_P, etc.
 *		Stores arbitrary data including nuls.
 *		A global instance can be shared amongst modules with no loss of efficiency.
 *		The data is word-aligned so reading is as fast as possible; there are no alignment issues to deal with.
 *		Structure-based, so can add methods to the structure instead of using macros.
 *		Strongly typed, so support can be added to other modules for implicit conversions, etc. (e.g. ArduinoJson)
 *		Works with updated String class to provide the simplest string/data management.
 *
 *		Macros are consistent with the PSTR types, but named 'FSTR'.
 *
 *  Notes on usage
 *
 *  	Best practice is usually to define constant data at the top of a module. Non-trivial strings are no different.
 *  	The mechanisms offered here provide a way to do that.
 *
 */

#ifndef __FLASH_STRING_H_
#define __FLASH_STRING_H_

#include "WString.h"

/*
 * Define and use a FlashString inline
 */
#define FSTR(_str)                                                                                                     \
	(__extension__({                                                                                                   \
		static DEFINE_FSTR(__c, _str);                                                                                 \
		__c;                                                                                                           \
	}))

/** @Brief Define a FlashString
 *  @param _name variable to identify the string
 *  @param _str content of the string
 *  @note the whole thing is word-aligned
 *  Example: DEFINE_FSTR(test, "This is a test\0Another test\0hello")
 *  The data includes the nul termiantor but the length does not.
 *  Use DEFINE_FSARR for array data so the length is correct.
 */
#define DEFINE_FSTR(_name, _str)                                                                                       \
	constexpr struct {                                                                                                 \
		FlashString fstr;                                                                                              \
		char data[ALIGNUP(sizeof(_str))];                                                                              \
	} _##_name PROGMEM = {{sizeof(_str) - 1}, _str};                                                                   \
	const FlashString& _name = _##_name.fstr;

// Declare a global reference to a FlashString instance
#define DECLARE_FSTR(_name) extern const FlashString& _name;

/** @Brief Define a FlashString containing array data
 *  @param _name variable to identify the data
 *  @param _arr array data
 *  @note Example: DEFINE_FSARR(test, {1, 2, 3, 4})
 *  Resulting data is not nul-terminated.
 */
#define DEFINE_FSARR(_name, _arr)                                                                                      \
	constexpr struct {                                                                                                 \
		FlashString fstr;                                                                                              \
		char data[ALIGNUP(sizeof(_arr))];                                                                              \
	} _##_name PROGMEM = {{sizeof(_arr)}, _arr};                                                                       \
	const FlashString& _name = _##_name.fstr;

// Get a pointer to the actual FlashString, used when creating tables
#define FSTR_PTR(_struct) &_##_struct.fstr

/*
 * Load a FlashString object into a named local (stack) buffer
 *
 * For example:
 *
 * 	DEFINE_FSTR(globalTest, "This is a testing string")
 * 	...
 * 	LOAD_FSTR(local, globalTest)
 * 	printf("%s, %u characters, buffer is %u bytes\n", local, globalTest.length, sizeof(local));
 *
 * 	@note a nul terminator is added in case string was constructed using DEFINE_FSARR()
 */
#define LOAD_FSTR(_name, _fstr)                                                                                        \
	char _name[(_fstr).size()] __attribute__((aligned(4)));                                                            \
	memcpy_aligned(_name, (_fstr).data(), sizeof(_name));                                                              \
	_name[(_fstr).length()] = '\0';

/*
 * Define a flash string and load it into a named array buffer on the stack.
 * This allows sizeof(_name) to work as if the string were defined thus:
 *
 * 	char _name[] = "text";
 */
#define FSTR_ARRAY(_name, _str)                                                                                        \
	static DEFINE_FSTR(_##_name, _str);                                                                                       \
	LOAD_FSTR(_name, _##_name)

/*
 * Load a FlashString object into a local (stack) buffer for immediate (inline) use.
 * Use with care; if buffer needs to remain in scope longer use LOAD_FSTR().
 *
 * Example:
 *
 * 	DEFINE_FSTR(test, "This is a testing string")
 *  ...
 * 	printf("%s\n", _FS(test));
 */
#define _FS(_fsb)                                                                                                      \
	(__extension__({                                                                                                   \
		char __buf[(_fsb).size()] __attribute__((aligned(4)));                                                         \
		memcpy_aligned(__buf, (_fsb).data(), sizeof(__buf));                                                           \
		__buf;                                                                                                         \
	}))

/** @brief describes a counted string stored in flash memory
 *  @note because the string length is stored there is no need to call strlen_P before reading the
 *  content into RAM. Data is stored word-aligned so it can be read as efficiently as possible.
 */
struct FlashString {
	uint32_t _length; // Only needs to be uint16_t but ensures data is aligned
	char _data[];

	uint32_t length() const
	{
		return _length;
	}

	uint32_t size() const
	{
		return ALIGNUP(_length + 1);
	}

	flash_string_t data() const
	{
		return FPSTR(_data);
	}

	/** @brief Check for equality with a C-string
	 *  @param str
	 *  @retval bool true if strings are identical
	 *  @note loads string into a stack buffer for the comparison, no heap required
	 */
	bool isEqual(const char* str) const
	{
		if(str == _data)
			return true;
		if(!str)
			return false;
		return strcmp(_FS(*this), str) == 0;
	}

	/** @brief Check for equality with another FlashString
	 *  @param str
	 *  @retval bool true if strings are identical
	 */
	bool isEqual(const FlashString& str) const
	{
		if(_length != str._length)
			return false;
		if(_data == str._data)
			return true;
		return memcmp_aligned(_data, str._data, ALIGNUP(_length)) == 0;
	}

	bool isEqual(const String& str) const
	{
		return str.equals(*this);
	}

	bool operator==(const char* str) const
	{
		return isEqual(str);
	}

	bool operator==(const FlashString& str) const
	{
		return isEqual(str);
	}

	bool operator==(const String& str) const
	{
		return isEqual(str);
	}
};

#endif /* __FLASH_STRING_H_ */
