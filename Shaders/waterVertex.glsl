#version 330 core

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;


out Vertex {
  vec2 texCoord;
  vec4 clipSpace;
  vec4 colour;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec3 worldPos;
} OUT;

void main(void) {

	vec4 worldPos = modelMatrix * vec4(position.x, 0.0, position.y, 1.0);
	mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));

	vec3 wNormal = normalize ( normalMatrix * normalize ( normal ));
    vec3 wTangent = normalize ( normalMatrix * normalize ( tangent.xyz ));

	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.binormal = cross ( wTangent , wNormal ) * tangent.w;

	OUT.worldPos = worldPos.xyz;
	vec4 clip = projMatrix * viewMatrix * worldPos;
	OUT.clipSpace = clip;
	gl_Position = clip;
	OUT.texCoord = vec2(position.x/2.0 + 0.5, position.y/2.0 + 0.5) * 6.0;
	OUT.colour = colour;
}

