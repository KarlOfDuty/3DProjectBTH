#version 440
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexTexture;
layout (location = 2) in vec3 vertexNormal;
layout (location = 3) in int vertexUseNormalMap;
layout (location = 4) in vec3 tangent;
//layout (location = 5) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out int useNormalMap;
out mat3 TBN;

void main()
{
	vec4 worldPos = model * vec4(vertexPos, 1.0f);
    FragPos = worldPos.xyz;
	gl_Position = projection * view * worldPos;
	TexCoords = vertexTexture;
	mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * vertexNormal;
	
	useNormalMap = vertexUseNormalMap;
	vec3 T = normalize(normalMatrix * tangent);
	vec3 N = normalize(normalMatrix * Normal);
	T = normalize(T-dot(T,N)*N);
	vec3 B = cross(N,T);
	TBN = transpose(mat3(T, B, N));
}