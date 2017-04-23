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

#include "polycode/ide/PolycodeGlobals.h"
#include "polycode/modules/ui/PolycodeUI.h"
#include "polycode/ide/PolycodeEditor.h"
#include "polycode/ide/PolycodeProject.h"
#include "polycode/ide/PolycodeProjectManager.h"
#include <Polycode.h>

using namespace Polycode;

class ProjectFontEntry : public UIElement {
	public:
		ProjectFontEntry(Core *core, ResourcePool *pool, String fontPath, String fontName);
		~ProjectFontEntry();
		
		void handleEvent(Event *event);
		
		UITextInput *fontNameInput;
		UILabel *fontFileLabel;
		
		UIImageButton *removeButton;
		ResourcePool *pool;
		String fontPath;
};

class PolycodeProjectEditor : public PolycodeEditor {
	public:
	PolycodeProjectEditor(Core *core, ResourcePool *pool, PolycodeProjectManager *projectManager);
	virtual ~PolycodeProjectEditor();
	
	void handleEvent(Event *event);
	
	bool openFile(OSFileEntry filePath);
	void Resize(int x, int y);
	void saveFile();
		
	protected:
	
	bool isLoading;
	
	void refreshFontEntries();
	
	PolycodeProjectManager *projectManager;
	
	UIImage *grid;

	Object configFile;	
	
	std::vector<UICheckBox*> moduleCheckboxes;
	
	UIElement *mainSettingsWindow;
	UIElement *moduleSettingsWindow;
	
	UIRect *headerBg;
	
	UIElement *fontEntryBase;	
	std::vector<ProjectFontEntry*> fontEntries;
				
	UICheckBox *vSyncCheckBox;
	UITextInput *defaultWidthInput;
	UITextInput *defaultHeightInput;	
	UITextInput *framerateInput;	
	UIComboBox *aaLevelComboBox;
	UIComboBox *afLevelComboBox;	
	UIComboBox *texFilteringComboBox;
	UITextInput *entryPointInput;	
	UIColorBox *bgColorBox;
	
	UIButton *addFontButton;
	
	PolycodeProject *associatedProject;
	
};

class PolycodeProjectEditorFactory : public PolycodeEditorFactory {
	public:
		PolycodeProjectEditorFactory(PolycodeProjectManager *projectManager);
		PolycodeEditor *createEditor(Core *core, ResourcePool *pool);
		
	protected:
		PolycodeProjectManager *projectManager; 
};
