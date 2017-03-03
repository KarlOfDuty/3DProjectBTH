#version 440
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform sampler2D gPosition;

void main()
{
    gl_Position = vec4(position, 1.0f);
    TexCoords = texCoords;
	vec3 FragPos = texture(gPosition, TexCoords).rgb;;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}