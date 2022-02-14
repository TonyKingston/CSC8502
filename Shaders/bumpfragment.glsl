#version 330 core
#define MAX_LIGHTS 3

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

uniform vec3 cameraPos;
uniform vec4 lightColour[MAX_LIGHTS];
uniform vec3 lightPos[MAX_LIGHTS];
uniform float lightRadius[MAX_LIGHTS];
uniform vec3 lightDirection[MAX_LIGHTS];
uniform float lightAngle[MAX_LIGHTS];
uniform float linear[MAX_LIGHTS];
uniform float quadratic[MAX_LIGHTS];
uniform float ambient[MAX_LIGHTS];

uniform int noOfLights;

in Vertex {
 vec3 colour;
 vec2 texCoord;
 vec3 normal;
 vec3 tangent; 
 vec3 binormal;
 vec3 worldPos;
 vec3 position;
} IN;

out vec4 fragColour;

void main ( void ) {
  vec3 incident;
  float attenuation;
  vec4 diffuse = texture(diffuseTex, IN.texCoord);
  vec3 bumpNormal = texture ( bumpTex , IN.texCoord ).rgb;
  if(diffuse.a < 0.1) {
     discard;
  }

  vec3 viewDir = normalize ( cameraPos - IN.worldPos );
  for (int i = 0; i < noOfLights; i++) {
  // Check if the light is directional
    if(lightRadius[i] == 0 && lightAngle[i] == 0.0) {
	  incident = normalize(-lightDirection[i]);
      attenuation = 1.0;
    } else {
      incident = normalize(lightPos[i] - IN.worldPos);
	  float distance = length ( lightPos[i] - IN.worldPos );
	  //attenuation = 1.0f - clamp ( distance / lightRadius ,0.0 ,1.0); // Linear attenuation
	  attenuation = 1/(1.0f + linear[i] * distance + quadratic[i] * pow(distance, 2)); // Constant-linear-quadratic falloff attenuation
      if(lightAngle[i] != 0) {	// Check if spotlight
        if (acos(dot(incident, normalize(-lightDirection[i]))) < lightAngle[i]) {
          continue;
        }
	  }
    }

    vec3 halfDir = normalize ( incident + viewDir );

    mat3 TBN = mat3 ( normalize ( IN.tangent ), 
       normalize ( IN.binormal ) , normalize( IN.normal ));
    bumpNormal = normalize ( TBN * normalize ( bumpNormal * 2.0 - 1.0));
    float lambert = max ( dot ( incident , bumpNormal ) , 0.0f );

    float specFactor = clamp ( dot ( halfDir , bumpNormal ) ,0.0 ,1.0);
    specFactor = pow ( specFactor , 60.0 );

    vec3 surface = ( diffuse.rgb * lightColour[i].rgb );
    fragColour.rgb += surface * lambert * attenuation;
    fragColour.rgb += ( lightColour[i].rgb * specFactor )* attenuation *0.33;
    fragColour.rgb += surface * ambient[i];
    fragColour.a += diffuse.a;
  }
  //bumpNormal = texture ( bumpTex , IN.texCoord ).rgb;
 // fragColour.rgb = IN.normal;
}

