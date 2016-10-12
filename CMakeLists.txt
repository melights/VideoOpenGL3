# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (ar)

list(APPEND CMAKE_MODULE_PATH "/home/long/ogl/external/rpavlik-cmake-modules-fe2273")
set(GLM_DIR "/home/long/depend/ogl/external/glm-0.9.7.1/build")
find_package(OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(GLEW 1.13.0 EXACT REQUIRED)
find_package(GLM REQUIRED )
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(X11)
include_directories(
	${PROJECT_SOURCE_DIR}
	/home/long/depend/ogl

)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# Tutorial 1
add_executable(ar 
	main.cpp
	/home/long/depend/ogl/common/shader.cpp
	/home/long/depend/ogl/common/shader.hpp
)
target_link_libraries(ar
	${OPENGL_LIBRARIES}
	${GLFW_LIBRARIES}
	${OpenCV_LIBS}
	${GLEW_LIBRARIES}
	${X11_LIBRARIES}
-lXi -lGLEW -lGLU -lGL -ldrm -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lxcb-dri3 -lxcb-present -lxcb-sync -lxshmfence -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXxf86vm -lXcursor -lXext -lXrender -lXfixes -lX11 -lpthread -lxcb -lXau -lXdmcp
)	


