#version 140

uniform float time;  
uniform sampler2D texSampler; 
smooth in vec2 texCoord_v; 
out vec4 color_f; 

vec2 texturePosition(){
	 int frame = int(time / 0.1);
	 vec2 offset = vec2(1.0)/vec2(8,4);
	 vec2 texCoordBase = texCoord_v / vec2(8,4);
	 vec2 texCoord = vec2(frame % 8, 3 - (frame / 8)) * offset+texCoordBase;

  return texCoord;
}

void main() {  
  vec2 texCoord = texturePosition();
  color_f = texture(texSampler, texCoord);
}
