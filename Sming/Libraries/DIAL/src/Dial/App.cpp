#include "App.h"

namespace Dial
{
HttpClient App::http;

bool App::status(ResponseCallback onResponse)
{
	auto request = new HttpRequest(applicationUrl);
	request->method = HTTP_GET;
	request->setResponseStream(new LimitedMemoryStream(maxDescriptionSize));
	if(onResponse != nullptr) {
		request->onRequestComplete([onResponse, this](HttpConnection& connection, bool successful) -> int {
			onResponse(*this, *(connection.getResponse()));

			return 0;
		});
	}

	return http.send(request);
}

bool App::run(ResponseCallback onResponse)
{
	auto request = new HttpRequest(applicationUrl);
	request->method = HTTP_POST;
	request->setResponseStream(new LimitedMemoryStream(maxDescriptionSize));
	request->onRequestComplete([onResponse, this](HttpConnection& connection, bool successful) -> int {
		auto headers = connection.getResponse()->headers;
		if(headers.contains(HTTP_HEADER_LOCATION)) {
			this->instanceUrl = headers[HTTP_HEADER_LOCATION];
		}

		if(onResponse != nullptr) {
			onResponse(*this, *(connection.getResponse()));
		}

		return 0;
	});

	return http.send(request);
}

bool App::run(const String& body, enum MimeType mime, ResponseCallback onResponse)
{
	auto request = new HttpRequest(applicationUrl);
	request->method = HTTP_POST;
	request->headers[HTTP_HEADER_CONTENT_TYPE] = ContentType::toString(mime);
	if(body.length() != 0) {
		request->setBody(body);
	}
	request->setResponseStream(new LimitedMemoryStream(maxDescriptionSize));
	request->onRequestComplete([onResponse, this](HttpConnection& connection, bool successful) -> int {
		auto headers = connection.getResponse()->headers;
		if(headers.contains(HTTP_HEADER_LOCATION)) {
			this->instanceUrl = headers[HTTP_HEADER_LOCATION];
		}

		if(onResponse != nullptr) {
			onResponse(*this, *(connection.getResponse()));
		}

		return 0;
	});

	return http.send(request);
}

bool App::run(const HttpParams& params, ResponseCallback onResponse)
{
	auto request = new HttpRequest(applicationUrl);
	request->method = HTTP_POST;
	request->setPostParameters(params);
	request->setResponseStream(new LimitedMemoryStream(maxDescriptionSize));
	request->onRequestComplete([onResponse, this](HttpConnection& connection, bool successful) -> int {
		auto headers = connection.getResponse()->headers;
		if(headers.contains(HTTP_HEADER_LOCATION)) {
			this->instanceUrl = headers[HTTP_HEADER_LOCATION];
		}

		if(onResponse != nullptr) {
			onResponse(*this, *(connection.getResponse()));
		}

		return 0;
	});

	return http.send(request);
}

bool App::stop(ResponseCallback onResponse)
{
	if(instanceUrl.length() == 0) {
		debug_w("Instance URL not set. Only started apps can be stopped.");
		return false;
	}

	auto request = new HttpRequest(Url(instanceUrl));
	request->method = HTTP_DELETE;
	request->setResponseStream(new LimitedMemoryStream(maxDescriptionSize));
	if(onResponse != nullptr) {
		request->onRequestComplete([onResponse, this](HttpConnection& connection, bool successful) -> int {
			onResponse(*this, *(connection.getResponse()));

			return 0;
		});
	}

	return http.send(request);
}

} // namespace Dial