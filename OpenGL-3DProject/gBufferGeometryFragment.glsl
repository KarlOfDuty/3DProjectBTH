#version 440
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalMap;
uniform int isMouseOvered;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

void main()
{
	gPosition = FragPos;
	//Normal map has it's texture coordinates inverted
    //gNormal = normalize(Normal);
	gNormal = texture(normalMap, TexCoords).rgb;
    //Transform normal vector to range [-1,1] from [0,1]
    gNormal = normalize(gNormal * 2.0 - 1.0);   
	//If mouse is over diffuse = red, else its the real texture
	if (isMouseOvered == 1)
	{
		gAlbedoSpec.rgb = vec3(1,0,0);
	}
	else
	{
		gAlbedoSpec.rgb = texture(diffuseTexture, TexCoords).rgb;
	}
	gAlbedoSpec.a = texture(specularTexture, TexCoords).r;
} 