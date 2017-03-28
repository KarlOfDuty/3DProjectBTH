#version 440
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vTexCoords;

out vec2 texCoords;

void main()
{
    gl_Position = vec4(vertexPos, 1.0f);
    texCoords = vTexCoords;
}