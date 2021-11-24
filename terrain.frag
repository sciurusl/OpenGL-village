#version 140

struct Material {      // structure that describes currently used material
  vec3  ambient;       // ambient component
  vec3  diffuse;       // diffuse component
  vec3  specular;      // specular component
  float shininess;     // sharpness of specular reflection
};

uniform Material material;

precision mediump float;       // Set the default precision to medium. We don't need as high of a
	                        // precision in the fragment shader.

 
in vec3 v_Position;       // Interpolated position for this fragment.
in vec4 v_Color;          // This is the color from the vertex shader interpolated across the
    
uniform mat4 PVMmatrix;      // A constant representing the combined model/view/projection matrix.
uniform mat4 Mmatrix;       // A constant representing the combined model/view matrix.
uniform mat4 Vmatrix;

smooth in vec3 thePosition;
in vec3 v_Normal;         // Interpolated normal for this fragment.
 
 in float visibility;
 uniform bool fog;
uniform sampler2D texSamplerBackground;
uniform sampler2D texSamplerDirt;
uniform sampler2D texSamplerRock;
uniform sampler2D texSamplerMap;

struct PointLight{
	vec3  ambient;       
	vec3  diffuse;      
	vec3  specular;     
	vec3  position;
	float cutOff;
	float exponent;
	float effect;
	vec3 attenuation;
	vec3  direction;
};

struct DirectionalLight{
	vec3  ambient;       
	vec3  diffuse;      
	vec3  specular;     
	vec3  direction;
	bool shines;
};

struct SpotLight{
	vec3  ambient;       
	vec3  diffuse;      
	vec3  specular;     
	vec3  position;
	bool turnedOn;
	vec3 attenuation;
};

uniform PointLight pointLight;
uniform DirectionalLight directionalLight;
uniform SpotLight spotLight;

uniform float time;
uniform bool fastenTime;
float sunSpeed = 0.05f;
smooth in float distanceVert;

smooth in vec2 texCoord_v;     // fragment texture coordinates
out vec4 color_f;  

vec4 getColor(vec3 pos, vec3 rayDir, vec3 ambient, vec3 diffuse, vec3 specular, Material material, vec3 fragPosition, vec3 fragNormal){

  vec3 R = reflect(-rayDir, fragNormal);
  vec3 V = normalize(-fragPosition);
  float NdotL = max(0.0, dot(fragNormal, rayDir));
  float RdotV = max(0.0, dot(R, V));

	return vec4(ambient + diffuse * NdotL + specular * pow(RdotV, material.shininess),1.0f);
	
}

vec4 getDirLight(DirectionalLight light, Material material, vec3 fragPosition, vec3 fragNormal) {
  vec3 pos = (Vmatrix * vec4(cos(time * sunSpeed), 0.0, sin(time * sunSpeed), 0.0)).xyz;

  vec3 rayDir = normalize(pos);
  vec4 color = getColor(pos,rayDir,light.ambient, light.diffuse,light.specular,material,fragPosition, fragNormal);
 
  return color;
}

vec4 getPointLight(PointLight light, Material material, vec3 fragPosition, vec3 fragNormal) {	

	vec3 pos = (Vmatrix * vec4(light.position, 1.0)).xyz;

 vec3 direction = normalize((Vmatrix * vec4(light.direction, 0.0)).xyz);

  vec3 rayDir = pos - fragPosition;
  float distance = distanceVert;
  distance = length(rayDir);
  rayDir = normalize(rayDir);

  vec4 color =  getColor(pos,rayDir,light.ambient, light.diffuse,light.specular,material,fragPosition, fragNormal);
  float angle = dot(-rayDir, direction);


  float attenuationFactor = 1.0f/(light.attenuation.x + light.attenuation.y*distance + light.attenuation.z*distance*distance);

  if(max(angle, 0) < light.cutOff)
    color = vec4(vec3(0.0),1);

  return color*attenuationFactor*pow(max(angle,0), light.exponent);
}

vec4 getSpotLight(SpotLight light, Material material, vec3 fragPosition, vec3 fragNormal) {	

vec3 pos = (Vmatrix * vec4(light.position, 1.0)).xyz;

  vec3 rayDir = pos - fragPosition;
  float distance = distanceVert;
  distance = length(rayDir);
  rayDir = normalize(rayDir);
	vec4 color =  getColor(pos,rayDir,light.ambient, light.diffuse,light.specular,material,fragPosition, fragNormal);
  float attenuationFactor = 1.5f;
	attenuationFactor = 1.0f/(light.attenuation.x + light.attenuation.y*distance + light.attenuation.z*distance);
	
  return color*attenuationFactor;
}

void main()
{
	
	vec4 colorMap = texture(texSamplerMap, texCoord_v/200);
	float backTexture = 1 - (colorMap.r + colorMap.g);
	vec4 backgroundColor = texture(texSamplerBackground, texCoord_v)*backTexture;
	vec4 dirtColor = texture(texSamplerDirt, texCoord_v)*colorMap.g;
	vec4 rockColor = texture(texSamplerRock, texCoord_v)*colorMap.r;

	vec4 totalColor = backgroundColor + dirtColor + rockColor;
	color_f =  v_Color;
	if(!fastenTime)
		color_f += getDirLight(directionalLight, material, thePosition,v_Normal);
	color_f +=  getPointLight(pointLight, material,thePosition,v_Normal);
	if(spotLight.turnedOn)
		color_f +=  getSpotLight(spotLight, material, thePosition,v_Normal);
	
	color_f = color_f *totalColor;
	if (fog)
		color_f = mix(vec4(vec3(0.5, 0.5, 0.5), 1.0), color_f, visibility);
}