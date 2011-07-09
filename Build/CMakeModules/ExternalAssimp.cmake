# Build a local version
INCLUDE(ExternalProject)

SET(assimp_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/Dependencies/Assimp)

#SET(assimp_SVN https://assimp.svn.sourceforge.net/svnroot/assimp/tags/2.0)
SET(assimp_GIT_REPO git://github.com/assimp/assimp.git)
SET(assimp_GIT_TAG b12d8be8ca61fc2e7fc1a49b311a5f0d65d77bd1) # TAG 2.0

ExternalProject_Add(assimp
	PREFIX ${assimp_PREFIX}
	GIT_REPOSITORY ${assimp_GIT_REPO}
	GIT_TAG ${assimp_GIT_TAG}
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
)

# tell CMake to look for our version
SET(ASSIMP_FOUND TRUE)
SET(ASSIMP_INCLUDE_DIR ${assimp_PREFIX}/include)
#SET(ASSIMP_LIBRARY ${assimp_PREFIX}/lib)
#SET(ASSIMP_LIBRARIES

#SET(CMAKE_PREFIX_PATH ${assimp_PREFIX})