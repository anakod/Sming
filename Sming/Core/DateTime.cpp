/*
  DateTime.cpp - Arduino Date and Time library
  Copyright (c) Michael Margolis.  All right reserved.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "DateTime.h"
#include "Data/CStringArray.h"

#ifdef ARCH_ESP32
#include <esp_idf_version.h>
#endif

#if defined(__WIN32) || (defined(ARCH_ESP32) && ESP_IDF_VERSION_MAJOR < 5)
static_assert(sizeof(time_t) != 8, "Great! Now supports 64-bit - please update code");
#warning "**Y2038** time_t is only 32-bits in this build configuration"
#else
static_assert(sizeof(time_t) == 8, "Expecting 64-bit time_t");
#endif

namespace
{
DEFINE_FSTR(flashMonthNames, LOCALE_MONTH_NAMES);
DEFINE_FSTR(flashDayNames, LOCALE_DAY_NAMES);

/* We can more efficiently compare text of 4 character length by comparing as words */
union FourDigitName {
	char c[4];
	uint32_t value;

	// Compare without case-sensitivity
	bool operator==(const FourDigitName& name) const
	{
		constexpr uint32_t caseMask{~0x20202020U};
		return ((value ^ name.value) & caseMask) == 0;
	}
};

const FourDigitName isoDayNames[7] PROGMEM = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const FourDigitName isoMonthNames[12] PROGMEM = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
												 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

/*
 * @brief Match a day or month name against a list of values and set the required value
 * @param ptr Current string position
 * @param value On success, contains index of matched string
 * @isoNames Array of values
 * @nameCount Number of names in array
 * @retval bool false on failure
 *
 * Names are compared without case-sensitivity
 */
bool matchName(const char* ptr, uint8_t& value, const FourDigitName isoNames[], unsigned nameCount)
{
	FourDigitName name{ptr[0], ptr[1], ptr[2]};
	for(unsigned i = 0; i < nameCount; ++i) {
		if(isoNames[i] == name) {
			value = i;
			return true;
		}
	}
	return false;
}

bool isLeapCentury(uint16_t year)
{
	return year % 100 != 0 || year % 400 == 0;
}

bool isLeapYear(uint16_t year)
{
	return year % 4 == 0 && isLeapCentury(year);
}

/** @brief Get the number of days in a month, taking leap years into account
 *  @param month 0=jan
 *  @param year
 *  @retval uint8_t number of days in the month
 */
uint8_t getMonthDays(uint8_t month, uint16_t year)
{
	if(month == dtFebruary) {
		return isLeapYear(year) ? 29 : 28;
	}
	const uint32_t monthDays = 0b101011010101;
	return 30 + ((monthDays >> month) & 0x01);
}

} // namespace

//******************************************************************************
//* DateTime Public Methods
//******************************************************************************

void DateTime::setTime(time_t time)
{
	fromUnixTime(time, &Second, &Minute, &Hour, &Day, &DayofWeek, &Month, &Year);
	Milliseconds = 0;
	calcDayOfYear();
}

bool DateTime::isNull()
{
	return Second == 0 && Minute == 0 && Hour == 0 && Day == 0 && Month == 0 && Year == 0 && DayofWeek == 0 &&
		   Milliseconds == 0;
}

bool DateTime::fromHttpDate(const String& httpDate)
{
	int first = httpDate.indexOf(',');
	if(first < 0 || httpDate.length() - first < 20) {
		return false;
	}
	first += 2; // Skip ", "
	auto ptr = httpDate.c_str();

	// Parse and return a decimal number and update ptr to the first non-numeric character after it
	auto parseNumber = [&ptr]() { return strtol(ptr, const_cast<char**>(&ptr), 10); };

	if(!matchName(ptr, DayofWeek, isoDayNames, 7)) {
		return false; // Invalid day of week
	}

	ptr += first;

	Day = parseNumber();
	if(*ptr == '\0') {
		return false;
	}
	ptr++;

	// Decode the month name
	if(!matchName(ptr, Month, isoMonthNames, 12)) {
		return false; // Invalid month
	}
	ptr += 4; // Skip space as well as month

	Year = parseNumber();
	if(*ptr == '\0') {
		return false;
	}

	if(Year < 70) {
		Year += 2000;
	} else if(Year < 100) {
		Year += 1900;
	}

	Hour = parseNumber();
	if(*ptr != ':') {
		return false;
	}

	ptr++;
	Minute = parseNumber();
	if(*ptr != ':') {
		return false;
	}

	ptr++;
	Second = parseNumber();
	Milliseconds = 0;
	calcDayOfYear();

	return true;
}

time_t DateTime::toUnixTime()
{
	return toUnixTime(Second + (Milliseconds / 1000), Minute, Hour, Day, Month, Year);
}

