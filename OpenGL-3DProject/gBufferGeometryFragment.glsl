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

in vec2 texCoords;
in vec3 fragPos;
in vec3 normal;
flat in int useNormalMap;

void main()
{
	gPosition = fragPos;
	//Checks if a normal map is used
	if(useNormalMap == 1)
	{
		gNormal = texture(normalMap, texCoords).rgb;
		//Transform normal vector to range [-1,1] from [0,1]
		gNormal = normalize(gNormal * 2.0 - 1.0);
	}
	else
	{
		gNormal = normalize(normal);
	}
	//Checks if the player clicks on the object
	if (isMouseOvered == 1)
	{
		gAlbedoSpec.rgb = vec3(1,0,0);
	}
	else
	{
		gAlbedoSpec.rgb = texture(diffuseTexture, texCoords).rgb;
	}
	gAlbedoSpec.a = texture(specularTexture, texCoords).r;
	gAmbient = texture(ambientTexture,texCoords).rgb;
} 