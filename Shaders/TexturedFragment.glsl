#version 330 core
uniform sampler2D diffuseTex;
//uniform sampler2D diffuseTex2;

in Vertex {
  vec2 texCoord;
  //vec4 jointWeights;
} IN;

out vec4 fragColour;
//out vec4 jointColour;
void main(void) {
 // fragColour = mix(texture(diffuseTex2, IN.texCoord), //texture(diffuseTex, IN.texCoord), 1.0);
  vec4 texColour = texture(diffuseTex, IN.texCoord);
  if(texColour.a < 0.1) {
     discard;
  }
  fragColour = texColour;
 // jointColour = IN.jointWeights;
}