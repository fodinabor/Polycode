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

#include "polycode/modules/ui/PolyUIMenu.h"
#include "polycode/modules/ui/PolyUIEvent.h"
#include "polycode/core/PolyInputEvent.h"
#include "polycode/core/PolyLabel.h"
#include "polycode/core/PolyCore.h"
#include "polycode/core/PolyConfig.h"
#include "polycode/core/PolySceneLine.h"
#include "polycode/core/PolyRenderer.h"

using namespace Polycode;

UIMenuItem::UIMenuItem(Core *core, ResourcePool *pool, String label, String _id, void *data, Number comboWidth, Number comboHeight) : UIElement(core) {
	
	this->label = label;
	ConfigRef conf = core->getConfig();
	
	String fontName = conf->getStringValue("Polycode", "uiMenuFont");
	int fontSize = conf->getNumericValue("Polycode", "uiMenuFontSize");
	
	Number paddingX = conf->getNumericValue("Polycode", "uiMenuTextOffsetX");
	Number paddingY = conf->getNumericValue("Polycode", "uiMenuTextOffsetY");
	
	itemLabel = new SceneLabel(pool->getMaterial("Unlit"), label, fontSize, pool->getFont(fontName));
	itemLabel->setBlendingMode(Renderer::BLEND_MODE_NORMAL);
	itemLabel->setPosition(paddingX, floor(((comboHeight/2.0) - itemLabel->getHeight()/2.0) + paddingY));
	addChild(itemLabel);
	itemLabel->color.setColor(0.0, 0.0, 0.0, 1.0);
	this->_id = _id;
	this->data = data;
}

String UIMenuItem::getMenuItemID() {
	return _id;
}

UIMenuItem::UIMenuItem(Core *core) : UIElement(core), data(NULL), itemLabel(NULL) {
}

bool UIMenuItem::isSelectable()
{
	return true;
}

UIMenuItem::~UIMenuItem() {
	if(itemLabel)
		delete itemLabel;
}

UIMenuDivider::UIMenuDivider(Core *core, ResourcePool *pool, Number comboWidth, Number comboHeight) : UIMenuItem(core) {
	ConfigRef conf = core->getConfig();
	Number paddingX = conf->getNumericValue("Polycode", "uiMenuSelectorPadding");
	
	line = new SceneLine(Vector3(paddingX, comboHeight/2.0, 0.0), Vector3(comboWidth-paddingX, comboHeight/2.0, 0.0));
	
	//	line->setLineWidth(1.0);
	line->setColor(Color(0.0, 0.0, 0.0, 0.7));
	addChild(line);
}

UIMenuDivider::~UIMenuDivider() {
	delete line;
}

bool UIMenuDivider::isSelectable()
{
	return false;
}

