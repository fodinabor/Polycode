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
#include "polycode/modules/ui/PolyUIEvent.h"
#include "polycode/modules/ui/PolyUITextInput.h"
#include "polycode/modules/ui/PolyUIWindow.h"
#include "polycode/modules/ui/PolyUIHSlider.h"
#include "polycode/modules/ui/PolyUIBox.h"
#include "polycode/modules/ui/PolyUIElement.h"
#include "polycode/core/PolyFont.h"

namespace Polycode {


	class _PolyExport UIColorPicker : public UIWindow {
		public:
			UIColorPicker(Core *core, ResourcePool *pool);
			virtual ~UIColorPicker();
			
			void updateSelectedColor(bool updateTextFields=true, bool updateHue = true, bool updateSV=true);
			void setHue(Number hueNum);			
			void handleEvent(Event *event);
			void setSaturationAndValue(Number S, Number V);
			
			void updateColorFromMainSelector();
			void updateColorFromHueSelector();
			void rebuildFromTextInputs();
			
			void cancelColorListeners();
			
			void Update(Number elapsed);			
			void onClose();
			
			bool isContinuous();
			void setContinuous(bool _continuous);
			
			void setPickerColor(Color newColor);
			Color getSelectedColor();
			
		protected:
		
			Core *core;
			Vector2 lastMainSelectorPosition;
			Number lastHueSelectorPosition;
			
			bool suppressTextChangeEvent;
			bool continuous;
		
			Color selectedColor;
		
			Number colorAlpha;
			
			Number currentH;
			Number currentS;
			Number currentV;
			
			UITextInput *rTextInput;
			UITextInput *gTextInput;
			UITextInput *bTextInput;
			UITextInput *aTextInput;						
									
			UIImage *mainFrame;
			UIImage *mainBg;			
			UIImage *hueFrame;
			UIImage *hueSelector;
			UIImage *mainSelector;	
			
			UIHSlider *alphaSlider;
			
			SceneMesh *mainColorRect;
			
			vector<UILabel *> junkLabels; // Kept only to delete
	};

	class _PolyExport UIColorBox : public UIElement {
		public:
			UIColorBox(Core *core, ResourcePool *resourcePool, UIColorPicker *colorPicker, Color initialColor, Number width, Number height);
			virtual ~UIColorBox();
			
			Color getSelectedColor();
			
			void setBoxColor(Color newColor);
			void showColorPicker();			
			void handleEvent(Event *event);
				
		protected:
		
		
			bool listeningToPicker;
			UIColorPicker *colorPicker;
			
			Color selectedColor;
				
			UIBox *frameImage;
			UIRect *bgImage;
			UIRect *colorShape;			
		
	};
}