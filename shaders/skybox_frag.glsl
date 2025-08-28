// Copyright (c) 2025 Ewan Crawford
#version 330 core

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube Skybox;

void main() { FragColor = texture(Skybox, TexCoords); }
