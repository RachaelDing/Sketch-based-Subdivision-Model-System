#version 430 core

uniform mat4 modelView;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 col;

out vec3 C;

void main(void) {
	C = col;
	gl_Position = modelView * vec4(vertex, 1.0);  
}
