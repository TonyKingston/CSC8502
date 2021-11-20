#version 330 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 position;
in vec4 colour;

out Vertex {
  vec4 colour;
} OUT;

void main(void) {
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position.x, 0.0, position.y, 1.0);
	//OUT.texCoord = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5);
	OUT.colour = colour;
}

