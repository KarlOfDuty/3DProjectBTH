#version 440
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gAmbient;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

void main()
{             
    // Retrieve data from G-buffer
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
	vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 diffuse = texture(gAlbedoSpec, TexCoords).rgb;
	float specular = texture(gAlbedoSpec, TexCoords).a;
	vec3 ambient = texture(gAmbient, TexCoords).rgb;
	//Multiplies the diffuse 
	vec3 lighting = diffuse * 0.5;
	vec3 viewDir = normalize(viewPos - fragPos);
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
        thisDiffuse *= attenuation;
        thisSpecular *= attenuation;
        lighting += thisDiffuse + thisSpecular;
	}
	FragColor = vec4(lighting, 1.0);
}

