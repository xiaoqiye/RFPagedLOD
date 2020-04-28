#version 430 core
in vec2 vs_TexCoords;

uniform sampler2D uTextureDiffuse;

out vec4 _outColor;

void main()
{
	_outColor = vec4(texture2D(uTextureDiffuse, vec2(vs_TexCoords.x, 1.0 - vs_TexCoords.y)).rgb, 1.0);
}