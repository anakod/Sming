/*
  IPAddress.h - Base class that provides IPAddress
  Copyright (c) 2011 Adrian McEwen.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef IPAddress_h
#define IPAddress_h

#include <user_config.h>
#include "Printable.h"
#include "WString.h"


#if LWIP_VERSION_MAJOR == 2
#define LWIP_IP_ADDR_T const ip_addr_t
#else
typedef struct ip_addr ip_addr_t;
#define LWIP_IP_ADDR_T ip_addr_t
#endif

// A class to make it easier to handle and pass around IP addresses

class IPAddress : public Printable
{
private:
    ip_addr_t _address = {0}; ///< IPv4 address

	void fromString(const String& address);

public:
    // Constructors
    IPAddress();
    IPAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
    IPAddress(uint32_t address);
    IPAddress(ip_addr address);
#if LWIP_VERSION_MAJOR == 2
    IPAddress(ip_addr_t address);
#endif
    IPAddress(const uint8_t *address);
    IPAddress(const String address);

    // Overloaded cast operator to allow IPAddress objects to be used where a pointer
    // to a four-byte uint8_t array is expected
    operator uint32_t() const { return _address.addr; }
    operator ip_addr() const { return _address; }
    operator ip_addr*() { return &_address; }

    operator char*() { return toString().begin(); }
#if LWIP_VERSION_MAJOR == 2
    operator ip_addr_t*() { return &_address; }
#endif

    bool operator==(const IPAddress& addr) { return _address.addr == addr._address.addr; }
    bool operator==(const uint8_t* addr);

    bool isNull() const { return _address.addr == 0; }
    String toString() const;

    bool compare(const IPAddress& addr, const IPAddress& mask) const
    {
        return ip_addr_netcmp(&_address, &addr._address, &mask._address);
    }

    // Overloaded index operator to allow getting and setting individual octets of the address
    uint8_t operator[](unsigned index) const
    {
        assert(index < sizeof(_address));
        return (index < sizeof(_address)) ? reinterpret_cast<const uint8_t*>(&_address)[index] : 0;
    }

    uint8_t& operator[](unsigned index)
    {
        assert(index < sizeof(_address));
        return reinterpret_cast<uint8_t*>(&_address)[index];
    }

    // Overloaded copy operators to allow initialisation of IPAddress objects from other types
    IPAddress& operator=(const uint8_t *address);
    IPAddress& operator=(uint32_t address);
    IPAddress& operator=(const String address);

    virtual size_t printTo(Print& p) const;
};


#define INADDR_NONE IPAddress()

#endif
