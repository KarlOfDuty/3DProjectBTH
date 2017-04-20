#version 440
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexTexture;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in int vertexUseNormalMap;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec2 texCoords;
out vec3 normal;
out flat int useNormalMap;


void main()
{
	//Position converted to world space
	vec4 worldPos = model * vec4(vertexPos, 1.0f);
    fragPos = worldPos.xyz;
	//Position converted to clip space
	gl_Position = projection * view * worldPos;

	texCoords = vertexTexture;
	//Calculate normal
	mat3 normalMatrix = transpose(inverse(mat3(model)));
    normal = normalMatrix * vertexNormal;
	useNormalMap = vertexUseNormalMap;
}