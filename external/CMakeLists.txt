# Copyright (c) 2025 Ewan Crawford

add_subdirectory(glfw-3.4)

# Don't add glew as subdirectory, as it doesn't use CMake.
# Instead compile directly.

set(GLEW_SOURCE glew-1.13.0/src/glew.c)

set(GLEW_HEADERS glew-1.13.0/include/GL/glew.h)

add_library( GLEW_1130 STATIC
	${GLEW_SOURCE}
	${GLEW_HEADERS}
)

target_link_libraries(GLEW_1130
	${OPENGL_LIBRARY}
)

add_subdirectory(freetype)
