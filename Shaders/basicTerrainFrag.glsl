#version 330 core
#define numTextures 3
uniform sampler2D terrainSampler[numTextures];
uniform sampler2D terrainBumps[numTextures];
uniform float bound[3];
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform vec3 lightDirection;
uniform float lightAngle;

in Vertex {
  vec2 texCoord;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
 // float bound[3] = {40.0f, 150.0f, 400.0f};
  float height = IN.worldPos.y;
  vec4 diffuse;
  vec3 bumpNormal;
  vec3 incident;
  float attenuation;

  // Check if the light is directional
  if(lightRadius == 0 && lightAngle == 0.0) {
	incident = normalize(-lightDirection);
	attenuation = 1.0;
  } else {
    incident = normalize(lightPos - IN.worldPos);
	float distance = length ( lightPos - IN.worldPos );
	attenuation = 1.0f - clamp ( distance / lightRadius ,0.0 ,1.0);
  }

  
 vec3 viewDir = normalize ( cameraPos - IN.worldPos );
 vec3 halfDir = normalize ( incident + viewDir );

 mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal),
                normalize(IN.normal));
  
 if (height < bound[0]) {
   diffuse = texture(terrainSampler[0], IN.texCoord);
   bumpNormal = texture( terrainBumps[0] , IN.texCoord ).rgb;
 } else if (height < bound[1]) {
   diffuse = mix(texture(terrainSampler[1],IN.texCoord), texture(terrainSampler[0], IN.texCoord), (bound[1] - height)  / (bound[1] - bound[0]));
   bumpNormal = mix(texture(terrainBumps[1],IN.texCoord), texture(terrainBumps[0], IN.texCoord), (bound[1] - height)  / (bound[1] - bound[0])).rgb;
 //  diffuse = texture(terrainBumps[1],IN.texCoord);
 } else if (height < bound[2]) {
   diffuse = mix(texture(terrainSampler[2],IN.texCoord), texture(terrainSampler[1], IN.texCoord), (bound[2] - height)  / (bound[2] - bound[1]));
   bumpNormal = mix(texture(terrainBumps[2],IN.texCoord), texture(terrainBumps[1], IN.texCoord), (bound[2] - height)  / (bound[2] - bound[1])).rgb;
  // diffuse = texture(terrainBumps[2],IN.texCoord);
 }

 bumpNormal = normalize(TBN * normalize(bumpNormal *2.0 - 1.0));
 float lambert = max ( dot ( incident , bumpNormal) , 0.0f );

 float specFactor = clamp ( dot ( halfDir , bumpNormal) ,0.0 ,1.0);
 specFactor = pow ( specFactor , 60.0 );

 vec3 surface = (diffuse.rgb * lightColour.rgb);
 fragColour.rgb = surface * lambert * attenuation;
 fragColour.rgb += (lightColour.rgb * specFactor) * attenuation * 0.33;
 fragColour.rgb += surface * 0.5f; // ambient light
 fragColour.a = diffuse.a;

 //fragColour = diffuse;

}