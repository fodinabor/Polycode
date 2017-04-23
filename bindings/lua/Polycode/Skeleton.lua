require "Polycode/Entity"

class "Skeleton" (Entity)


function Skeleton:Skeleton(...)
	local arg = {...}
	if type(arg[1]) == "table" and count(arg) == 1 then
		if ""..arg[1].__classname == "Entity" then
			self.__ptr = arg[1].__ptr
			return
		end
	end
	for k,v in pairs(arg) do
		if type(v) == "table" then
			if v.__ptr ~= nil then
				arg[k] = v.__ptr
			end
		end
	end
	if self.__ptr == nil and arg[1] ~= "__skip_ptr__" then
		self.__ptr = Polycode.Skeleton(unpack(arg))
	end
end

function Skeleton:playAnimationByName(animName, weight, once, restartIfPlaying)
	local retVal = Polycode.Skeleton_playAnimationByName(self.__ptr, animName, weight, once, restartIfPlaying)
end

function Skeleton:setBaseAnimationByName(animName)
	local retVal = Polycode.Skeleton_setBaseAnimationByName(self.__ptr, animName)
end

function Skeleton:stopAllAnimations()
	local retVal =  Polycode.Skeleton_stopAllAnimations(self.__ptr)
end

function Skeleton:stopAnimationByName(name)
	local retVal = Polycode.Skeleton_stopAnimationByName(self.__ptr, name)
end

function Skeleton:Update(elapsed)
	local retVal = Polycode.Skeleton_Update(self.__ptr, elapsed)
end

function Skeleton:getBoneByName(name)
	local retVal = Polycode.Skeleton_getBoneByName(self.__ptr, name)
	if retVal == nil then return nil end
	local __c = _G["shared_ptr<Bone>"]("__skip_ptr__")
	__c.__ptr = retVal
	return __c
end

function Skeleton:bonesVisible(val)
	local retVal = Polycode.Skeleton_bonesVisible(self.__ptr, val)
end

function Skeleton:getNumBones()
	local retVal =  Polycode.Skeleton_getNumBones(self.__ptr)
	return retVal
end

function Skeleton:getBone(index)
	local retVal = Polycode.Skeleton_getBone(self.__ptr, index)
	if retVal == nil then return nil end
	local __c = _G["shared_ptr<Bone>"]("__skip_ptr__")
	__c.__ptr = retVal
	return __c
end

function Skeleton:addBone(bone)
	local retVal = Polycode.Skeleton_addBone(self.__ptr, bone.__ptr)
end

function Skeleton:removeBone(bone)
	local retVal = Polycode.Skeleton_removeBone(self.__ptr, bone.__ptr)
end

function Skeleton:getBoneIndexByBone(bone)
	local retVal = Polycode.Skeleton_getBoneIndexByBone(self.__ptr, bone.__ptr)
	return retVal
end

function Skeleton:__delete()
	if self then Polycode.delete_Skeleton(self.__ptr) end
end
