// Copyright (c) 2025 Ewan Crawford
#version 330 core

in vec3 Pos;
in vec3 CamNormal;
in vec3 CamEyeDirection;
in vec3 CamLightDirection;
in vec2 UV;

out vec4 Color;

uniform mat4 V;
uniform vec3 LightPosition; // Worldspace
uniform sampler2D TexSampler;

void main() {
  // Try to model sunlight
  vec3 LightColor = vec3(1, 1, 1);
  float LightPower = 50.0f;
  vec3 Light = LightColor * LightPower;

  // Distance to the light
  float Dist = length(LightPosition - Pos);
  float DistSquared = Dist * Dist;
  // Normal of the computed fragment, in camera space
  vec3 N = normalize(CamNormal);

  // Direction of the light (from the fragment to the light)
  vec3 L = normalize(CamLightDirection);

  // Cosine of the angle between the normal and the light direction,
  // clamped above 0
  //  - light is at the vertical of the triangle -> 1
  //  - light is perpendicular to the triangle -> 0
  //  - light is behind the triangle -> 0
  float CosTheta = clamp(dot(N, L), 0, 1);

  // Eye vector (towards the camera)
  vec3 E = normalize(CamEyeDirection);
  // Direction in which the triangle reflects the light
  vec3 R = reflect(-L, N);

  // Cosine of the angle between the Eye vector and the Reflect vector,
  //
  // clamped to 0
  //  - Looking into the reflection -> 1
  //  - Looking elsewhere -> < 1
  float CosAlpha = clamp(dot(E, R), 0, 1);

  // Material properties
  vec3 MaterialDiffuseColor = texture(TexSampler, UV).rgb;
  vec3 MaterialAmbientColor = vec3(0.2, 0.2, 0.2) * MaterialDiffuseColor;
  vec3 MaterialSpecularColor = vec3(0.1, 0.1, 0.1);

  // Phong shading
  vec3 MaterialColor = MaterialAmbientColor;
  MaterialColor += MaterialDiffuseColor * Light * CosTheta / DistSquared;
  MaterialColor +=
      MaterialSpecularColor * Light * pow(CosAlpha, 5) / DistSquared;
  Color = vec4(MaterialColor, 1.);
}
