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
#include "polycode/core/PolyVector2.h"
#include "polycode/core/PolyVector4.h"

namespace Polycode {

	class Material;
	class ShaderBinding;
	class Texture;
	class GPUDrawCall;
	class RenderFrame;

	/**
	* Camera in a 3D scene. Cameras can be added to a scene and changed between dynamically. You can also set a shader to a camera that will run as a screen shader for post-processing effects.
	*/	
	class _PolyExport Camera : public Entity {
		public:
			
			/** ProjectionMode: Orthographic projection, with manually set size. */
			static const int ORTHO_SIZE_MANUAL = 0;

			/** ProjectionMode: Orthographic projection, with height specified used and width scaled proportionally . */
			static const int ORTHO_SIZE_LOCK_HEIGHT = 1;

			/** ProjectionMode: Orthographic projection, with width specified used and height scaled proportionally. */
			static const int ORTHO_SIZE_LOCK_WIDTH = 2;

			/** ProjectionMode: Orthographic projection, scaled to viewport backing resolution. */
			static const int ORTHO_SIZE_VIEWPORT = 3;

			/** ProjectionMode: Perspective projection, with field of view specified. */
			static const int PERSPECTIVE_FOV = 4;

			/** ProjectionMode: Perspective projection, with bounds set by edges of frustum. */
			static const int PERSPECTIVE_FRUSTUM = 5;
		
			/** ProjectionMode: Manual matrix projection. Use setProjectionMatrix to set the matrix. */
			static const int MANUAL_MATRIX = 6;
		

			/**
			* Constructor.
			* @param parentScene Scene to add the camera to.
			*/
			explicit Camera();
			virtual ~Camera();
			
			/**
			 * Builds the frustum clipping planes for this camera using the current render modelview and the camera's projection matrices.
			 */
			void buildFrustumPlanes();
			
			/**
			* Checks if the camera can see a sphere.
			* @param pos Position of the sphere to check.
			* @param fRadius Radius of the sphere.
			* @return Returns true if the sphere is within the camera's frustum, or false if it isn't.
			* @see canSee()
			*/								
			bool isSphereInFrustum(const Vector3 &pos, Number fRadius);
		
			/**
			 * Checks if an Axis-aligned bounding box is visible to the camera.
			 * @param aabb An axis-aligned bounding box
			 * @return Returns true if the AABB is within the camera's frustum, false if it isn't.
			 */
			bool isAABBInFrustum(const AABB &aabb);
			
			/**
			* Toggles orthographic projection mode for camera.
			* @param mode If true, sets the camera into orthographic projection mode.
			* @param orthoSizeX Width of the orthographic frustum (defaults to 1.0)
			* @param orthoSizeY Height of the orthographic frustum (defaults to 1.0)				
			*/			
			void setOrthoMode(bool mode);

			/**
			 * Sets the orthographic size of the camera.
			 * @param orthoSizeX Orthographic width
			 * @param orthoSizeY Orthographic height
			 */
			void setOrthoSize(Number orthoSizeX, Number orthoSizeY);
			
			/** Switches into frustum mode and sets up the planes. */
			void setFrustumMode(Number left, Number right, Number bottom, Number top, Number front, Number back);

			/**
			* Returns true if camera is in orthographic projection mode.
			* @return True if camera is orthographic, false if otherwise.
			*/
			bool getOrthoMode() {
				return projectionMode == ORTHO_SIZE_MANUAL ||
						projectionMode == ORTHO_SIZE_LOCK_HEIGHT ||
						projectionMode == ORTHO_SIZE_LOCK_WIDTH ||
						projectionMode == ORTHO_SIZE_VIEWPORT;
			}
			
			/**
			* Returns the width of the camera's orthographic frustum.
			* @return Width of the camera's orthographic frustum.
			*/
			Number getOrthoSizeX();
			
			/**
			* Returns the height of the camera's orthographic frustum.
			* @return Height of the camera's orthographic frustum.
			*/			
			Number getOrthoSizeY();
						
			/**
			* Sets the field of view (FOV) for the camera. The larger the field of view, the more the camera can see, the smaller it is, the more zoomed in it is.
			* @param fov The new FOV value.
			*/			
			void setFOV(Number fov);
			
			/**
			* Returns the current FOV value for the camera.
			* @return Current FOV value for the camera.
			*/			
			Number getFOV() {
				return fov;
			}
			
			/**
			 * Sets the clipping planes for the camera.
			 * @param nearClipPlane Near clipping plane.
			 * @param farClipPlane Far clipping plane.
			 */
			void setClippingPlanes(Number nearClipPlane, Number farClipPlane);
		
