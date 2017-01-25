#version 440
  
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 vertexColor;

out vec4 color;

void main()
{
	color = vertexColor;
    gl_Position = vec4(vertexPos.x, vertexPos.y, vertexPos.z, 1.0);
}