String DateTime::toShortDateString()
{
	return format(_F("%d.%m.%Y"));
}

String DateTime::toShortTimeString(bool includeSeconds)
{
	return format(includeSeconds ? _F("%T") : _F("%r"));
}

String DateTime::toFullDateTimeString()
{
	return format(_F("%x %T"));
}

String DateTime::toISO8601()
{
	return format(_F("%FT%TZ"));
}

String DateTime::toHTTPDate()
{
	return format(_F("%a, %d %b %Y %T GMT"));
}

void DateTime::addMilliseconds(long add)
{
	int ms = Milliseconds;
	ms += add;
	time_t cur = toUnixTime();
	cur += (ms / 1000);
	ms = ms % 1000;
	setTime(cur);
	Milliseconds = ms;
}

void DateTime::fromUnixTime(time_t timep, uint8_t* psec, uint8_t* pmin, uint8_t* phour, uint8_t* pday, uint8_t* pwday,
							uint8_t* pmonth, uint16_t* pyear)
{
	// convert the given time_t to time components
	// this is a more compact version of the C library localtime function

	unsigned long epoch = timep;
	if(psec != nullptr) {
		*psec = epoch % 60;
	}
	epoch /= 60; // now it is minutes
	if(pmin != nullptr) {
		*pmin = epoch % 60;
	}
	epoch /= 60; // now it is hours
	if(phour != nullptr) {
		*phour = epoch % 24;
	}
	epoch /= 24; // now it is days
	if(pwday != nullptr) {
		*pwday = (epoch + 4) % 7;
	}

	unsigned year = 70;
	unsigned long days = 0;
	while((days += (isLeapYear(year) ? 366 : 365)) <= epoch) {
		year++;
	}
	if(pyear != nullptr) {
		*pyear = year + 1900; // *pyear is returned as years from 1900
	}

	days -= isLeapYear(year) ? 366 : 365;
	epoch -= days; // now it is days in this year, starting at 0
	// *pdayofyear=epoch;  // days since jan 1 this year

	uint8_t month;
	for(month = dtJanuary; month <= dtDecember; month++) {
		uint8_t monthDays = getMonthDays(month, year);
		if(epoch >= monthDays) {
			epoch -= monthDays;
		} else {
			break;
		}
	}

	if(pmonth != nullptr) {
		*pmonth = month; // jan is month 0
	}
	if(pday != nullptr) {
		*pday = epoch + 1; // day of month
	}
}

time_t DateTime::toUnixTime(int sec, int min, int hour, int day, uint8_t month, uint16_t year)
{
	if(year < 69) {
		year += 2000;
	}

	// seconds from 1970 till 1 jan 00:00:00 this year
	auto seconds = time_t(year - 1970) * (SECS_PER_DAY * 365);

	// add extra days for leap years
	for(unsigned y = 1972; y < year; y += 4) {
		if(isLeapYear(y)) {
			seconds += SECS_PER_DAY;
		}
	}
	for(unsigned y = 1968; y >= year; y -= 4) {
		if(isLeapYear(y)) {
			seconds -= SECS_PER_DAY;
		}
	}

	// add days for this year
	for(unsigned m = dtJanuary; m < month; ++m) {
		seconds += SECS_PER_DAY * getMonthDays(m, year);
	}

	seconds += time_t(day - 1) * SECS_PER_DAY;
	seconds += time_t(hour) * SECS_PER_HOUR;
	seconds += time_t(min) * SECS_PER_MIN;
	seconds += sec;

	return seconds;
}

