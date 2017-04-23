require('Polycode/EventDispatcher')

function Resource(type) {
	if(arguments[0] != "__skip_ptr__") {
		this.__ptr = Polycode.Resource(type)
	}
	Object.defineProperties(this, {
		'reloadOnFileModify': { enumerable: true, configurable: true, get: Resource.prototype.__get_reloadOnFileModify, set: Resource.prototype.__set_reloadOnFileModify},
		'resourceFileTime': { enumerable: true, configurable: true, get: Resource.prototype.__get_resourceFileTime, set: Resource.prototype.__set_resourceFileTime},
		'platformData': { enumerable: true, configurable: true, get: Resource.prototype.__get_platformData, set: Resource.prototype.__set_platformData}
	})
}

Resource.RESOURCE_TEXTURE = 0
Resource.RESOURCE_MATERIAL = 1
Resource.RESOURCE_SHADER = 2
Resource.RESOURCE_PROGRAM = 3
Resource.RESOURCE_MESH = 5
Resource.RESOURCE_CUBEMAP = 6
Resource.RESOURCE_SPRITE = 7
Resource.RESOURCE_ENTITY_INSTANCE = 8
Resource.RESOURCE_FONT = 9
Resource.RESOURCE_SCRIPT = 10

Resource.prototype = Object.create(EventDispatcher.prototype)

Resource.prototype.__get_reloadOnFileModify = function() {
	return Polycode.Resource__get_reloadOnFileModify(this.__ptr)
}

Resource.prototype.__set_reloadOnFileModify = function(val) {
	Polycode.Resource__set_reloadOnFileModify(this.__ptr, val)
}

Resource.prototype.__get_resourceFileTime = function() {
	var retVal = new size_t("__skip_ptr__")
	retVal.__ptr = 	Polycode.Resource__get_resourceFileTime(this.__ptr)
	return retVal
}

Resource.prototype.__set_resourceFileTime = function(val) {
	Polycode.Resource__set_resourceFileTime(this.__ptr, val.__ptr)
}

Resource.prototype.__get_platformData = function() {
	var retVal = new RendererPlatformData("__skip_ptr__")
	retVal.__ptr = 	Polycode.Resource__get_platformData(this.__ptr)
	return retVal
}

Resource.prototype.__set_platformData = function(val) {
	Polycode.Resource__set_platformData(this.__ptr, val.__ptr)
}

Duktape.fin(Resource.prototype, function (x) {
	if (x === Resource.prototype) {
		return;
	}
	Polycode.Resource__delete(x.__ptr)
})

Resource.prototype.getResourceName = function() {
	return Polycode.Resource_getResourceName(this.__ptr)
}

Resource.prototype.getResourceType = function() {
	return Polycode.Resource_getResourceType(this.__ptr)
}

Resource.prototype.setResourceName = function(newName) {
	Polycode.Resource_setResourceName(this.__ptr, newName)
}

Resource.prototype.setResourcePath = function(path) {
	Polycode.Resource_setResourcePath(this.__ptr, path)
}

Resource.prototype.getResourcePath = function() {
	return Polycode.Resource_getResourcePath(this.__ptr)
}
