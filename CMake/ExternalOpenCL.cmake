# download C++ Wrapper for OpenCL

IF(NOT EXISTS ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/opencl.h)
    MESSAGE("Downloading http://www.khronos.org/registry/cl/api/1.1/opencl.h")
    FILE(DOWNLOAD http://www.khronos.org/registry/cl/api/1.1/opencl.h
        ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/opencl.h
        SHOW_PROGRESS)
ENDIF()

IF(NOT EXISTS ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl.h)
    MESSAGE("Downloading http://www.khronos.org/registry/cl/api/1.1/cl.h")
    FILE(DOWNLOAD http://www.khronos.org/registry/cl/api/1.1/cl.h
        ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl.h
        SHOW_PROGRESS)
ENDIF()

IF(NOT EXISTS ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_platform.h)
    MESSAGE("Downloading http://www.khronos.org/registry/cl/api/1.1/cl_platform.h")
    FILE(DOWNLOAD http://www.khronos.org/registry/cl/api/1.1/cl_platform.h
        ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_platform.h
        SHOW_PROGRESS)
ENDIF()

IF(NOT EXISTS ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_ext.h)
    MESSAGE("Downloading http://www.khronos.org/registry/cl/api/1.1/cl_ext.h")
    FILE(DOWNLOAD http://www.khronos.org/registry/cl/api/1.1/cl_ext.h
        ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_ext.h
        SHOW_PROGRESS)
ENDIF()

IF(NOT EXISTS ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_gl.h)
    MESSAGE("Downloading http://www.khronos.org/registry/cl/api/1.1/cl_gl.h")
    FILE(DOWNLOAD http://www.khronos.org/registry/cl/api/1.1/cl_gl.h
        ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_gl.h
        SHOW_PROGRESS)
ENDIF()

IF(NOT EXISTS ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_gl_ext.h)
    MESSAGE("Downloading http://www.khronos.org/registry/cl/api/1.1/cl_gl_ext.h")
    FILE(DOWNLOAD http://www.khronos.org/registry/cl/api/1.1/cl_gl_ext.h
        ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_gl_ext.h
        SHOW_PROGRESS)
ENDIF()

ADD_CUSTOM_TARGET(CL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_ext.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl_ext.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_gl.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl_gl.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_gl_ext.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl_gl_ext.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/cl_platform.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl_platform.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POLYCODE_DEPS_DOWNLOAD_DIR}/CL/opencl.h ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/opencl.h
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PolycodeDependencies_SOURCE_DIR}/../OpenCL-patch/cl.hpp ${POLYCODE_DEPS_CORE_PREFIX}/include/CL/cl.hpp
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENCL_LIBRARY} ${POLYCODE_DEPS_CORE_PREFIX}/lib/OpenCL.lib
)
