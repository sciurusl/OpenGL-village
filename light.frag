#version 140

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection

  bool  useTexture;         // defines whether the texture is used or not
};

uniform sampler2D texSampler;  // sampler for the texture access

uniform Material material;     // current material

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

uniform mat4 PVMmatrix;     // Projection * View * Model  --> model to clip coordinates
uniform mat4 Pmatrix;     // Projection * View * Model  --> model to clip coordinates
uniform mat4 Vmatrix;       // View                       --> world to eye coordinates
uniform mat4 Mmatrix;       // Model                      --> model to world coordinates
uniform mat4 normalMatrix;

smooth in vec3 thePosition;
smooth in vec3 theNormal;
smooth in vec4 theColor;
smooth in vec3 pointLightPos;
smooth in vec3 pointLightPosRef;
smooth in vec3 refDirection;

smooth in vec4 color_v;        // incoming fragment color (includes lighting)
smooth in vec2 texCoord_v;     // fragment texture coordinates
out vec4       color_f;        // outgoing fragment color

uniform bool fog;
uniform bool cube;
in float visibility;

uniform float time;
uniform bool fastenTime;
float sunSpeed = 0.05f;

vec4 getColor(vec3 pos, vec3 rayDir, vec3 ambient, vec3 diffuse, vec3 specular, Material material, vec3 fragPosition, vec3 fragNormal){

  vec3 R = reflect(-rayDir, fragNormal);
  vec3 V = normalize(-fragPosition);
  float NdotL = max(0.0, dot(fragNormal, rayDir));
  float RdotV = max(0.0, dot(R, V));

	return vec4(material.ambient * ambient + material.diffuse * diffuse * NdotL + material.specular * specular * pow(RdotV, material.shininess),1.0f);
	
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
  float distance = length(pos - fragPosition);
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
  float distance = length(pos - fragPosition);
  rayDir = normalize(rayDir);
	vec4 color =  getColor(pos,rayDir,light.ambient, light.diffuse,light.specular,material,fragPosition, fragNormal);
  float attenuationFactor = 1.5f;
  if(distance>1)
	attenuationFactor = 1.0f/(light.attenuation.x + light.attenuation.y*distance + light.attenuation.z*distance*distance);

  return color*attenuationFactor;
}

void main() {
	
	vec3 L = normalize(pointLightPos-thePosition);
	if(directionalLight.shines)
	color_f = color_v;
	if(!fastenTime)
		color_f = color_f + getDirLight(directionalLight, material, thePosition,theNormal);
	color_f = color_f + getPointLight(pointLight, material, thePosition, theNormal);
	if(spotLight.turnedOn)
		color_f = color_f + getSpotLight(spotLight, material, thePosition, theNormal);
	if(cube)
		color_f = vec4(0.7, 0.7, 0.7, 1);
	if(material.useTexture)
		color_f =  color_f * texture(texSampler, texCoord_v);

	if(fog)
		color_f = mix(vec4(vec3(0.5, 0.5, 0.5), 1.0), color_f, visibility);
}
