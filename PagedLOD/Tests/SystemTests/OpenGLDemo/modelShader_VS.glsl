#version 430 core

layout (location = 0) in vec3 _inPosition;
layout (location = 1) in vec2 _inTexCoords;

out vec2 vs_TexCoords;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelMatrix;

void main(){
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(_inPosition, 1.0);
    vs_TexCoords = _inTexCoords;
}