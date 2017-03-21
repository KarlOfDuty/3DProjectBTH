#version 440
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout(location = 3) out vec3 gAmbient;


uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalMap;
uniform sampler2D ambientTexture;
uniform int isMouseOvered;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
flat in int useNormalMap;
in mat3 TBN;

void main()
{
	gPosition = FragPos;
	if(useNormalMap != 0)
	{
		gNormal = texture(normalMap, TexCoords).rgb;
		gNormal = normalize(gNormal * 2.0 -1.0);
		gNormal = normalize(TBN * gNormal);
	}
	else
	{
		gNormal = normalize(Normal);
	}
	if (isMouseOvered == 1)
	{
		gAlbedoSpec.rgb = vec3(1,0,0);
	}
	else
	{
		gAlbedoSpec.rgb = texture(diffuseTexture, TexCoords).rgb;
	}
	gAlbedoSpec.a = texture(specularTexture, TexCoords).r;
	gAmbient = texture(ambientTexture,TexCoords).rgb;
} 