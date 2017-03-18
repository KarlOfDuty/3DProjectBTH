#version 440
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform sampler2D gPosition;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = vec4(vertexPos, 1.0f);
    TexCoords = texCoords;
	vec3 FragmentPos = vec3(model * texture(gPosition, texCoords));
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}