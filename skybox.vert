#version 140

uniform mat4 inversePVmatrix;
in vec2 screenCoord;
out vec3 texCoord_v;
out vec4 color_f;

void main() {
	vec4 farplaneCoord = vec4(screenCoord, 0.9999, 1.0);
	vec4 worldViewCoord = inversePVmatrix * farplaneCoord;
	texCoord_v = worldViewCoord.xyz / worldViewCoord.w;
	color_f = vec4(0.0, 0.0, 1.0, 1.0);
	gl_Position = farplaneCoord;
}