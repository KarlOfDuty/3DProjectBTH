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
	float bias = max(0.013 * (1.0 - dot(normal, lightDirection)), 0.005);    
	if(currentDepth - bias > closestDepth)
	{
		shadow = 1.0;
	}
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
	ambient = vec3(0.5,0.5,0.5);
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
        float distance = length(lights[i].Position - fragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
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

