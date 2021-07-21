#include "HttpEventedResource.h"
#include "../HttpServerConnection.h"

HttpEventedResource::HttpEventedResource(HttpResource* resource)
{
	delegate = resource;

	// [ Register the main events with priority 0 ]
	addEvent(EventType::EVENT_URL, [this](HttpServerConnection& connection, const char* at = nullptr, int length = 0) {
		if(delegate->onUrlComplete) {
			auto hasError = delegate->onUrlComplete(connection, *connection.getRequest(), *connection.getResponse());
			if(hasError) {
				return false;
			}
		}

		return true;
	});

	addEvent(EventType::EVENT_HEADERS,
			 [this](HttpServerConnection& connection, const char* at = nullptr, int length = 0) {
				 if(delegate->onHeadersComplete) {
					 auto hasError =
						 delegate->onHeadersComplete(connection, *connection.getRequest(), *connection.getResponse());
					 if(hasError) {
						 return false;
					 }
				 }

				 return true;
			 });

	addEvent(EventType::EVENT_UPGRADE, [this](HttpServerConnection& connection, const char* at = nullptr,
											  int length = 0) {
		if(delegate->onUpgrade) {
			auto hasError = delegate->onUpgrade(connection, *connection.getRequest(), const_cast<char*>(at), length);
			if(hasError) {
				return false;
			}
		}

		return true;
	});

	addEvent(EventType::EVENT_BODY, [this](HttpServerConnection& connection, const char* at = nullptr, int length = 0) {
		if(delegate->onBody) {
			auto hasError = delegate->onBody(connection, *connection.getRequest(), at, length);
			if(hasError) {
				return false;
			}
		}

		return true;
	});

	addEvent(EventType::EVENT_COMPLETE,
			 [this](HttpServerConnection& connection, const char* at = nullptr, int length = 0) {
				 if(delegate->onRequestComplete) {
					 auto hasError =
						 delegate->onRequestComplete(connection, *connection.getRequest(), *connection.getResponse());
					 if(hasError) {
						 return false;
					 }
				 }

				 return true;
			 });

	// [ Register the resource callbacks ]
	onUrlComplete = [this](HttpServerConnection& connection, HttpRequest& request, HttpResponse& response) -> int {
		return runEvent(EventType::EVENT_URL, connection, nullptr, 0);
	};

	onHeadersComplete = [this](HttpServerConnection& connection, HttpRequest& request, HttpResponse& response) -> int {
		return runEvent(EventType::EVENT_HEADERS, connection, nullptr, 0);
	};

	onUpgrade = [this](HttpServerConnection& connection, HttpRequest& request, char* at, int length) -> int {
		return runEvent(EventType::EVENT_UPGRADE, connection, nullptr, 0);
	};

	onBody = [this](HttpServerConnection& connection, HttpRequest& request, const char* at, int length) -> int {
		return runEvent(EventType::EVENT_BODY, connection, at, length);
	};

	onRequestComplete = [this](HttpServerConnection& connection, HttpRequest& request, HttpResponse& response) -> int {
		return runEvent(EventType::EVENT_COMPLETE, connection, nullptr, 0);
	};
}

int HttpEventedResource::runEvent(EventType type, HttpServerConnection& connection, const char* at, int length)
{
	auto request = connection.getRequest();
	if(request->headers[SKIP_HEADER] == "1") {
		// skip this event
		return 0;
	}

	auto list = getEvents(type);
	if(list != nullptr) {
		auto current = list->getHead();
		while(current != nullptr) {
			bool success = current->data(connection, at, length);
			if(!success) {
				request->headers[SKIP_HEADER] = "1";
				break;
			}
			current = current->next;
		}
	}

	return 0;
}

bool HttpEventedResource::addEvent(EventType type, EventCallback callback, int priority)
{
	PriorityList<EventCallback>* list = events[type];
	if(list == nullptr) {
		list = new PriorityList<EventCallback>();
		events[type] = list;
	}

	return list->add(callback, priority);
}