			/**
			 * Returns the near clipping plane of the camera.
			 * @return Near clipping plane of the camera.
			 */
			Number getNearClippingPlane();
		
			/**
			 * Returns the far clipping plane of the camera.
			 * @return Far clipping plane of the camera.
			 */
			Number getFarClippingPlane();

			Matrix4 createProjectionMatrix();

			/**
			 * Check if camera has a post filter material applied
			 * @return True if the camera has a filter material applied.
			 */
			bool hasFilterShader();
		
			/**
			 * Binds target buffers and renders the scene in multiple passes based on the post filter material.
			 */
			void drawFilter(RenderFrame *frame, std::shared_ptr<RenderBuffer> targetBuffer, Scene *targetScene);

			/**
			* Sets the post-processing shader for the camera.
			* @param shaderMaterial Post processing shader material.
			*/
			void setPostFilter(std::shared_ptr<Material> material);
			
			/**
			* Removes the currently assigned post filter.
			*/
			void removePostFilter();
			
			/**
			* Returns the shader material applied to the camera.
			*/			
			std::shared_ptr<Material> getScreenShaderMaterial() { return filterShaderMaterial; }

			/**
			 * Clones the camera.
			 */
			virtual Entity *Clone(bool deepClone, bool ignoreEditorOnly) const;

			/**
			 * Applies clone parameters to the camera.
			 */
			virtual void applyClone(Entity *clone, bool deepClone, bool ignoreEditorOnly) const;
			
			/**
			 * Returns the camera's projection matrix.
			 * @return Projection matrix.
			 */
			Matrix4 getProjectionMatrix();
		
			/**
			 * Manually sets the camera's projection matrix. Projection mode must be set to MANUAL_MATRIX.
			 * @param matrix Custom projection matrix.
			 * @see setProjectionMode
			 */
			void setCustomProjectionMatrix(Matrix4 matrix);
			
			/**
			 * Return's the camera's pixel viewport based on the last render pass.
			 */
			Polycode::Rectangle getViewport();

			void setViewport(const Polycode::Rectangle &viewport);
		
			/**
			* Toggles the frustum culling of the camera. (Defaults to true).
			*/
			bool frustumCulling;
			
			/**
			 * If set to true, the orthographic projection will be set with the 0,0 coordinate in the top left corner of the viewport. Otherwise, the 0,0 coordinate is in the center.
			 */ 
			bool topLeftOrtho;
		
			/**
			* Shifts camera frustum by factor of the frustum size. (x=-1 will shift the frustum to the left by a whole screen width).
			*/
			Vector2 cameraShift;
			  
			/** @deprecated use setProjectionMode(ProjectionMode mode) */
			void setOrthoSizeMode(int orthoSizeMode) { setProjectionMode(orthoSizeMode); }
			/** @deprecated use getProjectionMode() */
			int getOrthoSizeMode() const { return projectionMode; }

			/**
			 * Sets the projection mode of the camera. Possible values are ORTHO_SIZE_MANUAL, ORTHO_SIZE_LOCK_HEIGHT,ORTHO_SIZE_LOCK_WIDTH, ORTHO_SIZE_LOCK_WIDTH, PERSPECTIVE_FOV, PERSPECTIVE_FRUSTUM and MANUAL_MATRIX.
				See the documentation of each individual mode for details.
			 * @param mode New projection mode.
			 */
			void setProjectionMode(int mode);
		
			/**
			 * Returns the current projection mode.
			 * @return Current projection mode.
			 */
			int getProjectionMode() const { return projectionMode; }

			Vector3 projectRayFrom2DCoordinate(const Vector2 &coordinate, const Polycode::Rectangle &viewport);

			void renderFullScreenQuad(GPUDrawBuffer *drawBuffer, int shaderPass);
		
			ShaderPass getShaderPass(unsigned int index);
			unsigned int getNumShaderPasses();		  
		
		protected:
		
			Mesh *screenQuadMesh;

            void rebuildMaterialBindings();
			void setOrthoMatrix(Matrix4 &matrix, Number xSize, Number ySize, Number _near, Number _far, bool centered);
		
			int projectionMode;		   
			Matrix4 projectionMatrix;

			Polycode::Rectangle viewport;
			Number orthoSizeX;
			Number orthoSizeY;
		
			std::vector<ShaderPass> shaderPasses;

			Number nearClipPlane;
			Number farClipPlane;
			Number fov;

			Number leftFrustum,rightFrustum,topFrustum,bottomFrustum;

			Vector4 frustumPlanes[6];

			std::shared_ptr<Material> filterShaderMaterial;
			std::shared_ptr<RenderBuffer> originalFramebuffer;
		
			bool _hasFilterShader;
	};	
}
