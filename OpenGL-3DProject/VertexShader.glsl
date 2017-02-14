#version 440
  
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexColor;
layout (location = 5) uniform mat4 model;
layout (location = 6) uniform mat4 view;
layout (location = 7) uniform mat4 projection;

out vec4 color;

void main()
{
	mat4 mvp = projection*view*model;
	gl_Position = mvp * vec4(vertexPos, 1.0);
	color = vec4(vertexColor, 1.0);
}