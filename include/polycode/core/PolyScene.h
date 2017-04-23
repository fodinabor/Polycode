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
#include "polycode/core/PolyColor.h"
#include "polycode/core/PolyVector3.h"
#include "polycode/core/PolyEntity.h"
#include "polycode/core/PolyCore.h"
#include "polycode/core/PolyEventDispatcher.h"

#include <vector>

class CoreFile;

namespace Polycode {
		
	class Camera;
	class Entity;
	class Mesh;
	class RenderFrame;
	
	typedef std::shared_ptr<Scene> SceneRef;
	
	/**
	* Rendering container. The Scene class is the main container for all rendering in Polycode. Scenes are automatically rendered and need only be instantiated to immediately add themselves to the rendering pipeline. A Scene is created with a camera automatically.
	*/ 
	class _PolyExport Scene : public EventDispatcher {
	public:

        Scene(){}
		/**
		* Default constructor with options. 
		* @param sceneType Type of scene to create. Can be Scene::SCENE_2D, Scene::SCENE_3D or Scene::SCENE_2D_TOPLEFT
		*/		
		Scene(Core *core, int sceneType);
				
		/**
		* Default constructor. Defaults to type Scene::SCENE_3D
		*/
		Scene(Core *core);
		
		virtual ~Scene();
		
		/**
		* Adds a new Entity to the scene
		* @param entity New entity to add.
		*/
		void addEntity(Entity *entity);

		/**
		* Adds a new Entity to the scene
		* @param entity New entity to add.
		*/
		void addChild(Entity *entity);
		
		/**
		* Removes a Entity from the scene
		* @param entity New entity to remove.
		*/		
		virtual void removeEntity(Entity *entity);
		
		/**
		* Returns the scene's default camera.
		* @return The scene's default camera.
		*/				
		Camera *getDefaultCamera();
		
		/**
		* Returns the scene's active camera.
		* @return The scene's active camera.
		*/						
		Camera *getActiveCamera();
		
		/**
		* Sets the scene's active camera.
		* @param camera New camera to set as the active camera.
		*/
		void setActiveCamera(Camera *camera);
		
		/**
		* Enables and disables fog in the scene.
		* @param enable If false, disables lighting in the scene, if true, enables it.		
		*/		
		void enableFog(bool enable);
		
		/**
		* Sets the fog properties for the scene.
		* @param fogMode Fog falloff mode. (Renderer::FOG_LINEAR, Renderer::FOG_EXP, Renderer::FOG_EXP2).
		* @param color Fog color.
		* @param density Fog density.
		* @param startDepth Starting depth of the fog.
		* @param endDepth Ending depth of the fog.							
		*/				
		void setFogProperties(int fogMode, Color color, Number density, Number startDepth, Number endDepth);
		
		void setSceneType(int newType);

		virtual void fixedUpdate();
		virtual void Update(Number elapsed);
	
		bool isEnabled();		
		void setEnabled(bool enabled);
		
		void Render(RenderFrame *frame, Camera *targetCamera = NULL, std::shared_ptr<RenderBuffer> targetFramebuffer = nullptr, std::shared_ptr<Material> overrideMaterial = nullptr, bool shadowMapPass = false);
		
		
		void setOverrideMaterial(std::shared_ptr<Material> material);
		
		void handleEvent(Event *event);
		
		Ray projectRayFromCameraAndViewportCoordinate(Camera *camera, Vector2 coordinate);
		
		/**
		* Scene clear color
		*/ 
		Color clearColor;
		
		/**
		* If set to true, the renderer will clear the screen prior to rendering the scene
		* @default true
		*/
		bool useClearColor;

		/**
		* If set to true, the renderer will clear the depth buffer prior to rendering the scene.
		* @default true
		*/
		bool useClearDepth;

		/**
		* Ambient color, passed to lighting shaders
		*/				
		Color ambientColor;		
		
		/**
		* Fog color, passed to lighting shaders.
		*/						
		Color fogColor;				
		
		/**
		* If this is set to false, the scene is not rendered or updated during the render loop.
		*/
		bool enabled;
		
		/**
		* If ownsChildren is set to true, the scene will delete its children upon destruction (defaults to false).
		*/
		bool ownsChildren;
		
		static const int SCENE_3D = 0;
		static const int SCENE_2D = 1;
		static const int SCENE_2D_TOPLEFT = 2;
						
		Entity rootEntity;
		
		Polycode::Rectangle sceneMouseRect;
		bool remapMouse;
		
		bool constrainPickingToViewport;
		
		void doVisibilityChecking(bool val);
		bool doesVisibilityChecking();
			  
	protected:
		
		void initScene(int sceneType);
		void setEntityVisibility(Entity *entity, Camera *camera);
		void setEntityVisibilityBool(Entity *entity, bool val);
		
		bool _doVisibilityChecking;

		Camera *defaultCamera;
		Camera *activeCamera;
		
		std::shared_ptr<Material> overrideMaterial;
		
		Core *core;
		
		bool fogEnabled;
		int fogMode;
		Number fogDensity;
		Number fogStartDepth;
		Number fogEndDepth;
		
		int sceneType;
		
	};
}
