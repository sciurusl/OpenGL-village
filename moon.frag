#version 140

uniform sampler2D texSampler;  // sampler for texture access

smooth in vec2 texCoord_v; 
uniform float time;
out vec4 color_f;              // outgoing fragment color

void main() {
float aSin = sin(mod(time, 360));
float aCos = cos(mod(time, 360));
mat3 transMat = mat3(1,0,0,0,1,0, 0.5, 0.5, 1);
mat3 transMatBack = mat3(1,0,0,0,1,0, -0.5, -0.5, 1);
mat3 rotMat      = mat3(aCos, aSin, 0, -aSin, aCos, 0, 0, 0, 1);
vec3 st = vec3(texCoord_v,1);
  //st -= vec2(0.5);
  //st = rotMat*st; 
//st += vec2(0.5);
st = transMat*rotMat*transMatBack*st;
  color_f = texture(texSampler, st.xy);
}
