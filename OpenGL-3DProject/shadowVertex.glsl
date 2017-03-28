#version 440
layout (location = 0) in vec3 vertexPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	//Transform all vertices to light space using lightSpaceMatrix
    gl_Position = lightSpaceMatrix * model * vec4(vertexPos, 1.0f);
}  