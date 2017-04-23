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
#include "polycode/core/PolyEventDispatcher.h"
#include "polycode/core/PolyRendererPlatformData.h"

namespace Polycode {

    class Core;
	
	/**
	* Base class for resources. All resources that are managed by the ResourceManager subclass this.
	*/
	class _PolyExport Resource : public EventDispatcher {
		public:
					
			// ----------------------------------------------------------------------------------------------------------------
			/** @name Public members
			*  Available public members
			*/
			//@{
		
			Resource(int type);
			virtual ~Resource();
			
			virtual void reloadResource(Core *core);
			
			const String& getResourceName() const;
			int getResourceType() const;
			void setResourceName(const String& newName);
			void setResourcePath(const String& path);
			const String& getResourcePath() const;

			static const int RESOURCE_TEXTURE = 0;
			static const int RESOURCE_MATERIAL = 1;
			static const int RESOURCE_SHADER = 2;
			static const int RESOURCE_PROGRAM = 3;
			static const int RESOURCE_MESH = 5;
			static const int RESOURCE_CUBEMAP = 6;
			static const int RESOURCE_SPRITE = 7;
			static const int RESOURCE_ENTITY_INSTANCE = 8;
			static const int RESOURCE_FONT = 9;
			static const int RESOURCE_SCRIPT = 10;
		
			bool reloadOnFileModify;
		
			static bool defaultReloadOnFileModify;
		
			size_t resourceFileTime;
						
			//@}
			
			RendererPlatformData platformData;
		
		protected:

			int type;
			String resourcePath;
			String name;
	
					
	};
}
