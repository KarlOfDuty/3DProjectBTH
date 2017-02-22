#version 440
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

void main()
{
	gPosition = FragPos;
	gNormal = normalize(Normal);
	gAlbedoSpec = vec4(TexCoords,1,1);
} 