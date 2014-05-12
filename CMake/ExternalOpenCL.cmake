# download C++ Wrapper for OpenCL

ADD_CUSTOM_TARGET(CL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_INCLUDE_DIRS}/CL/cl.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_INCLUDE_DIRS}/CL/cl_ext.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl_ext.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_INCLUDE_DIRS}/CL/cl_gl.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl_gl.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_INCLUDE_DIRS}/CL/cl_gl_ext.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl_gl_ext.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_INCLUDE_DIRS}/CL/cl_platform.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl_platform.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_INCLUDE_DIRS}/CL/opencl.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/opencl.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PolycodeDependencies_SOURCE_DIR}/../OpenCL-patch/cl.hpp ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl.hpp
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_LIBRARY} ${POLYCODE_DEPS_CORE_PREFIX}/lib/OpenCL.lib
)