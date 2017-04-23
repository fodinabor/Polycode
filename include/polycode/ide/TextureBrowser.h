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
#include "Polycode.h"
#include "polycode/ide/PolycodeProject.h"
#include <memory>

using namespace Polycode;

class FolderUserData  {
public:
	String folderPath;
	int type;
};

class AssetEntry : public UIElement {
	public:
		AssetEntry(Core *core, ResourcePool *pool, String assetPath, String assetName, String extension, std::shared_ptr<Resource> resource);
		~AssetEntry();
		
		UIRect *imageShape;
		SceneSprite *spritePreview;
		UILabel *nameLabel;
		
		String assetPath;
		std::shared_ptr<Resource> resource;
		UIRect *selectShape;
};

class AssetList : public UIElement {
	public:
		AssetList(Core *core, ResourcePool *pool);
		~AssetList();
		
		void handleEvent(Event *event);
		
		bool hasExtension(String extension);
	
		void clearList();
		
		void showFolder(String folderPath);
		void showResourcePool(ResourcePool *pool, int resourceFilter);
	
		std::shared_ptr<Resource> getSelectedResource();
	
		String selectedPath;
		
		void setExtensions(std::vector<String> extensions);
		
	protected:
	
		std::shared_ptr<Resource> selectedResource;
		UIImageButton *reloadButton;
	
		String currentFolderPath;
	
		ResourcePool *resourcePool;
		AssetEntry *currentEntry;
		std::vector<AssetEntry*> assetEntries;
		
		std::vector<String> extensions;
};

class AssetBrowser : public UIWindow {
	public:
		AssetBrowser(Core *core, ResourcePool *pool);
		~AssetBrowser();
	
		String getSelectedAssetPath();
		String getFullSelectedAssetPath();
			
		void parseFolderIntoTree(UITree *tree, OSFileEntry folder);

		String getTemplatePath();
		String getFileName();
		
		void setExtensions(std::vector<String> extensions);
		
		void setProject(PolycodeProject *project);
	
		void setBrowseMode(unsigned int newBrowseMode);

		void setResourcePools(std::vector<ResourcePool*> pools, int resourceFilter);
		void setResourceFilter(int resourceType);
		void handleEvent(Event *event);
	
		std::shared_ptr<Resource> getSelectedResource();
	
		static const int BROWSE_MODE_FILES = 0;
		static const int BROWSE_MODE_RESOURCES = 1;
	
	protected:
	
		AssetList *assetList;
		UIScrollContainer *listContainer;
	
		PolycodeProject *currentProject;
	
		unsigned int browseMode;
		int resourceFilter;
	
		UIButton *cancelButton;
		UIButton *okButton;
		
		String templatePath;
			
		UITreeContainer *templateContainer; 
		UITree *defaultTemplateTree;
};