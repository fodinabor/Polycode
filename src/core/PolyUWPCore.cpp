/*
Copyright (C) 2015 by Ivan Safrin

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


#include "polycode/core/PolyUWPCore.h"
#include "polycode/core/PolyBasicFileProvider.h"
#include <ppltasks.h>

using namespace concurrency;
using namespace Polycode;

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Storage;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

UWPCore::UWPCore(PolycodeView *view, int xRes, int yRes, bool fullScreen, bool vSync, int aaLevel, int anisotropyLevel, int frameRate, int monitorIndex, bool retinaSupport) 
	: Core(xRes, yRes, fullScreen, vSync, aaLevel, anisotropyLevel, frameRate, monitorIndex) {

	m_Window = view->window;
	m_eglWindow = view->eglWindow;

	mEglDisplay = view->mEglDisplay;
	mEglContext = view->mEglContext;
	mEglSurface = view->mEglSurface;

	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	pcFreq = double(li.QuadPart) / 1000.0;

	lastMouseY = 0;
	lastMouseX = 0;

	fileProviders.push_back(new BasicFileProvider());

	renderer = new Renderer(this);

	renderer->setBackingResolutionScale(1.0, 1.0);

	graphicsInterface = new OpenGLGraphicsInterface(this);
	renderer->setGraphicsInterface(this, graphicsInterface);
	setVideoMode(xRes, yRes, fullScreen, vSync, aaLevel, anisotropyLevel, retinaSupport);

	getSoundManager()->setAudioInterface(new XAudio2AudioInterface(this));

}

UWPCore::~UWPCore() {

}

void UWPCore::checkEvents() {

	eventMutex.lock();

	UWPEvent event;
	for (int i = 0; i < systemInputEvents.size(); i++) {
		event = systemInputEvents[i];
		switch (event.eventGroup) {
		case UWPEvent::INPUT_EVENT:
			switch (event.eventCode) {
			case InputEvent::EVENT_MOUSEMOVE:
				input->setDeltaPosition(lastMouseX - event.mouseX, lastMouseY - event.mouseY);
				lastMouseX = event.mouseX;
				lastMouseY = event.mouseY;
				input->setMousePosition(event.mouseX, event.mouseY, getTicks());
				break;
			case InputEvent::EVENT_MOUSEDOWN:
				input->mousePosition.x = event.mouseX;
				input->mousePosition.y = event.mouseY;
				input->setMouseButtonState(event.mouseButton, true, getTicks());
				break;
			case InputEvent::EVENT_MOUSEWHEEL_UP:
				input->mouseWheelUp(getTicks());
				break;
			case InputEvent::EVENT_MOUSEWHEEL_DOWN:
				input->mouseWheelDown(getTicks());
				break;
			case InputEvent::EVENT_MOUSEUP:
				input->setMouseButtonState(event.mouseButton, false, getTicks());
				break;
			case InputEvent::EVENT_KEYDOWN:
				if (!checkSpecialKeyEvents(event.keyCode))
					input->setKeyState(event.keyCode, true, getTicks());
				break;
			case InputEvent::EVENT_KEYUP:
				input->setKeyState(event.keyCode, false, getTicks());
				break;
			case InputEvent::EVENT_TOUCHES_BEGAN:
				input->touchesBegan(event.touch, event.touches, getTicks());
				break;
			case InputEvent::EVENT_TOUCHES_ENDED:
				input->touchesEnded(event.touch, event.touches, getTicks());
				break;
			case InputEvent::EVENT_TOUCHES_MOVED:
				input->touchesMoved(event.touch, event.touches, getTicks());
				break;
			}
			break;
		case UWPEvent::SYSTEM_FOCUS_EVENT:
			switch (event.eventCode) {
			case Core::EVENT_LOST_FOCUS:
				loseFocus();
				break;
			case Core::EVENT_GAINED_FOCUS:
				gainFocus();
				break;
			}
			break;
		}
	}

	systemInputEvents.clear();
	eventMutex.unlock();

}

bool UWPCore::systemUpdate() {
	if (!running) {
		return false;
	}
	doSleep();
	updateCore();

	checkEvents();
	return running;
}

void UWPCore::setCursor(int cursorType) {

}

void launchThread(Threaded *target) {
	target->runThread();
	target->scheduledForRemoval = true;
}

void UWPCore::createThread(Threaded * target) {
	Core::createThread(target);
	std::thread *thread = new std::thread(launchThread, target);
	 
}

void UWPCore::copyStringToClipboard(const String& str) {

}

String UWPCore::getClipboardString() {
	return "";
}

void UWPCore::createFolder(const String& folderPath) {

}

void UWPCore::copyDiskItem(const String& itemPath, const String& destItemPath) {

}

void UWPCore::moveDiskItem(const String& itemPath, const String& destItemPath) {

}

void UWPCore::removeDiskItem(const String& itemPath) {

}

String UWPCore::openFolderPicker() {
	return "";
}

std::vector<String> UWPCore::openFilePicker(std::vector<CoreFileExtension> extensions, bool allowMultiple) {
	std::vector<String> ret;
	return ret;
}

String UWPCore::saveFilePicker(std::vector<CoreFileExtension> extensions) {
	return "";
}

void UWPCore::handleVideoModeChange(VideoModeChangeInfo *modeInfo) {
	if (eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext) == EGL_FALSE)
	{
		assert(false);
	}
}

void UWPCore::flushRenderContext() {
	eglSwapBuffers(mEglDisplay, mEglSurface);
}

void UWPCore::openURL(String url) {

}

unsigned int UWPCore::getTicks() {
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return (unsigned int)(li.QuadPart / pcFreq);
	return 0;
}

String UWPCore::executeExternalCommand(String command, String args, String inDirectory) {
	return "";
}


void wtoc(char* Dest, const WCHAR* Source)
{
	int i = 0;
	while (Source[i] != '\0') {
		Dest[i] = (char)Source[i];
		++i;
	}
	Dest[i] = 0;
}
void ctow(WCHAR* Dest, const char* Source)
{
	int i = 0;
	while (Source[i] != '\0') {
		Dest[i] = (WCHAR)Source[i];
		++i;
	}
	Dest[i] = 0;
}


bool UWPCore::systemParseFolder(const Polycode::String& pathString, bool showHidden, std::vector<OSFileEntry> &targetVector) {

	WIN32_FIND_DATA findFileData;

	WCHAR curDir[4096];
	GetCurrentDirectory(4096, curDir);

	WCHAR tmp[4096];
	memset(tmp, 0, sizeof(WCHAR) * 4096);
	ctow(tmp, pathString.c_str());

	WIN32_FILE_ATTRIBUTE_DATA fileData;

	GetFileAttributesEx(tmp, GetFileExInfoStandard, &fileData);

	if (!(fileData.dwFileAttributes != INVALID_FILE_ATTRIBUTES &&
		(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))) {
		return true;
	}

	SetCurrentDirectory(tmp);

	HANDLE hFind = FindFirstFileEx(L"*", FindExInfoStandard, &findFileData, FindExSearchNameMatch, NULL, 0);
	if (hFind == INVALID_HANDLE_VALUE) {
		SetCurrentDirectory(curDir);
		return true;
	}

	do {
		String fname(findFileData.cFileName);

		if ((fname.c_str()[0] != '.' || (fname.c_str()[0] == '.'  && showHidden)) && fname != "..") {
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				targetVector.push_back(OSFileEntry(pathString, fname, OSFileEntry::TYPE_FOLDER));
			}
			else {
				targetVector.push_back(OSFileEntry(pathString, fname, OSFileEntry::TYPE_FILE));
			}
		}
	} while (FindNextFile(hFind, &findFileData));
	FindClose(hFind);
	SetCurrentDirectory(curDir);

	return true; 
}

void UWPCore::handleSystemEvent(UWPEvent systemEvent) {
	eventMutex.lock();
	systemInputEvents.push_back(systemEvent);
	eventMutex.unlock();
}

void Core::getScreenInfo(int *width, int *height, int *hz) {

}

void UWPCore::setDeviceSize(Number x, Number y) {
	deviceWidth = x;
	deviceHeight = y;

	renderer->setBackingResolutionScale(xRes/deviceWidth, yRes/deviceHeight);
}

Number UWPCore::getBackingXRes() {
	return deviceWidth;
}

Number UWPCore::getBackingYRes() {
	return deviceHeight;
}