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
#include "polycode/core/PolyGlobals.h"
#include "polycode/core/PolyString.h"
#include "polycode/core/PolyRectangle.h"
#include "polycode/core/PolyVector2.h"
#include "polycode/core/PolyEventDispatcher.h"
#include "polycode/core/PolyCoreInput.h"
#include "polycode/core/PolyConfig.h"
#include "polycode/core/PolyThreaded.h"
#include "polycode/core/PolyQuaternion.h"
#include "polycode/core/PolyCoreFileProvider.h"

long getThreadID();

namespace Polycode {

	class Renderer;
    class ResourceManager;
    class SoundManager;
    class Logger;
	
	class _PolyExport CoreFileExtension : public PolyBase {
	public:
		CoreFileExtension() {}
		CoreFileExtension(String description, String extension) {
			this->extension = extension;
			this->description = description;
		}	
		String extension;
		String description;
	};
	
	class _PolyExport PolycodeViewBase : public PolyBase {
	public:
		PolycodeViewBase() { windowData = NULL; }
		virtual ~PolycodeViewBase(){}
		void *windowData;
		bool resizable;
	};
	
	class _PolyExport TimeInfo {
		public:
			TimeInfo();
			
			int seconds;
			int minutes;
			int hours;
			int month;
			int monthDay;
			int weekDay;
			int year;
			int yearDay;
	};
	
	class VideoModeChangeInfo {
	public:
		int xRes;
		int yRes;
		bool fullScreen;
		bool vSync;
		int aaLevel;
		int anisotropyLevel;
		bool retinaSupport;
	};
	
	class CoreMotionEvent : public Event {
		public:
			CoreMotionEvent();
            Vector3 amount;
    };

	/**
	* The main core of the framework. The core deals with system-level functions, such as window initialization and OS interaction. Each platform has its own implementation of this base class. NOTE: SOME OF THE FUNCTIONALITY IN THE CORE IS NOT FULLY IMPLEMENTED!!
	*/
	class _PolyExport Core : public EventDispatcher {

	public:
	
		/**
		* Constructor.
		* @param xRes Inital horizontal resolution of the renderer.
		* @param yRes Inital vertical resolution of the renderer.		
		* @param fullScreen True to launch in fullscreen, false to launch in window.
		* @param aaLevel Level of anti-aliasing. Possible values are 2,4 and 6.
		* @param frameRate Frame rate that the core will update and render at.
		* @param monitorIndex If fullScreen is true, the monitor index to fullscreen to. Pass -1 to use primary monitor.
		*/			
		Core(int xRes, int yRes, bool fullScreen, bool vSync, int aaLevel, int anisotropyLevel, int frameRate, int monitorIndex);
		virtual ~Core();
		
		bool Update();
		
		bool fixedUpdate();
		virtual bool systemUpdate() = 0;
		
		/**
		* Show or hide cursor.
		* @param newval True to show mouse, false to hide it.
		*/		
		virtual void enableMouse(bool newval);

		/**
		* Capture the mouse.
		*
		* The mouse will be unable to exit the polycode screen.
		* 
		* @param newval True to capture the mouse, false to uncapture it.
		*/		
		virtual void captureMouse(bool newval);
		
		/**
		* Sets the cursor the application is using.
		* @param cursorType Type of cursor to use. Possible values are CURSOR_ARROW, CURSOR_TEXT, CURSOR_POINTER, CURSOR_CROSSHAIR, CURSOR_RESIZE_LEFT_RIGHT, CURSOR_RESIZE_UP_DOWN
		*/
		virtual void setCursor(int cursorType) = 0;
		
		/**
		* Warps the cursor to a specified point in the window.
		* @param x New cursor x position 
		* @param y New cursor y position		
		*/
		virtual void warpCursor(int x, int y) {}
		
		/**
		 * Opens / closes the on-screen keyboard on a mobile device.
		 * @param open True to open false to close
		 */
		virtual void openOnScreenKeyboard(bool open){};
		
		/**
		* Launches a Threaded class into its own thread. See the documentation for Threaded for information on how to crated threaded classes.
		* @param target Target threaded class.
		* @see Threaded
		*/		
		virtual void createThread(Threaded *target);
		
		/**
		* Copies the specified string to system clipboard.
		* @param str String to copy to clipboard.
		*/
		virtual void copyStringToClipboard(const String& str) = 0;
		
		/**
		* Returns the system clipboard as a string.
		* @return String from clipboard.
		*/		
		virtual String getClipboardString() = 0;
		
		
		/**
		* Returns the current average frames per second.
		* @return Current average frames per second.
		*/		
		Number getFPS();
		
		/**
		* Shuts down the core and quits the application.
		*/
		void Shutdown();

		/**
		* Checks if core is in fullscreen mode.
		* @return True if in full screen, false if otherwise.
		*/		
		bool isFullscreen(){ return fullScreen; }
		
		/**
		* Returns the current anti-aliasing level.
		* @return Current anti-aliasing level.
		*/				
		int getAALevel() { return aaLevel; }

