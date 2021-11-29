#version 400

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 plane;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

in vec4 jointWeights;
in ivec4 jointIndices;

uniform mat4 joints [128];

out Vertex {
 vec4 colour;
 vec2 texCoord;
 vec3 normal;
 vec3 tangent; 
 vec3 binormal;
 vec3 worldPos;
  
} OUT;

void main ( void ) {
  vec4 localPos = vec4 ( position , 1.0f);
  vec4 skelPos = vec4 (0 ,0 ,0 ,0);

  for (int i = 0; i < 4; ++ i ) {
    int jointIndex = jointIndices[i];
    float jointWeight = jointWeights[i];

    skelPos += joints[jointIndex] * localPos * jointWeight;
  }
  vec4 worldPos = modelMatrix * vec4(skelPos.xyz, 1.0);
  gl_Position = (projMatrix * viewMatrix) * worldPos;
 mat3 normalMatrix = transpose ( inverse ( mat3 ( modelMatrix )));

 vec3 wNormal = normalize ( normalMatrix * normalize ( normal ));
 vec3 wTangent = normalize ( normalMatrix * normalize ( tangent.xyz ));

 OUT.normal = wNormal;
 OUT.tangent = wTangent;
 OUT.binormal = cross ( wTangent , wNormal ) * tangent.w;
  OUT.colour = colour;
  OUT.texCoord = texCoord;
  OUT.worldPos = worldPos.xyz;
}