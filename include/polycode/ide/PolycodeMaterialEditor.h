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

#include "polycode/ide/PolycodeEditor.h"
#include <Polycode.h>
#include "polycode/modules/ui/PolycodeUI.h"
#include "polycode/ide/PolycodeProps.h"
#include <memory>

using namespace Polycode;


class MaterialBrowserData  {
public:
	MaterialBrowserData() {
		material = NULL;
		shader = NULL;
		cubemap = NULL;
		postMaterial = NULL;
	}
	std::shared_ptr<Material> material;
	std::shared_ptr<Material> postMaterial;
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Cubemap> cubemap;
	String name;
};

class MaterialPreviewBox : public UIElement {
	public:
		MaterialPreviewBox(Core *core, ResourcePool *pool);
		~MaterialPreviewBox();
		void setMaterial(std::shared_ptr<Material> material);		
		void showPrimitive(unsigned int index);
		void clearMaterial();
		void Render(GPUDrawBuffer *buffer);
	
		void handleEvent(Event *event);
		
		Scene *previewScene;
		SceneLight *mainLight;
		SceneLight *secondLight;		
		SceneRenderTexture *renderTexture;
		UIRect *previewShape;
		
		std::vector<UIImageButton*> shapeSwitches;
		std::vector<ScenePrimitive*> shapePrimitives;
		UIImage *shapeSelector;
		
		Entity *previewBase;		
		ScenePrimitive *previewPrimitive;	
		std::shared_ptr<Material> currentMaterial;					
};

class MaterialPreviewProp : public PropProp {
	public:
		MaterialPreviewProp(Core *core, ResourcePool *pool);
		void setPropWidth(Number width);
		MaterialPreviewBox *previewBox;
};

class PostPreviewBox : public UIElement {
	public:
		PostPreviewBox(Core *core, ResourcePool *pool);
		~PostPreviewBox();
		void setMaterial(std::shared_ptr<Material> material);
		void clearMaterial();		
		void handleEvent(Event *event);
		void Resize(Number width, Number height);
		void Update(Number elapsed);	
			
		Scene *previewScene;
		SceneRenderTexture *renderTexture;
		UIRect *previewShape;				
		Entity *previewBase;		
		ScenePrimitive *previewPrimitive;	
		std::shared_ptr<Material> currentMaterial;	
		UIRect *headerBg;
				
		Number spinValue;
		
		UICheckBox *rotateCheckBox;
		UICheckBox *hdrCheckBox;		
		UITextInput *cameraExposureInput;
		UITextInput *lightStrength;
				
		SceneLight	*mainLight;
		SceneLight	*secondLight;

};


class MaterialBrowser : public UIElement {
	public:
		MaterialBrowser(Core *core, ResourcePool *pool);
		~MaterialBrowser();
		
		void Resize(Number width, Number height);
		
		UITree *addMaterial(std::shared_ptr<Material> material);
		UITree *addShader(std::shared_ptr<Shader> shader);
		UITree *addCubemap(std::shared_ptr<Cubemap> cubemap);
		UITree *addPostMaterial(std::shared_ptr<Material> material);
		
		void removeSelected();
		
		void handleEvent(Event *event);
		
		MaterialBrowserData *selectedData;

		UIImageButton *newShaderButton; 
		UIImageButton *newMaterialButton;
		UIImageButton *newCubemapButton;
		UIImageButton *newPostButton;
		
		UIImageButton *removeButton;
								
		UITree *selectedNode;
														
	protected:
	
		UIRect *headerBg;
		UITree *shadersNode;
		UITree *materialsNode;
		UITree *cubemapsNode;
		UITree *postEffectsNode;
			
		UITreeContainer *treeContainer; 
};

class CubemapEditorPane : public UIElement {
	public:
		CubemapEditorPane(Core *core, ResourcePool *resourcePool, ResourcePool *localPool);
		~CubemapEditorPane();
		void Resize(Number width, Number height);
		void setCubemap(std::shared_ptr<Cubemap> cubemap);
		void handleEvent(Event *event);
		std::shared_ptr<Cubemap> currentCubemap;		
		
		void Activate();
		void Deactivate();
	
	protected:
	
		PropList *propList;
		UIRect *headerBg;
		
		TextureProp *yPosTexture;
		TextureProp *yNegTexture;
		TextureProp *xPosTexture;
		TextureProp *xNegTexture;
		TextureProp *zPosTexture;
		TextureProp *zNegTexture;
		
		StringProp *nameProp;
		
		MaterialPreviewBox *cubemapPreview;
};

