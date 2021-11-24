//----------------------------------------------------------------------------------------
/**
 * \file    render_stuff.h
 * \author  veverlu4
 * \date    2020
 * \brief   Rendering stuff - drawing functions for models, etc..
 */
 //----------------------------------------------------------------------------------------

#ifndef __RENDER_STUFF_H
#define __RENDER_STUFF_H

#include "data.h"

typedef struct MeshGeometry {
  GLuint        vertexBufferObject;   // identifier for the vertex buffer object
  GLuint        elementBufferObject;  // identifier for the element buffer object
  GLuint        vertexArrayObject;    // identifier for the vertex array object
  unsigned int  numTriangles;         // number of triangles in the mesh
  // material
  glm::vec3     ambient;
  glm::vec3     diffuse;
  glm::vec3     specular;
  float         shininess;
  GLuint        texture;
  GLuint        textureMap;
  GLuint        textureDirt;
  GLuint        textureRock;
  GLuint        texture2;
  GLuint        texture3;
  GLuint        texture4;

} MeshGeometry;

// parameters of individual objects in the scene (e.g. position, size, speed, etc.)
typedef struct Object {
  glm::vec3 position;
  glm::vec3 direction;
  float     speed;
  float     size;

  bool destroyed;

  float startTime;
  float currentTime;

} Object;

typedef struct HumanObject : public Object {

  float viewAngle; // in degrees
  float height;
  float width;
  float length;

} HumanObject;

typedef struct CottageObject : public Object {
	float height;
	float width;
	float length;
} CottageObject;

typedef struct LogObject : public Object {

} LogObject;

typedef struct TreeObject : public Object {
	float height;
	float width;
	float length;
} TreeObject;

typedef struct BugObject : public Object {
	glm::vec3 initPosition;
} BugObject;

typedef struct StoneObject : public Object {

} StoneObject;

typedef struct FireObject : public Object {

	int    textureFrames;
	float  frameDuration;

} FireObject;

typedef struct StatusBarObject : public Object {
	int numOfTexture;
	LogObject *log;
} StatusBar;

typedef struct MoonObject : public Object {

} CloudObject;

typedef struct GlassObject : public Object {
	float height;
	float width;
	float length;
} GlassObject;


typedef struct TerrainObject : public Object {
	int    textureFrames;
	float  frameDuration;
	glm::vec3 refPos;

} TerrainObject;

typedef struct CatObject : public Object {
} CatObject;

typedef struct _commonShaderProgram {
  GLuint program;         
  GLint posLocation;       
  GLint colorLocation;     
  GLint normalLocation;    
  GLint texCoordLocation;  

  GLint PVMmatrixLocation;   
  GLint VmatrixLocation;      
  GLint MmatrixLocation;      
  GLint PmatrixLocation;     
  GLint normalMatrixLocation; 
  GLint skyColorLocation;

  GLint timeLocation;         

  GLint diffuseLocation;    
  GLint ambientLocation;   
  GLint specularLocation;  
  GLint shininessLocation;  

  GLint diffuseSpotLight;
  GLint ambientSpotLight;
  GLint specularSpotLight;
  GLint positionSpotLight;
  GLint attenuationSpotLight;
  GLint turnedOn;

  GLint fastenTimeLocation;
  GLint diffuseDirectionalLight;
  GLint ambientDirectionalLight;
  GLint specularDirectionalLight;
  GLint direction;

  GLint diffusePointLight;
  GLint ambientPointLight;
  GLint specularPointLight;
  GLint positionPointLight;
  GLint attenuationPointLight;
  GLint shines;
  GLint cutOff;
  GLint exponent;
  GLint effect;
  GLint directionPointLight;

  GLint useTextureLocation; 
  GLint texSamplerLocation; 

  GLint reflectorPositionLocation; 
  GLint reflectorDirectionLocation; 
} SCommonShaderProgram;

typedef struct terrainShaderProgram {
	GLuint program;              // = 0;

	GLint posLocation;           // = -1;
	GLint normalLocation;
	GLint texCoordLocation;      // = -1;

	GLint PVMmatrixLocation;     // = -1;
	GLint VmatrixLocation;       // = -1;
	GLint MmatrixLocation;       // = -1;
	GLint normalMatrixLocation; 

	GLint fastenTimeLocation;
	GLint timeLocation;         // = -1;

	GLint light;

	GLint diffuseLocation;    // = -1;
	GLint ambientLocation;    // = -1;
	GLint specularLocation;   // = -1;
	GLint shininessLocation;

	GLint texSamplerLocation;    // = -1;
	GLint texSamplerLocationDirt;    // = -1;
	GLint texSamplerLocationRock;    // = -1;
	GLint texSamplerLocationMap;

	GLint frameDurationLocation; // =

	GLint diffuseSpotLight;
	GLint ambientSpotLight;
	GLint specularSpotLight;
	GLint positionSpotLight;
	GLint attenuationSpotLight;
	GLint turnedOn;

	GLint diffuseDirectionalLight;
	GLint ambientDirectionalLight;
	GLint specularDirectionalLight;
	GLint direction;

	GLint diffusePointLight;
	GLint ambientPointLight;
	GLint specularPointLight;
	GLint positionPointLight;
	GLint attenuationPointLight;
	GLint shines;
	GLint cutOff;
	GLint exponent;
	GLint effect;
	GLint directionPointLight;
} TerrainShaderProgram;

typedef struct SkyboxShaderProgram {
	// identifier for the shader program
	GLuint program;                 // = 0;
	// vertex attributes locations
	GLint screenCoordLocation;      // = -1;
	GLint timeLocation;
	GLint isFog;
	GLint fastenTimeLocation;
	// uniforms locations
	GLint inversePVmatrixLocation; // = -1;
	GLint skyboxSamplerLocation;    // = -1;
} SkyboxShaderProgram;

typedef struct GlassShaderProgram{
	GLuint program;              // = 0;
  // vertex attributes locations
	GLint posLocation;           // = -1;
	GLint normalLocation;
	GLint texCoordLocation;      // = -1;
	// uniforms locations
	GLint PVMmatrixLocation;     // = -1;
	GLint VmatrixLocation;       // = -1;
	GLint MmatrixLocation;       // = -1;
	GLint normalMatrixLocation;              // = 0;
	// vertex attributes locations
	GLint screenCoordLocation;      // = -1;
	// uniforms locations
	GLint inversePVmatrixLocation; // = -1;
	GLint skyboxSamplerLocation;    // = -1;
} GlassShaderProgram;


void drawHuman(HumanObject* spaceShip, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawTree(TreeObject* tree, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawCottage(CottageObject* cottage, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawGlass(GlassObject* glass, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawLog(LogObject* log, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawBug(BugObject* bug, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawStone(StoneObject* stone, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawFire(FireObject* fire, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawStatusBar(StatusBarObject* statusBar, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawMoon(MoonObject* cloud, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawSkybox(const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawCat(CatObject* cat, const glm::mat4 & viewMatrix, const glm::mat4 & projectionMatrix);
void drawTerrain(TerrainObject* missile, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, glm::vec3 pos, glm::vec3 dir, bool fog);

void initializeShaderPrograms();
void cleanupShaderPrograms();

void initializeModels();
void cleanupModels();

#endif // __RENDER_STUFF_H
