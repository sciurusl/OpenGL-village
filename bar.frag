#version 140

uniform float time;
uniform int numOfTexture;
uniform sampler2D texSampler; 
uniform sampler2D texSampler2; 
uniform sampler2D texSampler3; 
uniform sampler2D texSampler4; 
smooth in vec2 texCoord_v; 
out vec4 color_f; 

void main() {
	if(numOfTexture==0)
		color_f = texture(texSampler, texCoord_v);
	if(numOfTexture==1)
		color_f = texture(texSampler2, texCoord_v);
	if(numOfTexture==2)
		color_f = texture(texSampler3, texCoord_v);
	if(numOfTexture==3)
		color_f = texture(texSampler4, texCoord_v);
}
