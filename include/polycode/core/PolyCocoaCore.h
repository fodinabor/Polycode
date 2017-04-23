/*
 Copyright (C) 2011 by Ivan Safrin
 
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
#include "polycode/core/PolyString.h"
#include "polycode/core/PolyGlobals.h"
#include "polycode/core/PolyLogger.h"
#include "polycode/core/PolyCore.h"
#include "polycode/core/PolyString.h"
#include "polycode/core/PolyRectangle.h"
#include "polycode/core/PolyPAAudioInterface.h"
#include <vector>
#include <OpenGL/gl.h>
#include <OpenGL/OpenGL.h>
#include "polycode/core/PolyOpenGLGraphicsInterface.h"
#include <mach/mach_time.h>
#include <unistd.h>
#include "polycode/core/PolyInputEvent.h"
#include <IOKit/hid/IOHIDLib.h>

#define POLYCODE_CORE CocoaCore

using std::vector;

@class PolycodeView;
@class NSOpenGLContext;

namespace Polycode {
	
	class CocoaEvent {
	public:
		int eventGroup;
		int eventCode;
		
		int mouseX;
		int mouseY;
		
		std::vector<TouchInfo> touches;
		TouchInfo touch;
		
		PolyKEY keyCode;
        String text;
		
		char mouseButton;
		
		static const int EVENTBASE_PLATFORMEVENT = 0x300;
		static const int INPUT_EVENT = EVENTBASE_PLATFORMEVENT+0;
		static const int FOCUS_EVENT = EVENTBASE_PLATFORMEVENT+1;
	};
	
	
	class HIDGamepadAxis {
		public:
		IOHIDElementCookie cookie;
		CFIndex logicalMin;
		CFIndex logicalMax;
		bool hasNullState;
		bool isHatSwitch;
		bool isHatSwitchSecondAxis;
	};

	class HIDGamepadButton {
		public:
		IOHIDElementCookie cookie;
	};	
	
	class GamepadDeviceEntry  {
		public:
			GamepadDeviceEntry() {
				numAxes = 0;
				numButtons = 0;
			}
			vector<HIDGamepadAxis> axisElements;
			vector<HIDGamepadButton> buttonElements;			
			unsigned int deviceID;
			IOHIDDeviceRef device;
			unsigned int numAxes;
			unsigned int numButtons;	
			CoreInput *input;		
	};
	
	class _PolyExport CocoaCore : public Core {		
	public:
		
		CocoaCore(PolycodeView *view, int xRes, int yRes, bool fullScreen, bool vSync, int aaLevel, int anisotropyLevel, int frameRate, int monitorIndex=-1, bool retinaSupport=false);
		virtual ~CocoaCore();
		
		bool createRenderContext();
		
		void enableMouse(bool newval);
		unsigned int getTicks();
		bool systemUpdate();
		
		void createThread(Threaded *target);		
		
		void createFolder(const String& folderPath);
		void copyDiskItem(const String& itemPath, const String& destItemPath);
		void moveDiskItem(const String& itemPath, const String& destItemPath);
		void removeDiskItem(const String& itemPath);
		String openFolderPicker();
		vector<String> openFilePicker(vector<CoreFileExtension> extensions, bool allowMultiple);
		String saveFilePicker(std::vector<CoreFileExtension> extensions);
		
		String executeExternalCommand(String command, String args, String inDirectory="");
		
		void setVideoMode(int xRes, int yRes, bool fullScreen, bool vSync, int aaLevel, int anisotropyLevel, bool retinaSupport=true);

		bool systemParseFolder(const Polycode::String& pathString, bool showHidden, std::vector<OSFileEntry> &targetVector);
		
		void launchApplicationWithFile(String application, String file);
		void openFileWithApplication(String file, String application);
		
		void setCursor(int cursorType);
		void warpCursor(int x, int y);
		
		void openURL(String url);
		
		void copyStringToClipboard(const String& str);
		String getClipboardString();		
		
		void initGamepad();
		void shutdownGamepad();
		
		void makeApplicationMain();
		
		void checkEvents();		
		
		int lastMouseY;
		int lastMouseX;				
		
		vector<CocoaEvent> cocoaEvents;
		
		NSOpenGLContext *context;
				
		vector<GamepadDeviceEntry*> gamepads;
		unsigned int nextDeviceID;
		
		bool checkSpecialKeyEvents(PolyKEY key);		

		Number getBackingXRes();
		Number getBackingYRes();
		
		void handleVideoModeChange(VideoModeChangeInfo *modeInfo);
		void flushRenderContext();
		
	protected:
	
		PAAudioInterface *audioInterface;
		PolycodeView *glView;
		uint64_t initTime;
		bool retinaSupport;
		
		IOHIDManagerRef hidManager;
	};
}
