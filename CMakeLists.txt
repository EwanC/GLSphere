# Copyright (c) 2025-2026 Ewan Crawford

cmake_minimum_required(VERSION 3.27 FATAL_ERROR)
project(GLSphere VERSION 1.0.0)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "Only Linux supported")
endif()

find_package(OpenGL REQUIRED)

add_subdirectory(external)

include_directories(
    external/glfw-3.4/include
    external/glm
    external/stb
    external/freetype/include
)

add_executable(glsphere src/main.cpp
                        src/shaders.cpp
                        src/sphere.cpp
                        src/controls.cpp
                        src/skybox.cpp
                        src/text.cpp
                        src/texture.cpp)

add_custom_target(copy_shaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
        "$<TARGET_FILE_DIR:glsphere>/shaders"
    COMMENT "Copying shaders"
)

add_custom_target(copy_textures
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/textures/"
        "$<TARGET_FILE_DIR:glsphere>/textures"
    COMMENT "Copying textures"
)

add_custom_target(copy_fonts
	COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/fonts/"
        "$<TARGET_FILE_DIR:glsphere>/fonts"
    COMMENT "Copying fonts"
)

add_dependencies(glsphere copy_shaders copy_textures copy_fonts)
target_link_libraries(glsphere glfw ${OPENGL_LIBRARY} GLEW_1130 freetype)
