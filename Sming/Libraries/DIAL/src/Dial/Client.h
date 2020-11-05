/**
 * Client.h
 *
 * Copyright 2020 slaff <slaff@attachix.com>
 *
 * This file is part of the Sming DIAL Library
 *
 * This library is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, version 3 or later.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with FlashString.
 * If not, see <https://www.gnu.org/licenses/>.
 *
 ****/

#pragma once

#include <Data/CStringArray.h>
#include <Network/UPnP/ControlPoint.h>
#include "App.h"

using namespace rapidxml;

/** @defgroup   DIAL client
 *  @brief      Provides DIAL client
 *  @ingroup    multimedia SSDP
 *  @{
 */

namespace Dial
{
DECLARE_FSTR(service_urn)

class Client : public UPnP::ControlPoint
{
public:
	using ConnectedCallback = Delegate<void(Client&, const XML::Document& doc, const HttpHeaders& headers)>;

	Client(size_t maxDescriptionSize = 4096) : maxDescriptionSize(maxDescriptionSize)
	{
	}

	/**
	 * @brief Searches for a DIAL device identified by a search type
	 * @param callback will be called once such a device is auto-discovered
	 * @param urn unique identifier of the search type. Default is Dial::service_urn
	 *
	 * @retval true when the connect request can be started
	 */
	virtual bool connect(ConnectedCallback callback, const String& type = nullptr);

	/**
	 * @brief Directly connects to a device's description xml URL.
	 * @param descriptionUrl the full URL where a description XML can be found.
	 * 		  For example: http://192.168.22.222:55000/nrc/ddd.xml";
	 * @param callback will be called once the XML is fetched
	 *
	 * @retval true when the connect request can be started
	 */
	bool connect(const Url& descriptionUrl, ConnectedCallback callback);

	bool formatMessage(SSDP::Message& msg, SSDP::MessageSpec& ms) override;

	void onNotify(SSDP::BasicMessage& msg) override;

	/**
	 * @brief Gets pointer to an application object
	 * @param applicationId the unique application.
	 * 				A list of registered ids can be found here: http://www.dial-multiscreen.org/dial-registry/namespace-database#TOC-Registered-Names
	 * @retval pointer to an application obeject
	 */
	App* getApp(const String& applicationId);

protected:
	static HttpClient http;

	Url getDescriptionUrl()
	{
		return descriptionUrl;
	}

	XML::Node* getNode(HttpConnection& connection, const String& path);

private:
	size_t maxDescriptionSize; // <<< Maximum size of TV XML description that is stored.
	ConnectedCallback onConnected;

	Url descriptionUrl;
	Url applicationUrl;
	String searchType;
	CStringArray uniqueServiceNames;
	ObjectMap<String, App> apps; // <<< list of invoked apps

	int onDescription(HttpConnection& conn, bool success);
};

} // namespace Dial

/** @} */