		/**
		* Returns the input class. See CoreInput for details in input.
		* @return Input class.
		* @see CoreInput
		*/					
		CoreInput *getInput();
		
		/**
		* Returns current horizontal resolution.
		* @return Current horizontal resolution.
		*/											
		Number getXRes();
		
		/**
		* Returns current vertical resolution.
		* @return Current vertical resolution.
		*/													
		Number getYRes();

		/**
		 * Returns actual current horizontal resolution.
		 * @return Current actual horizontal resolution.
		 */
		virtual Number getBackingXRes() { return getXRes(); }

		/**
		 * Returns actual current vertical resolution.
		 * @return Current actual horizontal resolution.
		 */
		virtual Number getBackingYRes() { return getYRes(); }
				
		/**
		* Provides the current width, height, and refresh rate of the screen.
		* @param width If non-NULL, current screen width will be written here (or 0 if unknown).
		* @param hight If non-NULL, current screen height will be written here (or 0 if unknown).
		* @param hz If non-NULL, current screen refresh rate will be written here (or 0 if unknown).
		*/
		static void getScreenInfo(int *width, int *height, int *hz);

		int getScreenWidth();
		int getScreenHeight();
		
		/**
		* Creates a folder on disk with the specified path.
		* @param folderPath Path to create the folder in.
		*/																			
		virtual void createFolder(const String& folderPath) = 0;
		
		/**
		* Copies a disk item from one path to another
		* @param itemPath Path to the item to copy.
		* @param destItemPath Destination path to copy to.
		*/																					
		virtual void copyDiskItem(const String& itemPath, const String& destItemPath) = 0;
		
		/**
		* Moves a disk item from one path to another
		* @param itemPath Path to the item to move.
		* @param destItemPath Destination path to move to.
		*/																							
		virtual void moveDiskItem(const String& itemPath, const String& destItemPath) = 0;
		
		/**
		* Removes a disk item.
		* @param itemPath Path to the item to remove.
		*/																							
		virtual void removeDiskItem(const String& itemPath) = 0;

		/**
		* Opens a system folder picker and suspends operation.
		* @return The selected path returned from the picker.
		*/
		virtual String openFolderPicker() = 0;

		void setFramerate(int frameRate, int maxFixedCycles = 8);
		
		/**
		* Opens a system file picker for the specified extensions.
		* @param extensions An STL vector containing the allowed file extensions that can be selected.
		* @param allowMultiple If set to true, the picker can select multiple files.
		* @return An STL vector of the selected file paths.
		*/																							
		virtual std::vector<String> openFilePicker(std::vector<CoreFileExtension> extensions, bool allowMultiple) = 0;
		
		virtual String saveFilePicker(std::vector<CoreFileExtension> extensions) = 0;

		virtual void handleVideoModeChange(VideoModeChangeInfo *modeInfo) = 0;
		virtual void flushRenderContext() = 0;
		virtual void prepareRenderContext() {}		virtual bool isWindowInitialized() {return true;};
		CoreFile *openFile(const Polycode::String& fileName, const Polycode::String& opts);
		void closeFile(CoreFile *file);
		
		virtual void addFileSource(const String &type, const String &source);
		void removeFileSource(const String &type, const String &source);
		
		std::vector<OSFileEntry> parseFolder(const Polycode::String& pathString, bool showHidden);

		virtual bool systemParseFolder(const Polycode::String& pathString, bool showHidden, std::vector<OSFileEntry> &targetVector) = 0;
		
		virtual String getResourcePathForFile(const String &fileName);
		
		/**
		* Sets a new video mode.
		* @param xRes New horizontal resolution of the renderer.
		* @param yRes New vertical resolution of the renderer.		
		* @param fullScreen True to launch in fullscreen, false to launch in window.
		* @param aaLevel Level of anti-aliasing. Possible values are 2,4 and 6.
		*/																									
		virtual void setVideoMode(int xRes, int yRes, bool fullScreen, bool vSync, int aaLevel, int anisotropyLevel, bool retinaSupport=true);
		
		/**
		* Resizes the renderer.
		* @param xRes New horizontal resolution of the renderer.
		* @param yRes New vertical resolution of the renderer.		
		*/
		virtual void resizeTo(int xRes, int yRes);
		
		void doSleep();
		
		/**
		* Launches the default browser and directs it to specified URL
		* @param url URL to launch.
		*/
		virtual void openURL(String url) = 0;
		
		/**
		* Returns the time elapsed since last frame.
		* @return Time elapsed since last frame in floating point microseconds.
		*/
		Number getElapsed();	
		
		/**
		* Returns the total ticks elapsed since launch.
		* @return Time elapsed since launch in milliseconds
		*/						
		virtual unsigned int getTicks() = 0;

		/** Returns the target number of milliseconds between frames */
		long getRefreshIntervalMs() const {
			return refreshInterval;
		}

		long getTimeSleptMs() const {
			return timeSleptMs;
		}
		
		Number getFixedTimestep();
		
		Renderer *getRenderer();
		Polycode::Rectangle getViewport();
		
