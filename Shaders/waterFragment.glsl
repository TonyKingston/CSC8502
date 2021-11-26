#version 330 core

uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D dudvTex;
uniform float waterMove;
uniform vec3 cameraPos;

const float waveStrength = 0.01;

in Vertex {
  vec2 texCoord;
  vec4 clipSpace;
  vec4 colour;
  vec3 normal;
  vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {
  vec3 viewDir = normalize ( cameraPos - IN.worldPos );
  float refractValue = dot(viewDir, vec3(0.0,1.0,0.0));
  //vec3 reflectDir = reflect ( -viewDir , normalize ( IN.normal ));
  vec2 texCoord = (IN.clipSpace.xy / IN.clipSpace.w) / 2.0 + 0.5; // Get screen space for texture sampling
  //vec2 dudv = (texture(dudvTex, vec2(IN.texCoord.x + waterMove, IN.texCoord.y)).rg * 2.0 - 1.0) * waveStrength; // Only care about red and gree for distortion
  // Could sample dudv map again for more distortion
  vec2 reflectCoord = vec2(texCoord.x,-texCoord.y);
  //reflectCoord.x = clamp(reflectCoord.x, 0.001, 0.999);
 // reflectCoord.y = clamp(reflectCoord.y, -0.999, -0.001);

  vec2 refractCoord = vec2(texCoord.x, texCoord.y);
  //refractCoord = clamp(refractCoord, 0.001, 0.999);

  vec4 reflectColour = texture(reflectTex,reflectCoord);
  vec4 refractColour = texture(refractTex,refractCoord);

 vec4 texColour = mix(reflectColour, refractColour, refractValue);
 fragColour = refractColour;
 // Add a bit of blue to the final colour
// fragColour = mix(texColour, vec4(0,0.1,0.5,1.0), 0.2);


}


