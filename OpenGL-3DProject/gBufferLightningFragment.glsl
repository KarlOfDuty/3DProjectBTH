#version 440
out vec4 FragColor;
in vec2 TexCoords;

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
uniform mat4 lightSpaceMatrix;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDirection)
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
	float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);    
	if(currentDepth - bias > closestDepth)
	{
		shadow = 1.0;
	}
	//float shadow = currentDepth > closestDepth  ? 1.0 : 0.0; 


	return shadow;
}

void main()
{             
    // Retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
	vec3 color = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;
	vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
	//vec3 lighting = Diffuse * (1.0 + (1.0 - shadow));
	vec3 ambient = Diffuse * 0.15;
	vec3 lighting = ambient;
	vec3 viewDir = normalize(viewPos - FragPos);
	for(int i = 0; i < NR_LIGHTS; ++i)
	{
		vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
        vec3 specular = lights[i].Color * spec * Specular;
        // Attenuation
        float distance = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        //lighting += diffuse + specular;
		float shadow = ShadowCalculation(FragPosLightSpace, Normal, lightDir);
		lighting += (1.0 - shadow) * (diffuse + specular);    
	}
	FragColor = vec4(lighting, 1.0f);
	float depthValue = texture(depthMap,TexCoords).r;
	// Test depthmap
	//FragColor = vec4(vec3(depthValue),1.0);
}

