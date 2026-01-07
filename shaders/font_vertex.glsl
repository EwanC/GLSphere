// Copyright (c) 2025-2026 Ewan Crawford
#version 330 core

layout(location = 0) in vec4 Pos;
out vec2 TexCoords;

uniform mat4 MVP;

void main() {
  gl_Position = MVP * vec4(Pos.xy, 0.0, 1.0);
  TexCoords = Pos.zw;
}
