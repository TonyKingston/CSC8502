#version 330 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 position;
in vec3 normal;
in vec4 colour;

out Vertex {
  vec2 texCoord;
  vec4 clipSpace;
  vec4 colour;
  vec3 normal;
  vec3 worldPos;
} OUT;

void main(void) {
	//gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position.x, 0.0, position.y, 1.0);
	vec4 worldPos = modelMatrix * vec4(position.x, 0.0, position.y, 1.0);
	mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));
	OUT.normal = normalize ( normalMatrix * normalize ( normal ));
	OUT.worldPos = worldPos.xyz;
	vec4 clip = projMatrix * viewMatrix * worldPos;
	OUT.clipSpace = clip;
	gl_Position = clip;
	OUT.texCoord = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5) * 128;
	OUT.colour = colour;
}

