#version 140

uniform mat4 PVMmatrix;     // Projection * View * Model --> model to clip coordinates
uniform float time;         // used for simulation of moving lights (such as sun)

in vec3 position;           // vertex position in world space
in vec2 texCoord;           // incoming texture coordinates

smooth out vec2 texCoord_v; // outgoing texture coordinates

float decay = 0.5;

void main() {
  gl_Position = PVMmatrix  * vec4(position, 1.0);
  texCoord_v = texCoord;// + offset;
}
