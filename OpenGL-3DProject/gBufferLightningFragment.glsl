#version 440
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gAmbient;
uniform sampler2D depthMap;

struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

const int NR_LIGHTS = 10;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;
uniform mat4 lightSpaceMatrix;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDirection)
{
	// Perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// Get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	float shadow = 0.0;
	// Calculate bias
	float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);   
	// PCF - Percentage-Closer Filtering - used to offset the texture coordinates
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);

	// Check wheter current frag pos is in shadow
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			if(currentDepth - bias > pcfDepth)
			{
				shadow += 1.0;
			}
		}
	}
	// Sample size - increase to improve shadow quality
	shadow /= 18.0;

	return shadow;
}

void main()
{             
    // Retrieve data from G-buffer	
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
	vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 diffuse = texture(gAlbedoSpec, TexCoords).rgb;
	float specular = texture(gAlbedoSpec, TexCoords).a;
	vec3 ambient = texture(gAmbient, TexCoords).rgb;
	//Multiplies the diffuse 
	vec3 lighting = vec3(diffuse.x * ambient.x, diffuse.y * ambient.y, diffuse.z * ambient.z);
	vec3 viewDir = normalize(viewPos - fragPos);
	vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

	for(int i = 0; i < NR_LIGHTS; ++i)
	{
		vec3 lightDir = normalize(lights[i].Position - fragPos);
        vec3 thisDiffuse = max(dot(normal, lightDir), 0.0) * diffuse * lights[i].Color;
        //Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        vec3 thisSpecular = lights[i].Color * spec * specular;
        //Attenuation
		float lightDistance = length(lights[i].Position - fragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * lightDistance + lights[i].Quadratic * lightDistance * lightDistance);
		// Calculate shadows
		float shadow = ShadowCalculation(fragPosLightSpace, normal, lightDir);
        thisDiffuse *= attenuation;
        thisSpecular *= attenuation;
		lighting += (1.0 - shadow) * (thisDiffuse + thisSpecular);
	}
	FragColor = vec4(lighting, 1.0f);
	float depthValue = texture(depthMap,TexCoords).r;
	// Test depthmap
	//FragColor = vec4(vec3(depthValue),1.0);
}