UIMenu::UIMenu(Core *core, ResourcePool *pool, Number menuWidth) : UIElement(core), resourcePool(pool)
{
	ConfigRef conf = core->getConfig();
	
	this->menuItemHeight = conf->getNumericValue("Polycode", "uiMenuItemHeight");
	this->menuWidth = menuWidth;
	nextItemHeight = 0;
	
	paddingX = conf->getNumericValue("Polycode", "uiMenuPaddingX");
	paddingY = conf->getNumericValue("Polycode", "uiMenuPaddingY");
	
	String dropdownBgImage = conf->getStringValue("Polycode", "uiMenuBgImage");
	
	Number st = conf->getNumericValue("Polycode", "uiMenuBgT");
	Number sr = conf->getNumericValue("Polycode", "uiMenuBgR");
	Number sb = conf->getNumericValue("Polycode", "uiMenuBgB");
	Number sl = conf->getNumericValue("Polycode", "uiMenuBgL");
	
	dropDownBox = new UIBox(core, pool, dropdownBgImage, st,sr,sb,sl, menuWidth, menuItemHeight);
	dropDownBox->setPosition(0,0);
	addChild(dropDownBox);
	
	String selectorBgImage = conf->getStringValue("Polycode", "uiMenuSelectorBgImage");
	
	st = conf->getNumericValue("Polycode", "uiMenuSelectorBgT");
	sr = conf->getNumericValue("Polycode", "uiMenuSelectorBgR");
	sb = conf->getNumericValue("Polycode", "uiMenuSelectorBgB");
	sl = conf->getNumericValue("Polycode", "uiMenuSelectorBgL");
	
	dropDownBox->blockMouseInput = true;
	
	selectorPadding = conf->getNumericValue("Polycode", "uiMenuSelectorPadding");
	
	selectorBox = new UIBox(core, pool, selectorBgImage, st,sr,sb,sl, menuWidth - (paddingX * 2.0), menuItemHeight + (selectorPadding * 2.0));
	dropDownBox->addChild(selectorBox);
	selectorBox->blockMouseInput = true;
	
	selectorBox->visible = false;
	
	selectedOffset = 0;
	dropDownBox->addEventListener(this, InputEvent::EVENT_MOUSEMOVE);
	dropDownBox->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
	dropDownBox->addEventListener(this, InputEvent::EVENT_MOUSEOUT);
	dropDownBox->processInputEvents = true;
	
	core->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
	core->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEUP);
	core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
	core->getInput()->getMousePosition();
	
	setWidth(menuWidth);
	setHeight(menuItemHeight);
	
	// ugh, hackz
	ignoreMouse = true;
}

UIMenuItem *UIMenu::getSelectedItem() {
	
	if(selectedOffset < 0) {
		return items[0];
	}
	
	if(selectedOffset > items.size()-1) {
		return items[items.size()-1];
	}
	
	
	if(selectedOffset >= 0 && selectedOffset < items.size()) {
		return items[selectedOffset];
	} else {
		return NULL;
	}
}

void UIMenu::Update(Number elapsed) {
	ignoreMouse = false;
}

void UIMenu::fitToScreenVertical() {
	// Make sure the entity doesn't go past the bottom of the screen.
	if(dropDownBox->getHeight() < core->getYRes()) {
		// If the entity is as high as the screen, no point trying to fit it in vertically.
		Vector2 screenPos = this->getScreenPositionForMainCamera();
		Number exceedScreenBottom = screenPos.y + dropDownBox->getHeight() - core->getYRes();
		if(exceedScreenBottom > 0) {
			this->setPosition(this->getPosition().x, this->getPosition().y - exceedScreenBottom);
		} else if(screenPos.y < 0) {
			this->setPosition(this->getPosition().x, 0);
		}
	}
}


void UIMenu::handleEvent(Event *event) {
	
	if(event->getDispatcher() == core->getInput()) {
		
		InputEvent *inputEvent = (InputEvent*) event;
		
		if(event->getEventCode() == InputEvent::EVENT_KEYDOWN) {
			if(inputEvent->key == KEY_ESCAPE) {
				dispatchEvent(new UIEvent(), UIEvent::CANCEL_EVENT);
			}
		}
		if((event->getEventCode() == InputEvent::EVENT_MOUSEDOWN || (event->getEventCode() == InputEvent::EVENT_MOUSEUP && initialMouse != inputEvent->getMousePosition())) && !ignoreMouse) {
			if(selectorBox->visible) {
				dispatchEvent(new UIEvent(), UIEvent::OK_EVENT);
			} else {
				dispatchEvent(new UIEvent(), UIEvent::CANCEL_EVENT);
			}
		}
	}
	
	if(event->getDispatcher() == dropDownBox) {
		switch(event->getEventCode()) {
			case InputEvent::EVENT_MOUSEOUT:
			{
				selectorBox->visible = false;
			}
				break;
			case InputEvent::EVENT_MOUSEMOVE:
			{
				core->setCursor(Core::CURSOR_ARROW);
				InputEvent *inputEvent = (InputEvent*) event;
				selectedOffset = floor(((inputEvent->getMousePosition().y-selectorPadding)-paddingY)/menuItemHeight);
				
				if(selectedOffset >= 0 && selectedOffset < items.size() && items[selectedOffset]->isSelectable()) {
					selectorBox->visible = true;
					selectorBox->setPosition(paddingX,paddingY+(selectedOffset*menuItemHeight) - selectorPadding);
				} else {
					selectorBox->visible = false;
				}
			}
				break;
		}
	}
}


