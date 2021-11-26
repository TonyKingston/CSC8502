#version 330 core

uniform sampler2D reflectTex;
uniform sampler2D refractTex;
uniform sampler2D dudvTex;

uniform vec3 cameraPos;

const float waveStrength = 0.02;

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
  vec3 reflectDir = reflect ( -viewDir , normalize ( IN.normal ));
  vec2 texCoord = (IN.clipSpace.xy / IN.clipSpace.w) / 2.0 + 0.5; // Get screen space for texture sampling
  vec2 dudv = (texture(dudvTex, vec2(IN.texCoord.x, IN.texCoord.y)).rg * 2.0 - 1.0) * waveStrength; // Only care about red and gree for distortion
  vec2 reflectCoord = vec2(texCoord.x, -texCoord.y);
 // reflectCoord.x = clamp(reflectCoord.x, 0.001, 0.999);
  //reflectCoord.y = clamp(reflectCoord.x, -0.999, -0.001);
  vec2 refractCoord = vec2(texCoord.x, texCoord.y) + dudv;
  refractCoord = clamp(refractCoord, 0.001, 0.999);
//  vec4 reflect = texture(reflectTex,reflectCoord);
  //vec4 refract = texture(refractTex,refractCoord);
 // fragColour = vec4(0,0,1.0,1.0);
 // fragColour = mix(reflect, refract, 0.5);
  vec4 reflectColour = texture(reflectTex,reflectCoord); 
  fragColour = reflectColour;
}

