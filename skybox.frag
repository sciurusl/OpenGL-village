#version 140

uniform samplerCube skyboxSampler;
in vec3 texCoord_v;
uniform float time;
out vec4 color_f;
uniform bool isFog;
float var;
uniform bool fastenTime;

void main() {
	float pct = 0;
	if (fastenTime)
		pct = 1;
	else{
		var = 40;
		pct = abs(sin((time-25)/var));
	}
	color_f = texture(skyboxSampler, texCoord_v);
	color_f = mix(color_f, vec4(0.0,0.0, 0.0, 1), pct);
	if(isFog)
		color_f = vec4(0.5, 0.5,0.5, 1.0);
}