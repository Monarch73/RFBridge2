// 
// 
// 

#include "WcFnRequestHandler.h"
#include <ESP8266WebServer.h>


 ////WcFnRequestHandler::WcFnRequestHandler(HandlerFunction& fn, const String &uri, HTTPMethod method, char wildcard = '*')
	////	: _fn(fn)
	////	, _uri(uri)
	////	, _method(method)
	////	, _wildcard(wildcard)
	////{
	////	assert(_wildcard != '/');
	////	// verify that the URI is reasonable (only contains wildcard at the beginning/end/whole path segments
	////	for (int i = 0; i < _uri.length(); i++) {
	////		if (_uri[i] == _wildcard) {
	////			if (i != 0 && i != _uri.length() - 1 && (_uri[i - 1] != '/' || _uri[i + 1] != '/')) {
	////				assert(false);
	////			}
	////		}
	////	}
	////}

 
WcFnRequestHandler::WcFnRequestHandler(HandlerFunction &fn, const String &uri, HTTPMethod &method, char wildcard)
	: _fn(fn)
	, _uri(uri)
	, _method(method)
	, _wildcard(wildcard)
 {

 }

bool WcFnRequestHandler::canHandle(HTTPMethod requestMethod, String requestUri)
{
	if (_method != HTTP_ANY && _method != requestMethod) {
		return false;
	}

	String uri = removeSlashes(_uri);
	requestUri = removeSlashes(requestUri);
	String wildcardStr;
	wildcardStr += _wildcard;
	while (1) {
		String uPath = getPathSegment(uri);
		String ruPath = getPathSegment(requestUri);
		if (uPath != ruPath && uPath != wildcardStr) {
			return false;
		}
		uri = removeSlashes(removePathSegment(uri));
		requestUri = removeSlashes(removePathSegment(requestUri));
		if (!uri.length() && !requestUri.length()) {
			return true;
		}
		if (!uri.length() || !requestUri.length()) {
			return false;
		}
	}

	return true;
}

bool WcFnRequestHandler::canUpload(String requestUri)
{
	return false;
}

bool WcFnRequestHandler::handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri)
{
	currentReqUri = requestUri;
	_fn(this, requestUri, requestMethod);
	currentReqUri = "";
	return true;
}

void WcFnRequestHandler::upload(ESP8266WebServer& server, String requestUri, HTTPUpload& upload)
{
}

String WcFnRequestHandler::getWildCard(String requestUri, String wcUri, int n, char wildcard)
{
	wcUri = removeSlashes(wcUri);
	requestUri = removeSlashes(requestUri);
	String wildcardStr;
	wildcardStr += wildcard;
	int i = 0;
	while (1) {
		String uPath = getPathSegment(wcUri);
		String ruPath = getPathSegment(requestUri);
		if (uPath == wildcardStr) {
			if (i == n) {
				return ruPath;
			}
			i++;
		}
		wcUri = removeSlashes(removePathSegment(wcUri));
		requestUri = removeSlashes(removePathSegment(requestUri));
		if (!wcUri.length() && !requestUri.length()) {
			return "";
		}
		if (!wcUri.length() || !requestUri.length()) {
			return "";
		}
	}
	return "";
}


String WcFnRequestHandler::getWildCard(int wcIndex)
{
	return getWildCard(currentReqUri, _uri, wcIndex);
}

String WcFnRequestHandler::getPathSegment(String uri)
{
	// assume slashes removed
	int slash = uri.indexOf("/");
	if (slash == -1) {
		return uri;
	}
	return uri.substring(0, slash);
}

String WcFnRequestHandler::removeSlashes(String uri)
{
	if (uri[0] == '/') {
		uri = uri.substring(1);
	}
	if (uri.length() && uri[uri.length() - 1] == '/') {
		uri = uri.substring(0, uri.length() - 1);
	}
	return uri;
}

String WcFnRequestHandler::removePathSegment(String uri)
{
	// assume slashes removed
	int slash = uri.indexOf("/");
	if (slash == -1) {
		return "";
	}
	return uri.substring(slash);
}