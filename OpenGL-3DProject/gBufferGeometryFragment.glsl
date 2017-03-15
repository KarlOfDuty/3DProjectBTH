#version 440
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout(location = 3) out vec3 gAmbient;


uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalMap;
uniform sampler2D ambientTexture;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
flat in int useNormalMap;

void main()
{
	gPosition = FragPos;
	if(useNormalMap == 1)
	{
		gNormal = texture(normalMap, TexCoords).rgb;
		//Transform normal vector to range [-1,1] from [0,1]
		gNormal = normalize(gNormal * 2.0 - 1.0);
	}
	else
	{
		gNormal = normalize(Normal);
	}
	gAmbient = texture(ambientTexture,TexCoords).rgb;
	//gAlbedoSpec.rgb = texture(diffuseTexture, TexCoords).rgb;
	gAlbedoSpec.rgb = texture(diffuseTexture, TexCoords).rgb;
	gAlbedoSpec.a = texture(specularTexture, TexCoords).r;
} 