class PostEditorPane : public UIElement {
	public:
		PostEditorPane(Core *core, ResourcePool *resourcePoolm, ResourcePool *localPool);
		~PostEditorPane();
		void Resize(Number width, Number height);
		void setMaterial(std::shared_ptr<Material> material);
		void handleEvent(Event *event);
		std::shared_ptr<Material> currentMaterial;	
		
		void Activate();
		void Deactivate();
	
		void adjustPreview();
		
		protected:
		
		UIRect *headerBgBottom;

		PropList *propList;				
		PropSheet *baseProps;
		ShaderPassesSheet *passProps;
		RenderTargetsSheet *targetsProps;
	
		PropList *optionsPropList;
		
		TargetBindingsSheet *targetBindingProps;
		ShaderOptionsSheet *shaderOptionsSheet;			
			
		UIVSizer *mainSizer;
		UIElement *topElement;
		UIElement *bottomElement;
						
		StringProp *nameProp;
		BoolProp *fp16Prop;
		
		PostPreviewBox *postPreview;
					
};

class ShaderEditorPane : public UIElement {
	public:
		ShaderEditorPane(Core *core, ResourcePool *resourcePool, ResourcePool *localPool);
		~ShaderEditorPane();
		void Resize(Number width, Number height);
		void setShader(std::shared_ptr<Shader> shader);
		
		void handleEvent(Event *event);
		
		void reloadPrograms();

		std::shared_ptr<Shader> currentShader;
		PolycodeProject *parentProject;
			
	protected:
	
		ResourcePool *localPool;
		bool changingShader;

		bool choosingVertexProgram;
	
		PropList *propList;
		UIRect *headerBg;
		
		ComboProp *vertexProgramProp;
		ComboProp *fragmentProgramProp;
		
		StringProp *nameProp;
		BoolProp *screenShaderProp;
	
		
		NumberProp *pointLightsProp;
		NumberProp *spotLightsProp;		
};


class MaterialEditorPane : public UIElement {
	public:
		MaterialEditorPane(Core *core, ResourcePool *pool, ResourcePool *localPool);
		~MaterialEditorPane();
	
		void Activate();
		void Deactivate();
	
		
		void setMaterial(std::shared_ptr<Material> material);
		void handleEvent(Event *event);
		
		void reloadShaders();
		void Resize(Number width, Number height);	
		
		std::shared_ptr<Material> currentMaterial;			
	protected:
	
		MaterialPreviewBox *materialPreview;
		bool changingMaterial;
	
		UIRect *headerBg;			
		
		PropList *propList;
		
		StringProp *nameProp;
		ComboProp *blendModeProp;
		ComboProp *shaderProp;
		ResourcePool *localPool;
		ShaderOptionsSheet *shaderOptionsSheet;
};

class MaterialMainWindow : public UIElement {
	public:
	MaterialMainWindow(Core *core, ResourcePool *resourcePool, ResourcePool *localResourcePool);
	~MaterialMainWindow();
	
	void Activate();
	void Deactivate();
	
	void Resize(Number width, Number height);
	
	MaterialEditorPane *materialPane;
	ShaderEditorPane *shaderPane;
	CubemapEditorPane *cubemapPane; 
	PostEditorPane *postPane;	
		
	UIColorPicker *colorPicker;
};

class PolycodeMaterialEditor : public PolycodeEditor {
	public:
	PolycodeMaterialEditor(Core *core, ResourcePool *pool);
	virtual ~PolycodeMaterialEditor();
	
	bool openFile(OSFileEntry filePath);
	void Resize(int x, int y);
	
	void Activate();
	void Deactivate();
	
	void handleEvent(Event *event);
	void saveFile();
	void saveMaterials(ObjectEntry *materialsEntry, std::vector<std::shared_ptr<Material> > materials);
	
	static String createStringValue(std::shared_ptr<LocalShaderParam> param);
	
	protected:
	
		ResourcePool *localResourcePool;
		
		MaterialBrowser *materialBrowser;
		UIHSizer *mainSizer;
		
		MaterialMainWindow *mainWindow;
		std::vector<std::shared_ptr<Material> > materials;
		std::vector<std::shared_ptr<Shader> > shaders;
		std::vector<std::shared_ptr<Cubemap> > cubemaps;
		std::vector<std::shared_ptr<Material> > postMaterials;
								
		UITree *selectedMaterialNode;
};

class PolycodeMaterialEditorFactory : public PolycodeEditorFactory {
	public:
		PolycodeMaterialEditorFactory() : PolycodeEditorFactory() { extensions.push_back("mat"); }
		PolycodeEditor *createEditor(Core *core, ResourcePool *pool) { return new PolycodeMaterialEditor(core, pool); }
};
