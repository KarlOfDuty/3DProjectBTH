#version 440
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

void main()
{             
    // Retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;

	if (TexCoords.x < 0.5  && TexCoords.y < 0.5)
	{
		FragColor = vec4(texture(gPosition, TexCoords*2).rgb,1);
	}
	else if (TexCoords.x > 0.5  && TexCoords.y < 0.5)
	{
		FragColor = vec4(texture(gNormal, TexCoords*2).rgb,1);
	}
	else if (TexCoords.x < 0.5  && TexCoords.y > 0.5)
	{
		FragColor = texture(gAlbedoSpec,TexCoords*2);
	}
	else if (TexCoords.x > 0.5  && TexCoords.y > 0.5)
	{
		FragColor = vec4(1,1,1,1);
	}
}

