function GPUDrawBuffer() {
	if(arguments[0] != "__skip_ptr__") {
		this.__ptr = Polycode.GPUDrawBuffer()
	}
	Object.defineProperties(this, {
		'projectionMatrix': { enumerable: true, configurable: true, get: GPUDrawBuffer.prototype.__get_projectionMatrix, set: GPUDrawBuffer.prototype.__set_projectionMatrix},
		'viewMatrix': { enumerable: true, configurable: true, get: GPUDrawBuffer.prototype.__get_viewMatrix, set: GPUDrawBuffer.prototype.__set_viewMatrix},
		'cameraMatrix': { enumerable: true, configurable: true, get: GPUDrawBuffer.prototype.__get_cameraMatrix, set: GPUDrawBuffer.prototype.__set_cameraMatrix},
		'clearColor': { enumerable: true, configurable: true, get: GPUDrawBuffer.prototype.__get_clearColor, set: GPUDrawBuffer.prototype.__set_clearColor},
		'clearDepthBuffer': { enumerable: true, configurable: true, get: GPUDrawBuffer.prototype.__get_clearDepthBuffer, set: GPUDrawBuffer.prototype.__set_clearDepthBuffer},
		'clearColorBuffer': { enumerable: true, configurable: true, get: GPUDrawBuffer.prototype.__get_clearColorBuffer, set: GPUDrawBuffer.prototype.__set_clearColorBuffer},
		'backingResolutionScale': { enumerable: true, configurable: true, get: GPUDrawBuffer.prototype.__get_backingResolutionScale, set: GPUDrawBuffer.prototype.__set_backingResolutionScale},
		'viewport': { enumerable: true, configurable: true, get: GPUDrawBuffer.prototype.__get_viewport, set: GPUDrawBuffer.prototype.__set_viewport},
		'shadowMapPass': { enumerable: true, configurable: true, get: GPUDrawBuffer.prototype.__get_shadowMapPass, set: GPUDrawBuffer.prototype.__set_shadowMapPass}
	})
}


GPUDrawBuffer.prototype.__get_projectionMatrix = function() {
	var retVal = new Matrix4("__skip_ptr__")
	retVal.__ptr = 	Polycode.GPUDrawBuffer__get_projectionMatrix(this.__ptr)
	return retVal
}

GPUDrawBuffer.prototype.__set_projectionMatrix = function(val) {
	Polycode.GPUDrawBuffer__set_projectionMatrix(this.__ptr, val.__ptr)
}

GPUDrawBuffer.prototype.__get_viewMatrix = function() {
	var retVal = new Matrix4("__skip_ptr__")
	retVal.__ptr = 	Polycode.GPUDrawBuffer__get_viewMatrix(this.__ptr)
	return retVal
}

GPUDrawBuffer.prototype.__set_viewMatrix = function(val) {
	Polycode.GPUDrawBuffer__set_viewMatrix(this.__ptr, val.__ptr)
}

GPUDrawBuffer.prototype.__get_cameraMatrix = function() {
	var retVal = new Matrix4("__skip_ptr__")
	retVal.__ptr = 	Polycode.GPUDrawBuffer__get_cameraMatrix(this.__ptr)
	return retVal
}

GPUDrawBuffer.prototype.__set_cameraMatrix = function(val) {
	Polycode.GPUDrawBuffer__set_cameraMatrix(this.__ptr, val.__ptr)
}

GPUDrawBuffer.prototype.__get_clearColor = function() {
	var retVal = new Color("__skip_ptr__")
	retVal.__ptr = 	Polycode.GPUDrawBuffer__get_clearColor(this.__ptr)
	return retVal
}

GPUDrawBuffer.prototype.__set_clearColor = function(val) {
	Polycode.GPUDrawBuffer__set_clearColor(this.__ptr, val.__ptr)
}

GPUDrawBuffer.prototype.__get_clearDepthBuffer = function() {
	return Polycode.GPUDrawBuffer__get_clearDepthBuffer(this.__ptr)
}

GPUDrawBuffer.prototype.__set_clearDepthBuffer = function(val) {
	Polycode.GPUDrawBuffer__set_clearDepthBuffer(this.__ptr, val)
}

GPUDrawBuffer.prototype.__get_clearColorBuffer = function() {
	return Polycode.GPUDrawBuffer__get_clearColorBuffer(this.__ptr)
}

GPUDrawBuffer.prototype.__set_clearColorBuffer = function(val) {
	Polycode.GPUDrawBuffer__set_clearColorBuffer(this.__ptr, val)
}

GPUDrawBuffer.prototype.__get_backingResolutionScale = function() {
	var retVal = new Vector2("__skip_ptr__")
	retVal.__ptr = 	Polycode.GPUDrawBuffer__get_backingResolutionScale(this.__ptr)
	return retVal
}

GPUDrawBuffer.prototype.__set_backingResolutionScale = function(val) {
	Polycode.GPUDrawBuffer__set_backingResolutionScale(this.__ptr, val.__ptr)
}

GPUDrawBuffer.prototype.__get_viewport = function() {
	var retVal = new Rectangle("__skip_ptr__")
	retVal.__ptr = 	Polycode.GPUDrawBuffer__get_viewport(this.__ptr)
	return retVal
}

GPUDrawBuffer.prototype.__set_viewport = function(val) {
	Polycode.GPUDrawBuffer__set_viewport(this.__ptr, val.__ptr)
}

GPUDrawBuffer.prototype.__get_shadowMapPass = function() {
	return Polycode.GPUDrawBuffer__get_shadowMapPass(this.__ptr)
}

GPUDrawBuffer.prototype.__set_shadowMapPass = function(val) {
	Polycode.GPUDrawBuffer__set_shadowMapPass(this.__ptr, val)
}

Duktape.fin(GPUDrawBuffer.prototype, function (x) {
	if (x === GPUDrawBuffer.prototype) {
		return;
	}
	Polycode.GPUDrawBuffer__delete(x.__ptr)
})
