#version 140

struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection

  bool  useTexture;    // defines whether the texture is used or not
};

// warning: sampler inside the Material struct can cause problems -> so its outside
uniform sampler2D texSampler;  // sampler for the texture access

struct Light {         // structure describing light parameters
  vec3  ambient;       // intensity & color of the ambient component
  vec3  diffuse;       // intensity & color of the diffuse component
  vec3  specular;      // intensity & color of the specular component
  vec3  position;      // light position
  vec3  spotDirection; // spotlight direction
  float spotCosCutOff; // cosine of the spotlight's half angle
  float spotExponent;  // distribution of the light energy within the reflector's cone (center->cone's edge)
};

in vec3 position;           // vertex position in world space
in vec3 normal;             // vertex normal
in vec2 texCoord;           // incoming texture coordinates

uniform bool fog;
uniform bool cat;
uniform bool cube;

uniform float time;         // time used for simulation of moving lights (such as sun)
uniform Material material;  // current material

uniform mat4 PVMmatrix;     // Projection * View * Model  --> model to clip coordinates
uniform mat4 Pmatrix;     // Projection * View * Model  --> model to clip coordinates
uniform mat4 Vmatrix;       // View                       --> world to eye coordinates
uniform mat4 Mmatrix;       // Model                      --> model to world coordinates
uniform mat4 normalMatrix;  // inverse transposed Mmatrix


uniform vec3 reflectorPosition;   // reflector position (world coordinates)
uniform vec3 reflectorDirection;  // reflector direction (world coordinates)

smooth out vec2 texCoord_v;  // outgoing texture coordinates
smooth out vec4 color_v;       // outgoing fragment color

smooth out vec3 thePosition;
smooth out vec4 theColor;
smooth out vec3 theNormal;
smooth out vec3 pointLightPos;
smooth out vec3 pointLightPosRef;
smooth out vec3 refDirection;


out float visibility;
float density = 0.5;
float gradient = 1.5;

void main() {

  visibility = 0.01;

  vec3 vertexPosition = (Vmatrix * Mmatrix * vec4(position, 1.0)).xyz;         // vertex in eye coordinates
  vec3 vertexNormal   = normalize( (Vmatrix * normalMatrix * vec4(normal, 0.0) ).xyz);   // normal in eye coordinates by NormalMatrix

  vec4 outputColor = vec4(0.0, 0.0, 0.0, 1.0f);

  if(cat){
		float pct = abs(sin(time/3));
		gl_Position = PVMmatrix * vec4(position*mix(1, 1.05, pct), 1);
	}
	else
		gl_Position = PVMmatrix * vec4(position, 1);   // out:v vertex in clip coordinates

  thePosition = vertexPosition;
  theColor = outputColor;
  theNormal = vertexNormal;

  vec4 distanceCam = Vmatrix* Mmatrix * vec4(position, 1.0);
  float distance  = length(distanceCam);


  visibility = exp(-pow((distance*density), gradient));
  visibility = clamp(visibility,0.0, 1.0);
  
  color_v = outputColor;
  texCoord_v = texCoord;
}
