#version 140

uniform mat4 PVMmatrix;      // A constant representing the combined model/view/projection matrix.
uniform mat4 Mmatrix;       // A constant representing the combined model/view matrix.
uniform mat4 Vmatrix;     
uniform mat4 normalMatrix;

in vec4 a_Position;     // Per-vertex position information we will pass in.
in vec4 a_Color;        // Per-vertex color information we will pass in.
in vec3 a_Normal;       // Per-vertex normal information we will pass in.
 
out vec3 v_Position;       // This will be passed into the fragment shader.
out vec4 v_Color;          // This will be passed into the fragment shader.
out vec3 v_Normal;         // This will be passed into the fragment shader.
 
 uniform sampler2D texSampler;

in vec2 texCoord;

smooth out vec2 texCoord_v;

uniform bool fog;
out float visibility;

float density = 0.5;
float gradient = 1.5;
smooth out vec3 thePosition;
smooth out float distanceVert;

void main()
{
visibility = 0.01;

    v_Position = vec3(Vmatrix*Mmatrix * a_Position);
	vec3 globalAmbientLight = vec3(0.4f);
	vec3 ambient = vec3(0.9f, 0.9f, 0.9f);
	vec4 outputColor = vec4(vec3(0.0f),1.0f);
    v_Color = outputColor;
	
   
    v_Normal =  normalize( (Vmatrix * normalMatrix * vec4(a_Normal, 0.0)).xyz);
	thePosition = v_Position;


   
    gl_Position = PVMmatrix * a_Position;
	texCoord_v = texCoord;
	

	 vec4 distanceCam = Vmatrix* Mmatrix * a_Position;
	float distance  = length(distanceCam);
	distanceVert = distance;

	  visibility = exp(-pow((distance*density), gradient));
	  visibility = clamp(visibility,0.0, 1.0);
}