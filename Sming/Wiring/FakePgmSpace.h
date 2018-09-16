/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * Support for reading flash memory the FlashString structure and associated macros for efficient flash memory string access.
 *
 ****/

#ifndef __FAKE_PGMSPACE_H_
#define __FAKE_PGMSPACE_H_

#include "m_printf.h"
#include "c_types.h"

#define PGM_P  const char *

#define PRIPSTR "%s"

typedef void prog_void;
typedef char prog_char;
typedef unsigned char prog_uchar;
typedef int8_t prog_int8_t;
typedef uint8_t prog_uint8_t;
typedef int16_t prog_int16_t;
typedef uint16_t prog_uint16_t;
typedef int32_t prog_int32_t;
typedef uint32_t prog_uint32_t;

/** @brief determines if the given value is aligned to a word (4-byte) boundary */
#define IS_ALIGNED(_x) (((uint32_t)(_x)&3) == 0)

// Align a size up to the nearest word boundary
#define ALIGNUP(_n) (((_n) + 3) & ~3)

// Align a size down to the nearest word boundary
#define ALIGNDOWN(_n) ((_n) & ~3)

#ifdef ICACHE_FLASH

#ifndef PROGMEM
#define PROGMEM __attribute__((aligned(4))) __attribute__((section(".irom.text")))
#endif

/*
 * Define and use a flash string inline
 */
#define PSTR(_str)                                                                                                     \
	(__extension__({                                                                                                   \
		static DEFINE_PSTR(__c, _str);                                                                                 \
		&__c[0];                                                                                                       \
	}))

/*
 * Declare and use a flash string inline.
 * Returns a pointer to a stack-allocated buffer of the precise size required.
 */
#define _F(_str)                                                                                                       \
	(__extension__({                                                                                                   \
		static DEFINE_PSTR(_flash_str, _str);                                                                          \
		LOAD_PSTR(_buf, _flash_str);                                                                                   \
		_buf;                                                                                                          \
	}))

#define pgm_read_byte(addr)                                                                                            \
	(__extension__({                                                                                                   \
		const char* __addr = (const char*)(addr);                                                                      \
		const char __addrOffset = ((unsigned long)__addr & 3);                                                         \
		const unsigned long* __addrAligned = (const unsigned long*)(__addr - __addrOffset);                            \
		(unsigned char)((*__addrAligned) >> (__addrOffset << 3));                                                      \
	}))

#define pgm_read_word(addr)                                                                                            \
	(__extension__({                                                                                                   \
		const char* __addr = (const char*)(addr);                                                                      \
		const char __addrOffset = ((unsigned long)__addr & 2);                                                         \
		const unsigned long* __addrAligned = (const unsigned long*)(__addr - __addrOffset);                            \
		(unsigned short)((*__addrAligned) >> (__addrOffset << 3));                                                     \
	}))

// No translation necessary (provided address is aligned)
#define pgm_read_dword(addr) (*(const unsigned long*)(addr))
#define pgm_read_float(addr) (*(const float*)(addr))

