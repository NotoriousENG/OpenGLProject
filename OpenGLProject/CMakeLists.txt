# CMakeList.txt : CMake project for OpenGLProject, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(freetype CONFIG REQUIRED)
find_package(unofficial-brotli CONFIG REQUIRED)

add_executable (OpenGLProject "OpenGLProject.cpp" "OpenGLProject.h" "Mesh.h" "Mesh.cpp" "Camera.h" "Camera.cpp" "ShaderLoader.h" "ShaderLoader.cpp" "LightRenderer.h" "LightRenderer.cpp" "MeshRenderer.h" "MeshRenderer.cpp" "TextureLoader.h" "TextureLoader.cpp" "TextRenderer.h" "TextRenderer.cpp")

file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR}/OpenGLProject)

# TODO: Add tests and install targets if needed.
target_link_libraries(OpenGLProject PRIVATE GLEW::GLEW)
target_link_libraries(OpenGLProject PRIVATE glfw)
target_link_libraries(OpenGLProject PRIVATE glm)
target_link_libraries(OpenGLProject PRIVATE LinearMath Bullet3Common BulletDynamics BulletCollision BulletSoftBody)
target_link_libraries(OpenGLProject PRIVATE unofficial::brotli::brotlidec unofficial::brotli::brotlienc unofficial::brotli::brotlicommon)
target_link_libraries(OpenGLProject PRIVATE freetype)
