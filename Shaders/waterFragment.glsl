#version 330 core

uniform sampler2D reflectTex;
uniform sampler2D refractTex;

in Vertex {
 // vec2 texCoord;
  vec4 clipSpace;
  vec4 colour;
} IN;

out vec4 fragColour;

void main(void) {
  vec2 texCoord = (IN.clipSpace.xy / IN.clipSpace.w) / 2 + 0.5; // Get screen space for texture sampling
  vec4 reflect = texture(reflectTex,vec2(texCoord.x, -texCoord.y));
  vec4 refract = texture(refractTex,vec2(texCoord.x, texCoord.y));
 // fragColour = vec4(0,0,1.0,1.0);
  fragColour = mix(reflect, refract, 0.5);
 // fragColour = reflect;
}