#ifdef __cplusplus
extern "C"
{
#endif
	void* memcpy_aligned(void* dst, const void* src, unsigned len);
	int memcmp_aligned(const void* ptr1, const void* ptr2, unsigned len);
	void *memcpy_P(void *dest, const void *src_P, size_t length);
	size_t strlen_P(const char * src_P);
	char *strcpy_P(char * dest, const char * src_P);
	char *strncpy_P(char * dest, const char * src_P, size_t size);
	int strcmp_P(const char *str1, const char *str2_P);
	int strcasecmp_P(const char* str1, const char* str2_P);
	char* strcat_P(char* dest, const char* src_P);
	char *strstr_P(char *haystack, const char *needle_P);

	#define sprintf_P(s, f_P, ...)                                                                                         \
		(__extension__({                                                                                                   \
			int len_P = strlen_P(f_P);                                                                                     \
			int __result = 0;                                                                                              \
			char* __localF = (char*)malloc(len_P + 1);                                                                     \
			if(__localF) {                                                                                                 \
				strcpy_P(__localF, f_P);                                                                                   \
				__localF[len_P] = '\0';                                                                                    \
			}                                                                                                              \
			__result = m_snprintf(s, len_P, __localF, ##__VA_ARGS__);                                                      \
			free(__localF);                                                                                                \
			__result;                                                                                                      \
		}))

	#define printf_P_heap(f_P, ...)                                                                                        \
		(__extension__({                                                                                                   \
			char* __localF = (char*)malloc(strlen_P(f_P) + 1);                                                             \
			strcpy_P(__localF, f_P);                                                                                       \
			int __result = os_printf_plus(__localF, ##__VA_ARGS__);                                                        \
			free(__localF);                                                                                                \
			__result;                                                                                                      \
		}))

	#define printf_P_stack(f_P, ...)                                                                                       \
		(__extension__({                                                                                                   \
			char __localF[256];                                                                                            \
			strncpy_P(__localF, f_P, sizeof(__localF));                                                                    \
			__localF[sizeof(__localF) - 1] = '\0';                                                                         \
			m_printf(__localF, ##__VA_ARGS__);                                                                             \
		}))

	#define printf_P printf_P_stack
#ifdef __cplusplus
}
#endif

#else /* ICACHE_FLASH */

#define PROGMEM

#define PSTR(_str) (_str)
#define _F(_str) (_str)

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#define pgm_read_float(addr) (*(const float *)(addr))

#define memcpy_aligned(dst, src, len) memcpy(dst, src, len)
#define memcpy_P(dest, src, num) memcpy((dest), (src), (num))
#define strlen_P(a) strlen((a))
#define strcpy_P(dest, src) strcpy((dest), (src))
#define strncpy_P(dest, src, size) strncpy((dest), (src), (size))
#define strcmp_P(a, b) strcmp((a), (b))
#define strcat_P(dest, src) strcat((dest), (src))
#define strstr_P(a, b) strstr((a), (b))
#define sprintf_P(s, f, ...) m_sprintf((s), (f), ##__VA_ARGS__)
#define printf_P(f, ...) m_printf((f), ##__VA_ARGS__)

#endif /* ICACHE_FLASH */

#define pgm_read_byte_near(addr) pgm_read_byte(addr)
#define pgm_read_word_near(addr) pgm_read_word(addr)
#define pgm_read_dword_near(addr) pgm_read_dword(addr)
#define pgm_read_float_near(addr) pgm_read_float(addr)
#define pgm_read_byte_far(addr) pgm_read_byte(addr)
#define pgm_read_word_far(addr) pgm_read_word(addr)
#define pgm_read_dword_far(addr) pgm_read_dword(addr)
#define pgm_read_float_far(addr) pgm_read_float(addr)

/** @brief define a PSTR
 *  @param _name name of string
 *  @param _str the string data
 *  @note may also be used for byte arrays { 1, 2, 3 } etc.
 */
#define DEFINE_PSTR(_name, _str) constexpr char _name[] PROGMEM = _str;

// Declare a global reference to a PSTR instance
#define DECLARE_PSTR(_name) extern constexpr PGM_P _name;

/*
 * Create a local (stack) buffer called _name and load it with flash data.
 * _flash_str is defined locally so the compiler knows its size (length + nul).
 * Size is rounded up to multiple of 4 bytes for fast copy.
 *
 * If defining a string within a function or other local context, must declare static.
 *
 * Example:
 * 	void testfunc() {
 * 		static DEFINE_PSTR(test, "This is a test string\n")
 * 		m_printf(LOAD_PSTR(test));
 * 	}
 *
 */
#define LOAD_PSTR(_name, _flash_str)                                                                                   \
	char _name[ALIGNUP(sizeof(_flash_str))] __attribute__((aligned(4)));                                               \
	memcpy_aligned(_name, _flash_str, sizeof(_name));

/*
 * Define a flash string and load it into a named array buffer on the stack.
 * This allows sizeof(_name) to work as if the string were defined thus:
 *
 * 	char _name[] = "text";
 */
#define PSTR_ARRAY(_name, _str) LOAD_PSTR(_name, PSTR(_str))

#endif /* __FAKE_PGMSPACE_H_ */
