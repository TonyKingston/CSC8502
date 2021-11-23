#version 330 core
//uniform sampler2D diffuseTex;
//uniform sampler2D diffuseTex2;
/*uniform sampler2D terrainSampler[16];
uniform float levels[32];
uniform int numLevels;

in Vertex {
  smooth vec2 texCoord;
  smooth float height;
  //vec4 jointWeights;
  
} IN;

out vec4 fragColour;
//out vec4 jointColour;
void main(void) {
  bool isSet = false;
  for(int i = 0; i < numLevels && !isSet; i++) {
    if(IN.height > numLevels) {
      continue;
    }
    int index = i/2;
    if (i % 2 == 0) {
      fragColour = texture(terrainSampler[index], IN.texCoord);
    } else {
      vec4 texColour = texture(terrainSampler[index], IN.texCoord);
      vec4 nextTex = texture(terrainSampler[index+1], IN.texCoord);

      float difference = levels[i] - levels[i-1];
      float next = (IN.height - levels[i-1]) /difference;
      fragColour = texColour*(1.0f - next) + nextTex*next;
    }
    isSet = true;
  }

  if (!isSet) {
    int index = numLevels / 2;
    fragColour = texture(terrainSampler[index], IN.texCoord);
  }
}
*/
//uniform sampler2D diffuseTex;
//uniform sampler2D diffuseTex2;
#define numTextures 3
uniform sampler2D terrainSampler[numTextures];

in Vertex {
  vec2 texCoord;
  float height;
  //vec4 jointWeights;
} IN;

out vec4 fragColour;
//out vec4 jointColour;
void main(void) {
  int bound[3] = {40, 150, 200};
  vec4 diffuse;
 // fragColour = mix(texture(diffuseTex2, IN.texCoord), //texture(diffuseTex, IN.texCoord), 1.0);
 if (IN.height < bound[0]) {
   diffuse = texture(terrainSampler[0], IN.texCoord);
 } else if (IN.height < bound[1]) {
   diffuse = mix(texture(terrainSampler[1],IN.texCoord), texture(terrainSampler[0], IN.texCoord), (bound[1] - IN.height)  / (bound[1] - bound[0]));
 } else if (IN.height < bound[2]) {
   diffuse = mix(texture(terrainSampler[2],IN.texCoord), texture(terrainSampler[1], IN.texCoord), (bound[2] - IN.height)  / (bound[2] - bound[1]));
 }
 fragColour = diffuse;
 // fragColour = texture(diffuseTex, IN.texCoord);
 // jointColour = IN.jointWeights;
}