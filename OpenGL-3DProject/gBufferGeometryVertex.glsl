#version 440
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexTexture;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in int vertexUseNormalMap;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out int useNormalMap;

void main()
{
	vec4 worldPos = model * vec4(vertexPos, 1.0f);
    FragPos = worldPos.xyz;
	gl_Position = projection * view * worldPos;
	TexCoords = vertexTexture;
	mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * vertexNormal;
	useNormalMap = vertexUseNormalMap;
}