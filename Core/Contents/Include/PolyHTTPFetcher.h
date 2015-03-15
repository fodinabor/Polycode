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

#pragma once
#include <string.h>

#include "PolyGlobals.h"
#include "PolyThreaded.h"

#define HTTP_VERSION 			"HTTP/1.1"
#define DEFAULT_USER_AGENT		"Polycode HTTP Fetcher/1.0"
#define DEFAULT_PAGE_BUF_SIZE 	2048

namespace Polycode {
	
	class HTTPFetcherEvent : public Event {
	public:
		HTTPFetcherEvent() { contentSize = 0; errorCode = 0; data = NULL; storedInFile = false; }
		~HTTPFetcherEvent(){}

		char* data;
		int errorCode;

		bool storedInFile;
		unsigned long contentSize;

		static const int EVENTBASE_SOCKETEVENT = 0x500;
		static const int EVENT_HTTP_ERROR = EVENTBASE_SOCKETEVENT + 2;
		static const int EVENT_HTTP_DATA_RECEIVED = EVENTBASE_SOCKETEVENT + 3;
	};

	class HTTPFetcher : public Threaded {
	public:
		HTTPFetcher(String address, bool saveToPath = false, String savePath = "");
		~HTTPFetcher();

		String getData();

		/*
		* Fetches a file given in the param
		* @param pathToFile Path String to the new file to fetch from the same host. Without leading "/"
		*/
		void fetchFile(String pathToFile, bool saveToPath = false, String savePath = "");

		static const int HTTPFETCHER_ERROR_WRONG_SIZE = 0x10F00;

		bool storeInFile;

	private:
        int s;
		String address;
		String bodyReturn;
		String path;
		String host;
		String protocol;
		String savePath;

		bool createSocket();
		void updateThread();
	};
}