UIMenu::~UIMenu() {
	core->getInput()->removeAllHandlersForListener(this);
	dropDownBox->ownsChildren = true;
	if(!ownsChildren) {
		delete dropDownBox;
	}
}

UIMenuItem *UIMenu::addOption(String label, String _id, void *data) {
	UIMenuItem *newItem = new UIMenuItem(core, resourcePool, label, _id, data, menuWidth, menuItemHeight);
	items.push_back(newItem);
	dropDownBox->addChild(newItem);
	newItem->setPosition(0,paddingY+nextItemHeight);
	nextItemHeight += menuItemHeight;
	dropDownBox->resizeBox(menuWidth, nextItemHeight + (paddingY * 2.0));
	
	Number difference = core->getYRes() - (getPosition().y + dropDownBox->getHeight());
	if(difference < 0) {
		setPositionY(getPosition().y + difference);
	}
	difference = core->getXRes() - (getPosition().x + dropDownBox->getWidth());
	if(difference < 0) {
		setPositionX(getPosition().x + difference);
	}
	
	return newItem;
}

UIMenuItem *UIMenu::addDivider()
{
	Number newItemHeight = menuItemHeight;
	UIMenuItem *newItem = new UIMenuDivider(core, resourcePool, menuWidth, newItemHeight);
	items.push_back(newItem);
	dropDownBox->addChild(newItem);
	newItem->setPosition(0, paddingY+nextItemHeight);
	nextItemHeight += newItemHeight;
	dropDownBox->resizeBox(menuWidth, nextItemHeight + (paddingY*2.0));
	return newItem;
}

void UIMenu::Resize(Number width, Number height) {
	UIElement::Resize(width, height);
}

UIGlobalMenu::UIGlobalMenu(Core *core, ResourcePool *pool) : UIElement(core), resourcePool(pool) {
	currentMenu = NULL;
	processInputEvents = true;
	willHideMenu = false;
	defaultMenuWidth = 100;
}

UIGlobalMenu::~UIGlobalMenu() {
	
}

void UIGlobalMenu::hideMenu() {
	removeChild(currentMenu);
	delete currentMenu;
	currentMenu = NULL;
	willHideMenu = false;
}

void UIGlobalMenu::Update(Number elapsed) {
	if(willHideMenu) {
		hideMenu();
	}
}

void UIGlobalMenu::handleEvent(Event *event) {
	if(event->getDispatcher() == currentMenu && event->getEventType() == "UIEvent") {
		if(event->getEventCode() == UIEvent::OK_EVENT) {
			willHideMenu = true;
		}
		
		if(event->getEventCode() == UIEvent::CANCEL_EVENT) {
			willHideMenu = true;
		}
		
	}
}

UIMenu *UIGlobalMenu::showMenuAtMouse(Number width) {
	Vector2 pos = core->getInput()->getMousePosition();
	return showMenu(pos.x, pos.y, width);
}

UIMenu *UIGlobalMenu::showMenu(Number x, Number y, Number width) {
	if(currentMenu) {
		hideMenu();
	}
	currentMenu = new UIMenu(core, resourcePool, width);
	
	currentMenu->addEventListener(this, UIEvent::OK_EVENT);
	currentMenu->addEventListener(this, UIEvent::CANCEL_EVENT);
	
	addChild(currentMenu);
	currentMenu->setPosition(x,y);
	
	return currentMenu;
	
}