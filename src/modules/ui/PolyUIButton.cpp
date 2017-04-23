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


#include "polycode/modules/ui/PolyUIButton.h"
#include "polycode/core/PolyConfig.h"
#include "polycode/core/PolyInputEvent.h"
#include "polycode/core/PolyLabel.h"
#include "polycode/core/PolyCore.h"
#include "polycode/core/PolyRenderer.h"

using namespace Polycode;

UIButton::UIButton(Core *core, ResourcePool *pool, String text, Number width, Number height) : UIElement(core) {
	
	ConfigRef conf = core->getConfig();
	
	String fontName = conf->getStringValue("Polycode", "uiDefaultFontName");
	int fontSize = conf->getNumericValue("Polycode", "uiButtonFontSize");
	
	Number st = conf->getNumericValue("Polycode", "uiButtonSkinT");
	Number sr = conf->getNumericValue("Polycode", "uiButtonSkinR");
	Number sb = conf->getNumericValue("Polycode", "uiButtonSkinB");
	Number sl = conf->getNumericValue("Polycode", "uiButtonSkinL");

	labelOffsetX = conf->getNumericValue("Polycode", "uiButtonLabelOffsetX");
	labelOffsetY = conf->getNumericValue("Polycode", "uiButtonLabelOffsetY");
		
	buttonRect = new UIBox(core, pool, conf->getStringValue("Polycode", "uiButtonSkin"),
						   st,sr,sb,sl,
						   width, height);	
	
	buttonRect->blockMouseInput	 = true;
	buttonFocusedRect= new UIBox(core, pool, conf->getStringValue("Polycode", "uiButtonFocusedSkin"),
								 st,sr,sb,sl,
								 width, height);		
	blockMouseInput = true;
	addChild(buttonRect);
	addChild(buttonFocusedRect);
	
	buttonFocusedRect->visible = false;
	
	buttonRect->addEventListener(this, InputEvent::EVENT_MOUSEOVER);
	buttonRect->addEventListener(this, InputEvent::EVENT_MOUSEOUT);
	buttonRect->addEventListener(this, InputEvent::EVENT_MOUSEUP);
	buttonRect->addEventListener(this, InputEvent::EVENT_MOUSEUP_OUTSIDE);	
	buttonRect->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
	
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
		
	pressedDown = false;
	
	buttonLabel = new SceneLabel(pool->getMaterial("Unlit"), text, fontSize, pool->getFont(fontName), Label::ANTIALIAS_FULL);
	buttonLabel->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	buttonLabel->color.setColorHexFromString(conf->getStringValue("Polycode", "uiButtonFontColor"));
	addChild(buttonLabel);
	labelXPos = floor((width-buttonLabel->getWidth())/2.0f) + labelOffsetX;
	labelYPos = labelOffsetY;
	buttonLabel->setPosition(labelXPos,labelYPos);
	buttonLabel->positionAtBaseline = true;
	buttonLabel->setAnchorPoint(-1.0, -1.0, 0.0);
	setWidth(width);
	setHeight(height);
	focusable = true;
	
	buttonRect->processInputEvents = true;
	
}

void UIButton::setCaption(String caption) {
	buttonLabel->setText(caption);
	Resize(getWidth(), getHeight());
}

void UIButton::Resize(Number width, Number height) {
	buttonRect->resizeBox(width, height);
	buttonFocusedRect->resizeBox(width, height);
	setWidth(width);
	setHeight(height);
	matrixDirty = true; 
	
	labelXPos = floor((width-buttonLabel->getWidth())/2.0f) + labelOffsetX;
	buttonLabel->setPosition(labelXPos,labelYPos);
	
	UIElement::Resize(width, height);
}

void UIButton::Update(Number elapsed) {
	if(hasFocus) {
		buttonFocusedRect->visible = true;
		buttonRect->visible = false;
	} else {
		buttonFocusedRect->visible = false;
		buttonRect->visible = true;
	}
}

UIButton::~UIButton() {
	core->getInput()->removeAllHandlersForListener(this);
	if(!ownsChildren) {
		delete buttonRect;
		delete buttonFocusedRect;
		delete buttonLabel;
	}
}
		
void UIButton::handleEvent(Event *event) {

	if(event->getDispatcher() == core->getInput()) {
		switch(event->getEventCode()) {
			case InputEvent::EVENT_KEYDOWN:
				if(hasFocus) {
					if(((InputEvent*)event)->key == KEY_RETURN || ((InputEvent*)event)->key == KEY_SPACE) {
						dispatchEvent(new UIEvent(), UIEvent::CLICK_EVENT);
					}
				}
			break;
		}
	}
	
	if(event->getDispatcher() == buttonRect) {
		switch(event->getEventCode()) {
			case InputEvent::EVENT_MOUSEOVER:
			break;
			case InputEvent::EVENT_MOUSEOUT:
				buttonLabel->setPosition(labelXPos,labelYPos);
				buttonRect->setPosition(0,0);				
				pressedDown = false;
			break;
			case InputEvent::EVENT_MOUSEUP_OUTSIDE:
				buttonLabel->setPosition(labelXPos,labelYPos);
				buttonRect->setPosition(0,0);
			break;			
			case InputEvent::EVENT_MOUSEUP:
				buttonLabel->setPosition(labelXPos,labelYPos);
				buttonRect->setPosition(0,0);
				if(pressedDown) {
					dispatchEvent(new UIEvent(), UIEvent::CLICK_EVENT);
				}
				pressedDown = false;
			break;
			case InputEvent::EVENT_MOUSEDOWN:
				pressedDown = true;
				if(focusParent)
					focusParent->focusChild(this);
				buttonLabel->setPosition(labelXPos+1,labelYPos+1);
				buttonRect->setPosition(1,1);
			break;
		}
	}
	
	UIElement::handleEvent(event);	  
}