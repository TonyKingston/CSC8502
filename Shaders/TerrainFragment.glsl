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
uniform sampler2D terrainBumps[numTextures];

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform float lightDirection;
uniform float lightAngle;

in Vertex {
  vec3 colour;
  vec2 texCoord;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec3 worldPos;
} IN;

out vec4 fragColour;
//out vec4 jointColour;
void main(void) {
  int bound[3] = {40, 150, 200};
  float height = IN.worldPos.y;
  vec4 diffuse;
  vec3 bumpNormal;

  vec3 incident = normalize ( lightPos - IN.worldPos );
  vec3 viewDir = normalize ( cameraPos - IN.worldPos );
  vec3 halfDir = normalize ( incident + viewDir );

  mat3 TBN = mat3 ( normalize ( IN.tangent ), 
       normalize ( IN.binormal ) , normalize( IN.normal ));

 if (height < bound[0]) {
   diffuse = texture(terrainSampler[0], IN.texCoord);
   bumpNormal = texture( terrainBumps[0] , IN.texCoord ).rgb;
 } else if (height < bound[1]) {
   diffuse = mix(texture(terrainSampler[1],IN.texCoord), texture(terrainSampler[0], IN.texCoord), (bound[1] - height)  / (bound[1] - bound[0]));
   bumpNormal = mix(texture(terrainBumps[1],IN.texCoord), texture(terrainBumps[0], IN.texCoord), (bound[1] - height)  / (bound[1] - bound[0])).rgb;
 } else if (height < bound[2]) {
   diffuse = mix(texture(terrainSampler[2],IN.texCoord), texture(terrainSampler[1], IN.texCoord), (bound[2] - height)  / (bound[2] - bound[1]));
   bumpNormal = mix(texture(terrainBumps[2],IN.texCoord), texture(terrainBumps[1], IN.texCoord), (bound[2] - height)  / (bound[2] - bound[1])).rgb;
 }
 bumpNormal = normalize ( TBN * normalize ( bumpNormal * 2.0 - 1.0));
 float lambert = max ( dot ( incident , bumpNormal ) , 0.0f );
 float distance = length ( lightPos - IN.worldPos );
 float attenuation = 1.0f - clamp ( distance / lightRadius ,0.0 ,1.0);
 float specFactor = clamp ( dot ( halfDir , bumpNormal ) ,0.0 ,1.0);
 specFactor = pow ( specFactor , 60.0 );
 vec3 surface = ( diffuse.rgb * lightColour.rgb );
 fragColour.rgb = surface * lambert * attenuation;
 fragColour.rgb += ( lightColour.rgb * specFactor )* attenuation *0.33;
 fragColour.rgb += surface * 0.1f;
 fragColour.a = diffuse.a;

}