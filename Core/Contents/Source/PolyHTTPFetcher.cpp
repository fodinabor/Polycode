/*
Copyright (C) 2014 by Joachim Meyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "PolyHTTPFetcher.h"
#include "PolyLogger.h"
#include "PolyCoreServices.h"
#include "PolyCore.h"

using namespace Polycode;

HTTPFetcher::HTTPFetcher(String address) : Threaded() {
	core = CoreServices::getInstance()->getCore();
	eventMutex = core->getEventMutex();

	this->address = address;
	int protocolIndex = address.find_first_of("://");
    if (protocolIndex != 0){
		protocolIndex += strlen("://");
		int pathIndex = address.find_first_of("/", protocolIndex);
		String path = address.substr(pathIndex+1, address.length());
		
		if (pathIndex != 0){
			host = address.substr(protocolIndex, pathIndex - protocolIndex);
		} else {
			host = address.substr(protocolIndex, address.length());
		}
	} else {
		int pathIndex = address.find_first_of("/");
		String path = address.substr(pathIndex+1, address.length());

		if (pathIndex != 0){
			host = address.substr(0, pathIndex);
		} else {
			host = address;
		}
	}

	struct sockaddr_in server;
		
	addrinfo *result = NULL;
	addrinfo hints;

    //Create a socket
#if PLATFORM == PLATFORM_WINDOWS
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        Logger::log("HTTP Fetcher: Could not create socket: %d\n", WSAGetLastError());
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        Logger::log("HTTP Fetcher: Could not create socket: %s\n", strerror(errno));
#endif
	}

    memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	if (getaddrinfo(host.c_str(), address.substr(0, protocolIndex - strlen("://")).c_str(), &hints, &result) != 0) {
#if PLATFORM == PLATFORM_WINDOWS
		Logger::log("HTTP Fetcher: Address resolve error: %d\n", WSAGetLastError());
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        Logger::log("HTTP Fetcher: Address resolve error: %s\n", strerror(errno));
#endif
		return;
	}
	
	server.sin_addr = ((sockaddr_in*)result->ai_addr)->sin_addr;
	server.sin_family = AF_INET;
	server.sin_port = ((sockaddr_in*)result->ai_addr)->sin_port;

	//Connect to remote server
	if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
#if PLATFORM == PLATFORM_WINDOWS
        Logger::log("HTTP Fetcher: connect error code: %d\n", WSAGetLastError());
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        Logger::log("HTTP Fetcher: connect error code: %s\n", strerror(errno));
#endif
		return;
	}

	CoreServices::getInstance()->getCore()->createThread(this);
}

HTTPFetcher::~HTTPFetcher(){
#ifdef _WINDOWS
	closesocket(s);
#else
	close(s);
#endif
}

void HTTPFetcher::updateThread(){
	//Send some data
	String request;
	if (path != "") {
		request = "GET /" + path + " " + String(HTTP_VERSION) + "\r\nHost: " + host + "\r\nUser-Agent: " + DEFAULT_USER_AGENT + "\r\nConnection: close\r\n\r\n";
	} else {
		request = "GET / " + String(HTTP_VERSION) + "\r\nHost: " + host + "\r\nUser-Agent: " + DEFAULT_USER_AGENT + "\r\nConnection: close\r\n\r\n";
	}
	if (send(s, request.c_str(), strlen(request.c_str()), 0) < 0) {
#if PLATFORM == PLATFORM_WINDOWS
        Logger::log("HTTP Fetcher: Send failed: %d\n", WSAGetLastError());
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        Logger::log("HTTP Fetcher: Send failed: %s\n",strerror(errno));
#endif
		return;
	}

	HTTPFetcherEvent *event = new HTTPFetcherEvent();

	char *server_reply = (char*)malloc(DEFAULT_PAGE_BUF_SIZE);
	unsigned long recv_size;
	do {
		//Receive a reply from the server
#if PLATFORM == PLATFORM_WINDOWS
		if ((recv_size = recv(s, server_reply, DEFAULT_PAGE_BUF_SIZE, 0)) == SOCKET_ERROR) {
			Logger::log("HTTP Fetcher: recv failed: %d\n", WSAGetLastError());
#elif PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
		if ((recv_size = recv(s, server_reply, DEFAULT_PAGE_BUF_SIZE, 0)) == -1) {
			Logger::log("HTTP Fetcher: recv failed: %s\n", strerror(errno));
#endif
			return;
		}

		//Add a NULL terminating character to make it a proper string before using it
		server_reply[recv_size] = '\0';
		event->data += String(server_reply);
	} while (recv_size != 0);

	char *charIndex = strstr(const_cast<char*>(event->data.c_str()), "HTTP/");
    if(charIndex == NULL){
        return;
    }
    int i;
	if (sscanf(charIndex, "HTTP/1.1 %d", &i) != 1 || i < 200 || i>299) {
		return;
	}
	charIndex = strstr(const_cast<char*>(event->data.c_str()), "Content-Length:");
	if (charIndex == NULL)
		charIndex = strstr(const_cast<char*>(event->data.c_str()), "Content-length:");
	if (sscanf(charIndex + strlen("content-length: "), "%d", &i) != 1) {
		return;
	}

	charIndex = strstr(const_cast<char*>(event->data.c_str()), "\r\n\r\n") + strlen("\r\n\r\n");

    event->data = charIndex;
	bodyReturn = event->data;
    dispatchEvent(event, HTTPFetcherEvent::EVENT_HTTP_DATA_RECEIVED);
	killThread();
}

void HTTPFetcher::fetchFile(String pathToFile){
	path = pathToFile;
	CoreServices::getInstance()->getCore()->createThread(this);
}

String HTTPFetcher::getData(){
	return this->bodyReturn;
}
