#version 430 core
in vec2 vs_TexCoords;

uniform sampler2D uTextureDiffuse;
uniform int LODLevel;

out vec4 _outColor;

void main()
{
	float DeepColor = 1.0;
	float Red = 0.0, Green = 0.0, Blue = 0.0;
	if (LODLevel >= 8){
		Red = DeepColor;
		Blue = (LODLevel - 8) * 0.25;
	}else if (LODLevel < 8 && LODLevel >= 4)
	{
		Green = DeepColor;
		Blue = (LODLevel - 4) * 0.25;
		Red = 0.4;
	}else{
		Red = DeepColor;
		Green = 1.0;
		Blue = (LODLevel) * 0.25;
	}

	_outColor = vec4(Red, Green, Blue, 1.0);
}