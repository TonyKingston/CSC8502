#version 330 core

uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D bumpTex;
uniform sampler2D dudvTex;
uniform sampler2D depthTex;
uniform float waterMove;
uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;
uniform vec3 lightDirection;
uniform float lightAngle;

uniform float near;
uniform float far;

const float waveStrength = 0.01;

in Vertex {
  vec2 texCoord;
  vec4 clipSpace;
  vec4 colour;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
  vec3 incident;
  float attenuation;

  if(lightRadius == 0 && lightAngle == 0.0) {
	incident = normalize(-lightDirection);
	attenuation = 1.0;
  } else {
    incident = normalize(lightPos - IN.worldPos);
	float distance = length ( lightPos - IN.worldPos );
	attenuation = 1.0f - clamp ( distance / lightRadius ,0.0 ,1.0);
	//attenuation = 1.0f;
  }

  vec3 viewDir = normalize ( cameraPos - IN.worldPos );
 // vec3 halfDir = normalize ( incident + viewDir );


  vec2 texCoord = (IN.clipSpace.xy / IN.clipSpace.w) / 2.0 + 0.5; // Get screen space for texture sampling

  // Get value from refract depth buffer
  float depth = texture(depthTex, vec2(texCoord.x, texCoord.y)).r;
  // Calculate distance between camera and terrain under the water
  float floorDistance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
  float waterDistance = 2.0 * near * far / (far + near - (2.0 * gl_FragCoord.z - 1.0) * (far - near));
  float waterDepth = floorDistance - waterDistance;
  // Sampling twice for distortion in both x and y.
  // Doing it this way makes sampling from the bump map easier
  vec2 distort = texture(dudvTex, vec2(IN.texCoord.x + waterMove, IN.texCoord.y)).rg * 0.1; // Only care about red and gree for distortion
  distort = IN.texCoord + vec2(distort.x, distort.y + waterMove);
  vec2 dudv = (texture(dudvTex, distort).rg * 2.0 - 1.0) * waveStrength; 
  vec2 reflectCoord = vec2(texCoord.x,-texCoord.y) + dudv;
  //reflectCoord.x = clamp(reflectCoord.x, 0.001, 0.999);
 // reflectCoord.y = clamp(reflectCoord.y, -0.999, -0.001);

  vec2 refractCoord = vec2(texCoord.x, texCoord.y) + dudv;
  //refractCoord = clamp(refractCoord, 0.001, 0.999);

  /* mat3 TBN = mat3 ( normalize ( IN.tangent ), 
       normalize ( IN.binormal ) , normalize( IN.normal ));
 // vec3 bumpNormal = texture ( bumpTex , IN.texCoord ).rgb;
  vec4 bumpNormalColour = texture ( bumpTex , distort );
//  bumpNormal = normalize ( TBN * normalize ( bumpNormal * 2.0 - 1.0));
  //vec3 bumpNormal = normalize(TBN * normalize (vec3(bumpNormalColour.r * 2.0 -1.0, bumpNormalColour.b, bumpNormalColour.g * 2.0 - 1.0))); // Making the normals point up a little more
  vec3 bumpNormal = normalize(TBN * normalize (bumpNormalColour.rgb *2.0 -1.0)); // Making the normals point up a little more
  float lambert = max ( dot ( incident , bumpNormal ) , 0.0f );*/
 
  /*float specFactor = clamp ( dot ( halfDir , bumpNormal ) ,0.0 ,1.0);
  specFactor = pow ( specFactor , 20.0);*/
 
  vec4 reflectColour = texture(reflectTex,reflectCoord);
  vec4 refractColour = texture(refractTex,refractCoord);

 vec4 normalMapColour = texture(bumpTex, distort);
 vec3 bumpNormal = normalize(vec3(normalMapColour.r * 2.0 - 1.0, normalMapColour.b * 3.0, normalMapColour.g * 2.0 -1.0));

 vec3 fromLight = IN.worldPos.xyz - lightPos;
 vec3 reflectedLight = reflect(normalize(fromLight), bumpNormal);
 float specular = max(dot(reflectedLight, viewDir), 0.0);
 specular = pow(specular, 30.0);
 vec3 specularHighlights = lightColour.rgb * specular * 0.6;

  // Fresnel
 float refractValue = dot(viewDir, bumpNormal);
 refractValue = pow(refractValue, 2.0); // Make the water a bit more reflective
 reflectColour += vec4(specularHighlights, 0.0);
 vec4 texColour = mix(reflectColour, refractColour, refractValue);
 vec3 surface = texColour.rgb * lightColour.rgb;
 //texColour = mix(texColour, vec4(0,0.2,0.5,1.0), 0.2);

 // Add a bit of blue to the final colour
 fragColour = mix(texColour, vec4(0,0.2,0.5,1.0), 0.2) + vec4(specularHighlights, 0.0);
 /*fragColour.rgb = surface * lambert * attenuation;
 fragColour.rgb +=  ( lightColour.rgb * specFactor )* attenuation *0.33;
 fragColour.rgb += surface * 0.1f;
 fragColour.a = texColour.a;*/
// fragColour.a = clamp(waterDepth / 5.0, 0.0, 1.0);

}


