#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform vec4 plane;

in vec3 position;
in vec2 texCoord;


out Vertex {
vec2 texCoord;
float height;
 } OUT;
void main ( void ) {
 vec4 worldPos = ( modelMatrix * vec4 ( position ,1));
 gl_ClipDistance[0] = dot(worldPos,plane);
 gl_Position = projMatrix * viewMatrix * worldPos;
 OUT.texCoord = ( textureMatrix * vec4 ( texCoord , 0.0 , 1.0)).xy;
 //OUT.texCoord = texCoord;
 OUT.height = position.y;
}