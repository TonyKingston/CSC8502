#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform vec3 lightDirection;
uniform float lightAngle;
uniform float linear;
uniform float quadratic;

in Vertex {
 vec3 colour;
 vec2 texCoord;
 vec3 normal;
 vec3 tangent; 
 vec3 binormal;
 vec3 worldPos;
} IN;

out vec4 fragColour;

void main ( void ) {
  vec3 incident;
  float attenuation;
  vec4 diffuse = texture(diffuseTex, IN.texCoord);
  if(diffuse.a < 0.1) {
     discard;
  }

  // Check if the light is directional
  if(lightRadius == 0 && lightAngle == 0.0) {
	incident = normalize(-lightDirection);
	attenuation = 1.0;
  } else {
    incident = normalize(lightPos - IN.worldPos);
	float distance = length ( lightPos - IN.worldPos );
	//attenuation = 1.0f - clamp ( distance / lightRadius ,0.0 ,1.0); // Linear attenuation
	attenuation = 1/(1.0f + linear * distance + quadratic * pow(distance, 2)); // Constant-linear-quadratic falloff attenuation
  }

 vec3 viewDir = normalize ( cameraPos - IN.worldPos );
 vec3 halfDir = normalize ( incident + viewDir );

 mat3 TBN = mat3 ( normalize ( IN.tangent ), 
       normalize ( IN.binormal ) , normalize( IN.normal ));
 vec3 bumpNormal = texture ( bumpTex , IN.texCoord ).rgb;
 bumpNormal = normalize ( TBN * normalize ( bumpNormal * 2.0 - 1.0));
 float lambert = max ( dot ( incident , bumpNormal ) , 0.0f );

 float specFactor = clamp ( dot ( halfDir , bumpNormal ) ,0.0 ,1.0);
 specFactor = pow ( specFactor , 60.0 );
 vec3 surface = ( diffuse.rgb * lightColour.rgb );
 fragColour.rgb = surface * lambert * attenuation;
 fragColour.rgb += ( lightColour.rgb * specFactor )* attenuation *0.33;
 fragColour.rgb += surface * 0.1f;
 fragColour.a = diffuse.a;
}

