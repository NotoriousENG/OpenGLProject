﻿# CMakeList.txt : CMake project for OpenGLProject, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable (OpenGLProject "OpenGLProject.cpp" "OpenGLProject.h" "Mesh.h" "Mesh.cpp" "Camera.h" "Camera.cpp" "ShaderLoader.h" "ShaderLoader.cpp" "LightRenderer.h" "LightRenderer.cpp")

file(COPY Assets DESTINATION ${CMAKE_BINARY_DIR}/OpenGLProject)

# TODO: Add tests and install targets if needed.
target_link_libraries(OpenGLProject PRIVATE GLEW::GLEW)
target_link_libraries(OpenGLProject PRIVATE glfw)
target_link_libraries(OpenGLProject PRIVATE glm)