String DateTime::format(const char* sFormat)
{
	if(sFormat == nullptr) {
		return nullptr;
	}

	String sReturn;

	char c;
	while((c = *sFormat++) != '\0') {
		if(c != '%') {
			// Normal character
			sReturn += c;
			continue;
		}

		if(*sFormat == '\0') {
			// Ignore % as last character
			break;
		}

		c = *sFormat++;
		switch(c) {
		// Year (not implemented: EY, Oy, Ey, EC, G, g)
		case 'Y': // Full year as a decimal number, e.g. 2018
			sReturn += Year;
			break;
		case 'y': // Year, last 2 digits as a decimal number [00..99]
			sReturn.concat(Year % 100, DEC, 2);
			break;
		case 'C': // Year, first 2 digits as a decimal number [00..99]
			sReturn.concat(Year / 100, DEC, 2);
			break;
		// Month (not implemented: Om)
		case 'b': // Abbreviated month name, e.g. Oct (always English)
		case 'h': // Synonym of b
			sReturn.concat(CStringArray(flashMonthNames)[Month], 3);
			break;
		case 'B': // Full month name, e.g. October (always English)
			sReturn += CStringArray(flashMonthNames)[Month];
			break;
		case 'm': // Month as a decimal number [01..12]
			sReturn.concat(Month + 1, DEC, 2);
			break;
		// Week (not implemented: OU, OW, OV)
		case 'U': // Week of the year as a decimal number (Sunday is the first day of the week) [00..53]
			sReturn.concat(calcWeek(0), DEC, 2);
			break;
		case 'V': // ISO 8601 week number (01-53)
			// !@todo Calculation of ISO 8601 week number is crude and frankly wrong but does anyone care?
			sReturn.concat(calcWeek(1) + 1, DEC, 2);
			break;
		case 'W': // Week of the year as a decimal number (Monday is the first day of the week) [00..53]
			sReturn.concat(calcWeek(1), DEC, 2);
			break;
		case 'x': // Locale preferred date format
			sReturn += format(_F(LOCALE_DATE));
			break;
		case 'X': // Locale preferred time format
			sReturn += format(_F(LOCALE_TIME));
			break;
		// Day of year/month (Not implemented: Od, Oe)
		case 'j': // Day of the year as a decimal number [001..366]
			sReturn.concat(DayofYear, DEC, 3);
			break;
		case 'd': // Day of the month as a decimal number [01..31]
			sReturn.concat(Day, DEC, 2);
			break;
		case 'e': // Day of the month as a decimal number [ 1,31]
			sReturn.concat(Day, DEC, 2, ' ');
			break;
		// Day of week (Not implemented: Ow, Ou)
		case 'w': // Weekday as a decimal number with Sunday as 0 [0..6]
			sReturn += char('0' + DayofWeek);
			break;
		case 'a': // Abbreviated weekday name, e.g. Fri
			sReturn.concat(CStringArray(flashDayNames)[DayofWeek], 3);
			break;
		case 'A': // Full weekday name, e.g. Friday
			sReturn += CStringArray(flashDayNames)[DayofWeek];
			break;
		case 'u': // Weekday as a decimal number, where Monday is 1 (ISO 8601 format) [1..7]
			sReturn += (DayofWeek == 0) ? '7' : char('0' + DayofWeek);
			break;
		// Time (not implemented: OH, OI, OM, OS)
		case 'H': // Hour as a decimal number, 24 hour clock [00..23]
			sReturn.concat(Hour, DEC, 2);
			break;
		case 'I': // Hour as a decimal number, 12 hour clock [0..12]
			sReturn.concat(Hour ? ((Hour > 12) ? Hour - 12 : Hour) : 12, DEC, 2);
			break;
		case 'M': // Minute as a decimal number [00..59]
			sReturn.concat(Minute, DEC, 2);
			break;
		case 'S': // Second as a decimal number [00..61]
			sReturn.concat(Second, DEC, 2);
			break;
		// Other (not implemented: Ec, Ex, EX, z, Z)
		case 'c': // Locale preferred date and time format, e.g. Tue Dec 11 08:48:32 2018
			sReturn += format(_F(LOCALE_DATE_TIME));
			break;
		case 'D': // US date (MM/DD/YY)
			sReturn += format(_F("%m/%d/%y"));
			break;
		case 'F': // ISO 8601 date format (YYYY-mm-dd)
			sReturn += format(_F("%Y-%m-%d"));
			break;
		case 'r': // 12-hour clock time (hh:MM:SS AM)
			sReturn += format(_F("%I:%M:%S %p"));
			break;
		case 'R': // Short time (HH:MM)
			sReturn += format(_F("%H:%M"));
			break;
		case 'T': // ISO 8601 time format (HH:MM:SS)
			sReturn += format(_F("%H:%M:%S"));
			break;
		case 'p': // Meridiem [AM,PM]
			sReturn += (Hour < 12) ? "AM" : "PM";
			break;
		case '%': // Literal percent (%). The full conversion specification must be %%
			sReturn += '%';
			break;
		case 'n': // Newline character (\n)
			sReturn += '\n';
			break;
		case 't': // Horizontal tab (\t)
			sReturn += '\t';
			break;
		default: // Silently ignore % and process next character
			--sFormat;
		}
	}
	return sReturn;
}

void DateTime::calcDayOfYear()
{
	DayofYear = 0;
	for(unsigned m = dtJanuary; m < Month; ++m) {
		switch(m) {
		case dtSeptember:
		case dtApril:
		case dtJune:
		case dtNovember:
			DayofYear += 30;
			break;
		case dtFebruary:
			DayofYear += isLeapYear(Year) ? 29 : 28;
			break;
		default:
			DayofYear += 31;
		}
	}
	DayofYear += Day;
}

uint8_t DateTime::calcWeek(uint8_t firstDay)
{
	int16_t startOfWeek = DayofYear - DayofWeek + firstDay;
	int8_t firstDayofWeek = startOfWeek % 7;
	if(firstDayofWeek < 0) {
		firstDayofWeek += 7;
	}
	return (startOfWeek + 7 - firstDayofWeek) / 7;
}
