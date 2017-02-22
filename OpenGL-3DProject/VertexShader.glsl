#version 440
  
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 vertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 color;

void main()
{
	mat4 mvp = projection*view*model;
	gl_Position = mvp * vec4(vertexPos, 1.0);
	color = vertexColor;
}