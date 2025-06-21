cmake_minimum_required(VERSION 3.1)
project(RigidBodySimulation)

include(Cmake/CPM.cmake)

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries")
file(GLOB SOURCES 
	${SRC_DIR}/*.cpp)

# Include files
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Includes")

# Config file
configure_file(${SRC_DIR}/helpers/RootDir.h.in ${SRC_DIR}/helpers/RootDir.h)

# Executable definitions and properties
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${INCLUDE_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
if(NOT WIN32)
	target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()

# GLFW
CPMAddPackage(
  NAME glfw
  GIT_REPOSITORY "https://github.com/glfw/glfw.git"
  GIT_TAG "3.4"
  OPTIONS
    "GLFW_BUILD_EXAMPLES OFF"
    "GLFW_BUILD_TESTS OFF"
    "GLFW_BUILD_DOCS OFF"
    "GLFW_INSTALL OFF"
  )

#target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_SOURCE_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# GLM
CPMAddPackage("https://github.com/g-truc/glm.git#1.0.1")

# GLAD
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "glfw" "glm::glm" "${CMAKE_DL_LIBS}")