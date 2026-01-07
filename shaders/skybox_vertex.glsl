// Copyright (c) 2025-2026 Ewan Crawford
#version 330 core

layout(location = 0) in vec3 Pos;
out vec3 TexCoords;

uniform mat4 MVP;

void main() {
  TexCoords = Pos;
  gl_Position = MVP * vec4(Pos, 1);
}
