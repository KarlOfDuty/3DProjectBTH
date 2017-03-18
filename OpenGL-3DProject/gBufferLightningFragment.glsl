#version 440
out vec4 FragColor;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D depthMap;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	//Perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//Transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	//Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(depthMap, projCoords.xy).r;
	//Get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	//Check wheter current frag pos is in shadow
	float shadow = 0.0;
	if(currentDepth > closestDepth)
	{
		shadow = 1.0;
	}


	return shadow;
}

void main()
{             
    // Retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	float shadow = ShadowCalculation(FragPosLightSpace);
	vec3 lighting = vec3(0.1 + (1.0-shadow),0.1 + (1.0-shadow),0.1 + (1.0-shadow));
	vec3 viewDir = normalize(viewPos - FragPos);
	for(int i = 0; i < NR_LIGHTS; ++i)
	{
		vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * color * lights[i].Color;
        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;
        // Attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
	}
	lighting * color;
	FragColor = vec4(lighting, 1.0f);
	float depthValue = texture(depthMap,TexCoords).r;
	// Test depthmap
	//FragColor = vec4(vec3(depthValue),1.0);
}