		/**
		* Returns the total ticks elapsed since launch.
		* @return Time elapsed since launch in floating point seconds.
		*/		
		double getTicksFloat();
		
		void setUserPointer(void *ptr) { userPointer = ptr; }
		void *getUserPointer() { return userPointer; }
		
		static const int EVENTBASE_CORE = 0x200;
		static const int EVENT_CORE_RESIZE = EVENTBASE_CORE+0;
		static const int EVENT_LOST_FOCUS = EVENTBASE_CORE+1;
		static const int EVENT_GAINED_FOCUS = EVENTBASE_CORE+2;

		static const int EVENT_UNDO = EVENTBASE_CORE+3;
		static const int EVENT_REDO = EVENTBASE_CORE+4;
		static const int EVENT_COPY = EVENTBASE_CORE+5;
		static const int EVENT_CUT = EVENTBASE_CORE+6;
		static const int EVENT_SELECT_ALL = EVENTBASE_CORE+7;
		static const int EVENT_PASTE = EVENTBASE_CORE+8;
		
		static const int EVENT_GYRO_ROTATION = EVENTBASE_CORE+9;
		static const int EVENT_ACCELEROMETER_MOTION = EVENTBASE_CORE+10;
		
		virtual String executeExternalCommand(String command, String args, String inDirectory) = 0;
		
		/**
		* Returns the default working path of the application.
		*/
		String getDefaultWorkingDirectory();
		
		/**
		* Returns the default working path of the application.
		*/
		String getUserHomeDirectory();	
		
		/**
		* Brings the application on top (Cocoa only for now).
		*/
		virtual void makeApplicationMain() {}
		
		ConfigRef getConfig();
		
        ResourceManager *getResourceManager();
        SoundManager *getSoundManager();
        Logger *getLogger();
        
        std::mutex POLYIGNORE eventMutex;
		
		void removeThread(Threaded *thread);
		
		static const int CURSOR_ARROW = 0;
		static const int CURSOR_TEXT = 1;
		static const int CURSOR_POINTER = 2;
		static const int CURSOR_CROSSHAIR = 3;
		static const int CURSOR_RESIZE_LEFT_RIGHT = 4;
		static const int CURSOR_RESIZE_UP_DOWN = 5;
		static const int CURSOR_OPEN_HAND = 6;		
		
		bool paused;
		bool pauseOnLoseFocus;
		
		/**
		* Default width of the desktop screen
		*/
		int defaultScreenWidth;
		
		/**
		* Default height of the desktop screen
		*/		
		int defaultScreenHeight;
		
		Quaternion deviceAttitude;
        static double fixedTimestep;
        
	protected:	
	
        ResourceManager *resourceManager;
        SoundManager *soundManager;
        Logger *logger;
		ConfigRef config;
		
		virtual bool checkSpecialKeyEvents(PolyKEY key) { return false; }
		
		std::vector<CoreFileProvider*> fileProviders;
		
		void loseFocus();
		void gainFocus();
		
		String userHomeDirectory;
		String defaultWorkingDirectory;
		
		void *userPointer;
		
		long refreshInterval;
		unsigned int timeSleptMs;
		
		bool fullScreen;
		int aaLevel;
	
		std::vector<Vector2>	videoModes;
		void updateCore();

		int numVideoModes;
		
		bool running;
		Number fps;
		unsigned int frameTicks;
		unsigned int lastFrameTicks;
		unsigned int lastFPSTicks;
		unsigned int elapsed;
		
		double fixedElapsed;
		double timeLeftOver;
		double maxFixedElapsed;
		
		bool mouseEnabled;
		bool mouseCaptured;
		int anisotropyLevel;
		unsigned int lastSleepFrameTicks;
		
		std::vector<Threaded*> threads;
        std::mutex threadedEventMutex;
		
		int xRes;
		int yRes;
		bool vSync;
		
		bool coreResized;
		
		int monitorIndex;
		
		int frames;
		
		CoreInput *input;
		Renderer *renderer;
	};
	
	class _PolyExport DummyCore : public Core {
	public:
		
		DummyCore();
		~DummyCore();
		
		bool systemUpdate();
		void setCursor(int cursorType);
		void createThread(Threaded *target);
		void copyStringToClipboard(const String& str);
		String getClipboardString();
		void createFolder(const String& folderPath);
		void copyDiskItem(const String& itemPath, const String& destItemPath);
		void moveDiskItem(const String& itemPath, const String& destItemPath);
		void removeDiskItem(const String& itemPath);
		String openFolderPicker();
		std::vector<String> openFilePicker(std::vector<CoreFileExtension> extensions, bool allowMultiple);
		String saveFilePicker(std::vector<CoreFileExtension> extensions);
		void handleVideoModeChange(VideoModeChangeInfo *modeInfo);
		void flushRenderContext();
		void openURL(String url);
		unsigned int getTicks();
		String executeExternalCommand(String command, String args, String inDirectory);
		bool systemParseFolder(const Polycode::String& pathString, bool showHidden, std::vector<OSFileEntry> &targetVector);
		
		
		
	};
	
	
	
}
