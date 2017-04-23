/*
 Copyright (C) 2012 by Ivan Safrin
 
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
#include "polycode/core/PolySceneLabel.h"
#include "polycode/core/PolyScenePrimitive.h"
#include "polycode/core/PolyEntity.h"
#include "polycode/core/PolyFont.h"
#include "polycode/core/PolyConfig.h"
#include "polycode/modules/ui/PolyUIEvent.h"
#include "polycode/modules/ui/PolyUIElement.h"

namespace Polycode {

	class _PolyExport UICheckBox : public UIElement {
		public:
			UICheckBox(Core *core, ResourcePool *pool, String caption, bool checked);
			virtual ~UICheckBox();
		
			void setChecked(bool val);
			void changeCheck();
			bool isChecked();
			void handleEvent(Event *event);
			String getCaptionLabel();
			
		private:
			bool checked;
			UILabel *captionLabel;
			UIImage *buttonImageChecked;
			UIImage *buttonImageUnchecked;			
	};
}