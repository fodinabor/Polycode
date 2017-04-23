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
#include "polycode/core/PolyEntity.h"
#include "polycode/core/PolyShader.h"
#include "polycode/core/PolyRenderDataArray.h"
#include "polycode/core/PolyRenderer.h"

namespace Polycode {

	class Material;
	class Mesh;
	class Texture;
	class Skeleton;
    class Bone;
	class Image;
	class ResourcePool;
    
	/**
	* 3D polygonal mesh instance. The SceneMesh is the base for all polygonal 3d geometry. It can have simple textures or complex materials applied to it.
	*/
	class _PolyExport SceneMesh : public Entity {
		public:
		
        
            SceneMesh();
        
			/**
			* Construct a scene mesh from a mesh file.
			* @param fileName Path to mesh file to load.
			*/
			explicit SceneMesh(ResourcePool *pool, const String& fileName);
			
			/**
			* Construct scene mesh from an existing Mesh instance.
			*/
			explicit SceneMesh(std::shared_ptr<Mesh> mesh);

			
			/**
			* Construct scene mesh from an existing Mesh instance.
			*/			
			static SceneMesh *SceneMeshFromMesh(std::shared_ptr<Mesh> mesh);
			
			virtual ~SceneMesh();
			
			void Render(GPUDrawBuffer *buffer);
			

			ShaderPass getShaderPass(unsigned int index);
			unsigned int getNumShaderPasses();
			void addShaderPass(ShaderPass pass);
			void removeShaderPass(int shaderIndex);
		
			/**
			* Returns the Mesh instance of the actual mesh.
			*/
			std::shared_ptr<Mesh> getMesh();
			
			/**
			* Returns the material applied.
			*/							
			std::shared_ptr<Material> getMaterial();
			
			/**
			* Loads a skeleton from a file and applies it to the scene mesh.
			* @param fileName Filename to load the skeleton from.
			*/
			std::shared_ptr<Skeleton> loadSkeleton(Core *core, const String& fileName);

			/**
			* Clears the currently applied material
			*/
			void clearMaterial();

			/**
			* Set material from existing Material instance.
			* @param material Material to apply.
			*/												
			void setMaterial(std::shared_ptr<Material> material);
			
			/**
			* Set the mesh this scene mesh renders.
			* @param mesh Set a new mesh to render.
			*/															
			void setMesh(std::shared_ptr<Mesh> mesh);
		
			/**
			* Sets a skeleton from an existing skeleton instance.
			* @param skeleton Skeleton to set to this mesh.
			*/
			void setSkeleton(std::shared_ptr<Skeleton> skeleton);
			
			/**
			* Returns the skeleton applied to this scene mesh.
			*/
			std::shared_ptr<Skeleton> getSkeleton();
			
			/**
			 * Sets the line width for line-based meshes.
			 */
			void setLineWidth(Number newWidth);

			/**
			 * If this mesh was loaded form file, returns the filename of the loaded mesh.
			 */
			String getFilename();
		
			/**
			 * Sets the filename path of the mesh.
			 */
			void setFilename(const String &fileName);
		
			/**
			 * Loads mesh from file. Deletes current mesh if ownsMesh is set to true.
			 */
			void loadFromFile(ResourcePool *pool, const String &fileName);
		
			/**
			 * Line width for line-based meshes.
			 */
			Number lineWidth;
		
			/**
			 * If set to true, will antialias the lines in a line-based mesh. Defaults to false.
			 */
			bool lineSmooth;
			
			/**
			 * If setto true, will antialias points in a point-based mesh. Defaults to false.
			 */
			bool pointSmooth;
			
			/**
			 * If set to true, will check against actual geometry polygons on ray hit detection. Defaults to false.
			 */
			bool useGeometryHitDetection;
			
			bool customHitDetection(const Ray &ray);
		
			/**
			 * The Renderer has an ability to set an override material that is set for all rendered entities. If forceMaterial is set to true, this entity will always use its assigned material, even if an override material is set.
			 */
			void setForceMaterial(bool forceMaterial);
			bool getForceMaterial();
		
			virtual Entity *Clone(bool deepClone, bool ignoreEditorOnly) const;
			virtual void applyClone(Entity *clone, bool deepClone, bool ignoreEditorOnly) const;
			
			/**
			 * If this flag is set to false, backface culling is disabled when rendering this entity, rendering both sides of each face. Set to true by default.
			 */
			bool backfaceCulled;
			bool sendBoneMatricesToMaterial;
			
		protected:
		
            void createMaterialBoneParams(ShaderBinding *shaderBinding);
			bool useVertexBuffer;
			std::shared_ptr<Mesh> mesh;
			std::shared_ptr<Material> material;
			std::shared_ptr<Skeleton> skeleton;
			std::vector<std::shared_ptr<LocalShaderParam> > colorParams;
            std::vector<std::shared_ptr<LocalShaderParam> > boneMatrixParams;        
			std::vector<ShaderPass> shaderPasses;
			String fileName;
		
		
		
	};
}
