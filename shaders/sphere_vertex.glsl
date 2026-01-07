// Copyright (c) 2025-2026 Ewan Crawford
#version 330 core

layout(location = 0) in vec3 VertexPos;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

out vec3 Pos;               // worldspace
out vec2 UV;                // worldspace
out vec3 CamEyeDirection;   // cameraspace
out vec3 CamLightDirection; // cameraspace
out vec3 CamNormal;         // cameraspace

uniform mat4 MVP;
uniform mat4 V;
uniform vec3 LightPosition; // Worldspace

void main() {
  Pos = VertexPos;
  UV = VertexTexCoord;
  gl_Position = MVP * vec4(Pos, 1);

  // Vector that goes from the vertex to the camera, in camera space.
  // In camera space, the camera is at the origin (0,0,0).
  vec3 CamVertexPos = (V * vec4(Pos, 1)).xyz;
  CamEyeDirection = vec3(0, 0, 0) - CamVertexPos;

  // Vector that goes from the vertex to the light, in camera space.
  vec3 CamLightPos = (V * vec4(LightPosition, 1)).xyz;
  CamLightDirection = CamLightPos + CamEyeDirection;

  // Normal of the vertex, in camera space
  CamNormal = (V * vec4(VertexNormal, 0)).xyz;
}
