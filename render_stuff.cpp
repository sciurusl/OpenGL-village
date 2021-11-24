//----------------------------------------------------------------------------------------
/**
 * \file    render_stuff.cpp
 * \author  veverlu4
 * \date    2020
 * \brief   Rendering stuff - drawing functions for models, etc..
 */
 //----------------------------------------------------------------------------------------

#include <iostream>
#include "pgr.h"
#include "render_stuff.h"
#include "data.h"
#include "Terrain.h"
#include "spline.h"
#include <time.h>
#include <fstream>
#include <map>
#include <algorithm>

MeshGeometry* asteroidGeometry = NULL;
MeshGeometry* treeGeometry = NULL;
MeshGeometry* cottageGeometry = NULL;
MeshGeometry* glassGeometry = NULL;
MeshGeometry* humanGeometry = NULL;
MeshGeometry* ufoGeometry = NULL;
MeshGeometry* missileGeometry = NULL;
MeshGeometry* terrainGeometry = NULL;
MeshGeometry* moonGeometry = NULL;
MeshGeometry* explosionGeometry = NULL;
MeshGeometry* fireGeometry = NULL;
MeshGeometry* statusBarGeometry = NULL;
MeshGeometry* skyboxGeometry = NULL;
MeshGeometry* catGeometry = NULL;
MeshGeometry* logGeometry = NULL;
MeshGeometry* bugGeometry = NULL;
MeshGeometry* stoneGeometry = NULL;

const char* TREE_MODEL_NAME = "tree/10445_Oak_Tree_v1_max2010_iteration-1.obj";
const char* BUG_MODEL_NAME = "data/10024_Grassshoper_v1_L3.obj";
const char* COTTAGE_MODEL_NAME = "data/dom 1.obj";
const char* GLASS_MODEL_NAME = "data/ghostdog.obj";
const char* LOG_MODEL_NAME = "data/12304_Wooden_Log_Stack_v2_l3.obj";
const char* HUMAN_MODEL_NAME = "data/Cinto.fbx";
const char* FIRE_TEXTURE_NAME = "data/flames.jpg";
const char* STATUS_BAR_TEXTURE_NAME1 = "data/fireBar1.png";
const char* STATUS_BAR_TEXTURE_NAME2 = "data/fireBar2.png";
const char* STATUS_BAR_TEXTURE_NAME3 = "data/fireBar3.png";
const char* STATUS_BAR_TEXTURE_NAME4 = "data/fireBar4.png";
const char* TERRAIN_TEXTURE_NAME = "data/terrain.jpg";
const char* TERRAIN_TEXTURE_MAP_NAME = "data/map.png";
const char* TERRAIN_DIRT_TEXTURE_NAME = "data/dirt.png";
const char* TERRAIN_ROCK_TEXTURE_NAME = "data/road.png";
const char* MOON_TEXTURE_NAME = "data/moon.png";
const char* SKYBOX_CUBE_TEXTURE_FILE_PREFIX = "data/skybox_new/clouds1";
const char* CAT_TEXTURE = "data/Cat_diffuse.jpg";
const char* CAT_OBJ = "data/12221_Cat_v1_l3.obj";
const char* suffixes[] = { "east", "west", "north", "south", "up", "down" };

SCommonShaderProgram shaderProgram;
TerrainShaderProgram terrainShaderProgram;
SkyboxShaderProgram skyboxShaderProgram;
GlassShaderProgram glassShaderProgram;

bool useLighting = false;

const int num_of_sides = 1;
int count_vertices = 0;
float terrainVertices[3 * 2 * 3 * TERRAIN_HEIGHT * TERRAIN_WIDTH * num_of_sides] = {};
unsigned int terrainIndices[TERRAIN_HEIGHT * TERRAIN_WIDTH * 2 * 3 * num_of_sides * 3];
int terrainTrianglesCount;

struct fireShaderProgram {
	// identifier for the shader program
	GLuint program;              // = 0;
	// vertex attributes locations
	GLint posLocation;           // = -1;
	GLint texCoordLocation;      // = -1;
	// uniforms locations
	GLint PVMmatrixLocation;     // = -1;
	GLint VmatrixLocation;       // = -1;
	GLint timeLocation;          // = -1;
	GLint texSamplerLocation;    // = -1;
	GLint frameDurationLocation; // = -1;

} fireShaderProgram;


struct barShaderProgram {
	// identifier for the shader program
	GLuint program;              // = 0;
	// vertex attributes locations
	GLint posLocation;           // = -1;
	GLint texCoordLocation;      // = -1;
	// uniforms locations
	GLint PVMmatrixLocation;     // = -1;
	GLint VmatrixLocation;       // = -1;
	GLint timeLocation;          // = -1;
	GLint texSamplerLocation;    // = -1;
	GLint texSamplerLocation2;    // = -1;
	GLint texSamplerLocation3;    // = -1;
	GLint texSamplerLocation4;    // = -1;

	GLint numberOfTexture;

} barShaderProgram;

struct moonShaderProgram {
	// identifier for the shader program
	GLuint program;           // = 0;
	// vertex attributes locations
	GLint posLocation;        // = -1;
	GLint texCoordLocation;   // = -1;
	// uniforms locations
	GLint PVMmatrixLocation;  // = -1;

	GLint timeLocation;       // = -1;
	GLint texSamplerLocation; // = -1;
} moonShaderProgram;

void setTransformUniforms(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(shaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniformMatrix4fv(shaderProgram.PmatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniformMatrix4fv(shaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "cube");
	glUniform1i(myUniformLocation, 0);

	// just take 3x3 rotation part of the modelMatrix
	// we presume the last row contains 0,0,0,1
	const glm::mat4 modelRotationMatrix = glm::mat4(
		modelMatrix[0],
		modelMatrix[1],
		modelMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));

	//or an alternative single-line method: 
	//glm::mat4 normalMatrix = glm::transpose(glm::inverse(glm::mat4(glm::mat3(modelRotationMatrix))));

	glUniformMatrix4fv(shaderProgram.normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));  // correct matrix for non-rigid transform
}

void setMaterialUniforms(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, GLuint texture) {

	glUniform3fv(shaderProgram.diffuseLocation, 1, glm::value_ptr(diffuse));  // 2nd parameter must be 1 - it declares number of vectors in the vector array
	glUniform3fv(shaderProgram.ambientLocation, 1, glm::value_ptr(ambient));
	glUniform3fv(shaderProgram.specularLocation, 1, glm::value_ptr(specular));
	glUniform1f(shaderProgram.shininessLocation, shininess);

	if (texture != 0) {
		glUniform1i(shaderProgram.useTextureLocation, 1);  // do texture sampling
		glUniform1i(shaderProgram.texSamplerLocation, 0);  // texturing unit 0 -> samplerID   [for the GPU linker]
		glActiveTexture(GL_TEXTURE0 + 0);                  // texturing unit 0 -> to be bound [for OpenGL BindTexture]
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	else {
		glUniform1i(shaderProgram.useTextureLocation, 0);  // do not sample the texture
	}
}

void drawHuman(HumanObject* human, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(shaderProgram.program);

	// prepare modeling transform matrix
	glm::mat4 modelMatrix = alignObject(human->position, -human->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	//modelMatrix = glm::rotate(modelMatrix, 3.14f, glm::vec3(0, 1, 1));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(human->size, human->size, human->size));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "cat");
	glUniform1i(myUniformLocation, 0);

	setMaterialUniforms(
		humanGeometry->ambient,
		humanGeometry->diffuse,
		humanGeometry->specular,
		humanGeometry->shininess,
		humanGeometry->texture
	);

	// draw geometry
	glBindVertexArray(humanGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, humanGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	return;
}

void drawTree(TreeObject* tree, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(tree->position.x, tree->position.y, tree->position.z));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(tree->size));
	//modelMatrix = glm::rotate(modelMatrix, 3.14f, glm::vec3(0, 1, 1));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
	GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "cat");
	glUniform1i(myUniformLocation, 0);

	setMaterialUniforms(
		treeGeometry->ambient,
		treeGeometry->diffuse,
		treeGeometry->specular,
		treeGeometry->shininess,
		treeGeometry->texture
	);


	// draw geometry
	glBindVertexArray(treeGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, treeGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	return;
}

void drawCottage(CottageObject* cottage, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = alignObject(cottage->position, cottage->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(cottage->size));
	//modelMatrix = glm::rotate(modelMatrix, 3.14f, glm::vec3(0, 1, 1));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
	GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "cat");
	glUniform1i(myUniformLocation, 0);

	setMaterialUniforms(
		cottageGeometry->ambient,
		cottageGeometry->diffuse,
		cottageGeometry->specular,
		cottageGeometry->shininess,
		cottageGeometry->texture
	);

	// draw geometry
	glBindVertexArray(cottageGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, cottageGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	return;
}

void drawGlass(GlassObject* glass, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(glass->position.x, glass->position.y, glass->position.z - 0.1));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(glass->size));
	modelMatrix = glm::rotate(modelMatrix, 3.14f, glm::vec3(0, 1, 1));

	glm::mat4 PVM = projectionMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(shaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniformMatrix4fv(shaderProgram.PmatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniformMatrix4fv(shaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(shaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	glUniformMatrix4fv(shaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "cube");
	glUniform1i(myUniformLocation, 1);

	const glm::mat4 modelRotationMatrix = glm::mat4(
		modelMatrix[0],
		modelMatrix[1],
		modelMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));

	glUniformMatrix4fv(shaderProgram.normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	setMaterialUniforms(
		glassGeometry->ambient,
		glassGeometry->diffuse,
		glassGeometry->specular,
		glassGeometry->shininess,
		glassGeometry->texture
	);

	// draw geometry
	glBindVertexArray(glassGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, glassGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_BLEND);
	return;
}

void drawLog(LogObject* log, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(log->position.x, log->position.y, log->position.z));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(log->size));
	//modelMatrix = glm::rotate(modelMatrix, 3.14f, glm::vec3(0, 1, 1));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);
	GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "cat");
	glUniform1i(myUniformLocation, 0);

	setMaterialUniforms(
		logGeometry->ambient,
		logGeometry->diffuse,
		logGeometry->specular,
		logGeometry->shininess,
		logGeometry->texture
	);

	// draw geometry
	glBindVertexArray(logGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, logGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	return;
}

void drawBug(BugObject* bug, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = alignObject(bug->position, bug->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(bug->size));
	modelMatrix = glm::rotate(modelMatrix, 3.14f, glm::vec3(0, 1, 1));

	// send matrices to the vertex & fragment shader
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "cat");
	glUniform1i(myUniformLocation, 0);

	setMaterialUniforms(
		bugGeometry->ambient,
		bugGeometry->diffuse,
		bugGeometry->specular,
		bugGeometry->shininess,
		bugGeometry->texture
	);

	// draw geometry
	glBindVertexArray(bugGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, bugGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	return;
}


void drawTerrain(TerrainObject* terrain, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, glm::vec3 pos, glm::vec3 dir, bool fog) {

	//glUseProgram(terrainShaderProgram.program);
	glUseProgram(terrainShaderProgram.program);

	// align missile coordinate system to match its position and direction - see alignObject() function

	glm::mat4 Mmatrix = alignObject(terrain->position, terrain->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	Mmatrix = glm::scale(Mmatrix, glm::vec3(terrain->size));
	//matrix = matrix * billboardRotationMatrix; // make billboard to face the camera

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * Mmatrix;

	glUniformMatrix4fv(terrainShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));

	glUniformMatrix4fv(terrainShaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniformMatrix4fv(terrainShaderProgram.MmatrixLocation, 1, GL_FALSE, glm::value_ptr(Mmatrix));

	const glm::mat4 modelRotationMatrix = glm::mat4(
		Mmatrix[0],
		Mmatrix[1],
		Mmatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelRotationMatrix));

	glUniformMatrix4fv(terrainShaderProgram.normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	if (fog) {
		GLint myUniformLocation = glGetUniformLocation(terrainShaderProgram.program, "fog");
		//std::cout << myUniformLocation << "myUnifLoc\n";
		if (myUniformLocation != -1)
			glUniform1i(myUniformLocation, 1);
	}
	else {
		GLint myUniformLocation = glGetUniformLocation(terrainShaderProgram.program, "fog");
		//std::cout << myUniformLocation << "myUnifLoc\n";
		if (myUniformLocation != -1)
			glUniform1i(myUniformLocation, 0);
	}

	glUniform1i(terrainShaderProgram.texSamplerLocation, 0);
	glUniform3fv(terrainShaderProgram.light, 1, glm::value_ptr(terrain->refPos));
	//glBindVertexArray(terrainGeometry->vertexArrayObject);

	glBindVertexArray(terrainGeometry->vertexArrayObject);

	glUniform3fv(terrainShaderProgram.diffuseLocation, 1, glm::value_ptr(terrainGeometry->diffuse));  // 2nd parameter must be 1 - it declares number of vectors in the vector array
	glUniform3fv(terrainShaderProgram.ambientLocation, 1, glm::value_ptr(terrainGeometry->ambient));
	glUniform3fv(terrainShaderProgram.specularLocation, 1, glm::value_ptr(terrainGeometry->specular));
	glUniform1f(terrainShaderProgram.shininessLocation, terrainGeometry->shininess);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainGeometry->texture);
	glUniform1i(terrainShaderProgram.texSamplerLocationDirt, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, terrainGeometry->textureDirt);
	glUniform1i(terrainShaderProgram.texSamplerLocationRock, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, terrainGeometry->textureRock);
	glUniform1i(terrainShaderProgram.texSamplerLocationMap, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, terrainGeometry->textureMap);


	glDrawElements(GL_TRIANGLES, terrainGeometry->numTriangles * 3, GL_UNSIGNED_INT, nullptr);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	CHECK_GL_ERROR();
	int b = 0;
	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();
	return;
}

void drawCat(CatObject* cat, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(cat->position.x, cat->position.y, cat->position.z - 0.2));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(cat->size / 8));
	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "cat");
	glUniform1i(myUniformLocation, 1);

	setMaterialUniforms(
		catGeometry->ambient,
		catGeometry->diffuse,
		catGeometry->specular,
		catGeometry->shininess,
		catGeometry->texture
	);


	// draw geometry
	glBindVertexArray(catGeometry->vertexArrayObject);
	//glDrawElements(GL_TRIANGLES, catGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3 * catGeometry->numTriangles);
	glBindVertexArray(0);
	glUseProgram(0);

	return;
}

void drawStone(StoneObject* stone, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(shaderProgram.program);

	glm::mat4 modelMatrix = alignObject(stone->position, stone->direction, glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(stone->size));

	setTransformUniforms(modelMatrix, viewMatrix, projectionMatrix);

	GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "cat");
	glUniform1i(myUniformLocation, 0);

	setMaterialUniforms(
		stoneGeometry->ambient,
		stoneGeometry->diffuse,
		stoneGeometry->specular,
		stoneGeometry->shininess,
		stoneGeometry->texture
	);


	// draw geometry
	glBindVertexArray(stoneGeometry->vertexArrayObject);
	glDrawElements(GL_TRIANGLES, stoneGeometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	return;
}

void drawFire(FireObject* fire, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(fireShaderProgram.program);

	glm::mat4 billboardRotationMatrix = glm::mat4(
		viewMatrix[0],
		viewMatrix[1],
		viewMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	// inverse view rotation
	billboardRotationMatrix = glm::transpose(billboardRotationMatrix);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), fire->position);
	matrix = glm::scale(matrix, glm::vec3(fire->size));
	matrix = matrix * billboardRotationMatrix; // make billboard to face the camera

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
	glUniformMatrix4fv(fireShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));  // model-view-projection
	glUniformMatrix4fv(fireShaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));   // view
	glUniform1f(fireShaderProgram.timeLocation, fire->currentTime - fire->startTime);
	glUniform1i(fireShaderProgram.texSamplerLocation, 0);
	glUniform1f(fireShaderProgram.frameDurationLocation, fire->frameDuration);

	glBindVertexArray(fireGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_2D, fireGeometry->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, fireGeometry->numTriangles);

	glBindVertexArray(0);
	glUseProgram(0);

	glDisable(GL_BLEND);

	return;
}

void drawStatusBar(StatusBarObject* statusBar, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(barShaderProgram.program);

	glm::mat4 billboardRotationMatrix = glm::mat4(
		viewMatrix[0],
		viewMatrix[1],
		viewMatrix[2],
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	);
	// inverse view rotation
	billboardRotationMatrix = glm::transpose(billboardRotationMatrix);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), statusBar->position);
	matrix = glm::scale(matrix, glm::vec3(statusBar->size));
	matrix = matrix * billboardRotationMatrix; // make billboard to face the camera

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
	glUniformMatrix4fv(barShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));  // model-view-projection
	glUniformMatrix4fv(barShaderProgram.VmatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));   // view
	glUniform1f(barShaderProgram.timeLocation, statusBar->currentTime - statusBar->startTime);
	glUniform1i(barShaderProgram.texSamplerLocation, 0);
	glUniform1i(barShaderProgram.numberOfTexture, statusBar->numOfTexture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, statusBarGeometry->texture);
	glUniform1i(barShaderProgram.texSamplerLocation2, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, statusBarGeometry->texture2);
	glUniform1i(barShaderProgram.texSamplerLocation3, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, statusBarGeometry->texture3);
	glUniform1i(barShaderProgram.texSamplerLocation4, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, statusBarGeometry->texture4);

	glBindVertexArray(statusBarGeometry->vertexArrayObject);
	//glBindTexture(GL_TEXTURE_2D, statusBarGeometry->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, statusBarGeometry->numTriangles);

	

	glBindVertexArray(0);
	glUseProgram(0);

	return;
}

void drawMoon(MoonObject* moon, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(moonShaderProgram.program);

	glm::mat4 matrix = glm::translate(glm::mat4(1.0f), moon->position);
	matrix = glm::scale(matrix, glm::vec3(moon->size));

	glm::mat4 PVMmatrix = projectionMatrix * viewMatrix * matrix;
	glUniformMatrix4fv(moonShaderProgram.PVMmatrixLocation, 1, GL_FALSE, glm::value_ptr(PVMmatrix));        // model-view-projection
	glUniform1f(moonShaderProgram.timeLocation, moon->currentTime - moon->startTime);
	glUniform1i(moonShaderProgram.texSamplerLocation, 0);

	glBindTexture(GL_TEXTURE_2D, moonGeometry->texture);
	glBindVertexArray(moonGeometry->vertexArrayObject);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, moonGeometry->numTriangles);

	CHECK_GL_ERROR();

	glBindVertexArray(0);
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	return;
}



void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {

	glUseProgram(skyboxShaderProgram.program);
	glm::mat4 matrix = projectionMatrix * viewMatrix;
	glm::mat4 viewRotation = viewMatrix;
	viewRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 inversePVmatrix = glm::inverse(projectionMatrix * viewRotation);

	glUniformMatrix4fv(skyboxShaderProgram.inversePVmatrixLocation, 1, GL_FALSE, glm::value_ptr(inversePVmatrix));
	glUniform1i(skyboxShaderProgram.skyboxSamplerLocation, 0);

	glBindVertexArray(skyboxGeometry->vertexArrayObject);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxGeometry->texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, skyboxGeometry->numTriangles + 2);

	glBindVertexArray(0);
	glUseProgram(0);
}

void cleanupShaderPrograms(void) {

	pgr::deleteProgramAndShaders(shaderProgram.program);

	pgr::deleteProgramAndShaders(fireShaderProgram.program);
	pgr::deleteProgramAndShaders(terrainShaderProgram.program);
	pgr::deleteProgramAndShaders(moonShaderProgram.program);
	pgr::deleteProgramAndShaders(skyboxShaderProgram.program);
}

void initializeShaderPrograms(void) {

	std::vector<GLuint> shaderList;
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "light.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "light.frag"));

	shaderProgram.program = pgr::createProgram(shaderList);

	// get vertex attributes locations, if the shader does not have this uniform -> return -1
	shaderProgram.posLocation = glGetAttribLocation(shaderProgram.program, "position");
	shaderProgram.normalLocation = glGetAttribLocation(shaderProgram.program, "normal");
	shaderProgram.texCoordLocation = glGetAttribLocation(shaderProgram.program, "texCoord");
	// get uniforms locations
	shaderProgram.PVMmatrixLocation = glGetUniformLocation(shaderProgram.program, "PVMmatrix");
	shaderProgram.PmatrixLocation = glGetUniformLocation(shaderProgram.program, "Pmatrix");
	shaderProgram.VmatrixLocation = glGetUniformLocation(shaderProgram.program, "Vmatrix");
	shaderProgram.MmatrixLocation = glGetUniformLocation(shaderProgram.program, "Mmatrix");
	shaderProgram.normalMatrixLocation = glGetUniformLocation(shaderProgram.program, "normalMatrix");
	shaderProgram.timeLocation = glGetUniformLocation(shaderProgram.program, "time");
	shaderProgram.fastenTimeLocation = glGetUniformLocation(shaderProgram.program, "fastenTime");
	// material
	shaderProgram.ambientLocation = glGetUniformLocation(shaderProgram.program, "material.ambient");
	shaderProgram.diffuseLocation = glGetUniformLocation(shaderProgram.program, "material.diffuse");
	shaderProgram.specularLocation = glGetUniformLocation(shaderProgram.program, "material.specular");
	shaderProgram.shininessLocation = glGetUniformLocation(shaderProgram.program, "material.shininess");
	// texture
	shaderProgram.texSamplerLocation = glGetUniformLocation(shaderProgram.program, "texSampler");
	shaderProgram.useTextureLocation = glGetUniformLocation(shaderProgram.program, "material.useTexture");
	// reflector
	shaderProgram.reflectorPositionLocation = glGetUniformLocation(shaderProgram.program, "reflectorPosition");
	shaderProgram.reflectorDirectionLocation = glGetUniformLocation(shaderProgram.program, "reflectorDirection");

	shaderList.clear();

	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "fire.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "fire.frag"));

	// create the program with two shaders
	fireShaderProgram.program = pgr::createProgram(shaderList);

	// get position and texture coordinates attributes locations
	fireShaderProgram.posLocation = glGetAttribLocation(fireShaderProgram.program, "position");
	fireShaderProgram.texCoordLocation = glGetAttribLocation(fireShaderProgram.program, "texCoord");
	// get uniforms locations
	fireShaderProgram.PVMmatrixLocation = glGetUniformLocation(fireShaderProgram.program, "PVMmatrix");
	fireShaderProgram.VmatrixLocation = glGetUniformLocation(fireShaderProgram.program, "Vmatrix");
	fireShaderProgram.timeLocation = glGetUniformLocation(fireShaderProgram.program, "time");
	fireShaderProgram.texSamplerLocation = glGetUniformLocation(fireShaderProgram.program, "texSampler");
	fireShaderProgram.frameDurationLocation = glGetUniformLocation(fireShaderProgram.program, "frameDuration");

	shaderList.clear();

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "bar.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "bar.frag"));

	// create the program with two shaders
	barShaderProgram.program = pgr::createProgram(shaderList);

	// get position and texture coordinates attributes locations
	barShaderProgram.posLocation = glGetAttribLocation(barShaderProgram.program, "position");
	barShaderProgram.texCoordLocation = glGetAttribLocation(barShaderProgram.program, "texCoord");
	// get uniforms locations
	barShaderProgram.PVMmatrixLocation = glGetUniformLocation(barShaderProgram.program, "PVMmatrix");
	barShaderProgram.VmatrixLocation = glGetUniformLocation(barShaderProgram.program, "Vmatrix");
	barShaderProgram.timeLocation = glGetUniformLocation(barShaderProgram.program, "time");
	barShaderProgram.texSamplerLocation = glGetUniformLocation(barShaderProgram.program, "texSampler");
	barShaderProgram.numberOfTexture = glGetUniformLocation(barShaderProgram.program, "numOfTexture");
	barShaderProgram.texSamplerLocation2 = glGetUniformLocation(barShaderProgram.program, "texSampler2");
	barShaderProgram.texSamplerLocation3 = glGetUniformLocation(barShaderProgram.program, "texSampler3");
	barShaderProgram.texSamplerLocation4 = glGetUniformLocation(barShaderProgram.program, "texSampler4");
	

	shaderList.clear();

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "terrain.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "terrain.frag"));

	terrainShaderProgram.program = pgr::createProgram(shaderList);

	terrainShaderProgram.posLocation = glGetAttribLocation(terrainShaderProgram.program, "a_Position");
	terrainShaderProgram.normalLocation = glGetAttribLocation(terrainShaderProgram.program, "a_Normal");
	terrainShaderProgram.texCoordLocation = glGetAttribLocation(terrainShaderProgram.program, "texCoord");
	terrainShaderProgram.timeLocation = glGetUniformLocation(terrainShaderProgram.program, "time");
	terrainShaderProgram.fastenTimeLocation = glGetUniformLocation(terrainShaderProgram.program, "fastenTime");

	terrainShaderProgram.PVMmatrixLocation = glGetUniformLocation(terrainShaderProgram.program, "PVMmatrix");
	terrainShaderProgram.normalMatrixLocation = glGetUniformLocation(terrainShaderProgram.program, "normalMatrix");
	terrainShaderProgram.VmatrixLocation = glGetUniformLocation(terrainShaderProgram.program, "Vmatrix");
	terrainShaderProgram.MmatrixLocation = glGetUniformLocation(terrainShaderProgram.program, "Mmatrix");

	terrainShaderProgram.ambientLocation = glGetUniformLocation(terrainShaderProgram.program, "material.ambient");
	terrainShaderProgram.diffuseLocation = glGetUniformLocation(terrainShaderProgram.program, "material.diffuse");
	terrainShaderProgram.specularLocation = glGetUniformLocation(terrainShaderProgram.program, "material.specular");
	terrainShaderProgram.shininessLocation = glGetUniformLocation(terrainShaderProgram.program, "material.shininess");

	terrainShaderProgram.texSamplerLocation = glGetUniformLocation(terrainShaderProgram.program, "texSamplerBackground");
	terrainShaderProgram.texSamplerLocationDirt = glGetUniformLocation(terrainShaderProgram.program, "texSamplerDirt");
	terrainShaderProgram.texSamplerLocationRock = glGetUniformLocation(terrainShaderProgram.program, "texSamplerRock");
	terrainShaderProgram.texSamplerLocationMap = glGetUniformLocation(terrainShaderProgram.program, "texSamplerMap");
	terrainShaderProgram.light = glGetUniformLocation(terrainShaderProgram.program, "lightPos");
	shaderList.clear();

	CHECK_GL_ERROR();

	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "moon.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "moon.frag"));

	moonShaderProgram.program = pgr::createProgram(shaderList);

	moonShaderProgram.posLocation = glGetAttribLocation(moonShaderProgram.program, "position");
	moonShaderProgram.texCoordLocation = glGetAttribLocation(moonShaderProgram.program, "texCoord");
	moonShaderProgram.PVMmatrixLocation = glGetUniformLocation(moonShaderProgram.program, "PVMmatrix");
	moonShaderProgram.timeLocation = glGetUniformLocation(moonShaderProgram.program, "time");
	moonShaderProgram.texSamplerLocation = glGetUniformLocation(moonShaderProgram.program, "texSampler");
	shaderList.clear();

	// push vertex shader and fragment shader
	shaderList.push_back(pgr::createShaderFromSource(GL_VERTEX_SHADER, skyboxFarPlaneVertexShaderSrc));
	shaderList.push_back(pgr::createShaderFromSource(GL_FRAGMENT_SHADER, skyboxFarPlaneFragmentShaderSrc));

	// create the program with two shaders
	skyboxShaderProgram.program = pgr::createProgram(shaderList);

	// handles to vertex attributes locations
	skyboxShaderProgram.screenCoordLocation = glGetAttribLocation(skyboxShaderProgram.program, "screenCoord");
	skyboxShaderProgram.timeLocation = glGetUniformLocation(skyboxShaderProgram.program, "time");
	skyboxShaderProgram.fastenTimeLocation = glGetUniformLocation(skyboxShaderProgram.program, "fastenTime");
	// get uniforms locations
	skyboxShaderProgram.skyboxSamplerLocation = glGetUniformLocation(skyboxShaderProgram.program, "skyboxSampler");
	skyboxShaderProgram.inversePVmatrixLocation = glGetUniformLocation(skyboxShaderProgram.program, "inversePVmatrix");
	shaderList.clear();

}

bool loadWithoutAssimp(const std::string& fileName, const std::string& textureName, SCommonShaderProgram& shaderL, MeshGeometry** geometry, int num_of_vertices_all, int num_of_polygons) {
	FILE* myfile;
	char name[sizeof(fileName) + 1];
	strcpy(name, &fileName[0]);
	fopen_s(&myfile, name, "rb");
	if (myfile == nullptr) {
		std::cout << "WARNING! File could not be loaded!\n";
		fclose(myfile);
		return false;
	}
	int num_of_vertices = 0;
	int num_of_texCoord = 0;
	int verticesAndNormals = 0;
	bool textures = false;
	std::vector<float> vertices;
	std::vector<float> verticesArray;
	std::vector<float> verticesTemp;
	std::vector<float> texCoords;
	int num_polyg = 0;
	char str[120];
	float vertex1, vertex2, vertex3;
	float* texTemp;
	float* normTemp;
	int f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12;
	int ret;
	std::map<std::tuple<int, int, int>, float> mapOfFaces;
	while (fscanf(myfile, "%s", str) != EOF) {
		if (strcmp(str, "v") == 0) {
			//std::cout << "only one char\n";
			fscanf(myfile, "%f %f %f", &vertex1, &vertex2, &vertex3);
			vertices.push_back(vertex1);
			vertices.push_back(vertex2);
			vertices.push_back(vertex3);
			num_of_vertices++;
		}
		else if (strcmp(str, "vn") == 0) {
			//std::cout << "got here\n";
			fscanf(myfile, " %f %f %f", &vertex1, &vertex2, &vertex3);
			vertices.push_back(vertex1);
			vertices.push_back(vertex2);
			vertices.push_back(vertex3);
			verticesAndNormals++;
		}
		else if (strcmp(str, "vt") == 0) {
			//std::cout << "and here\n";
			fscanf(myfile, " %f %f", &vertex1, &vertex2);
			texCoords.push_back(vertex1);
			texCoords.push_back(vertex2);
			num_of_texCoord++;
		}
		else if (strcmp(str, "f") == 0) {
			//std::cout << "f is here\n";

			fscanf(myfile, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &f1, &f2, &f3, &f4, &f5, &f6, &f7, &f8, &f9, &f10, &f11, &f12);
			int cnt = 2;
			f1 -= 1;
			f2 -= 1;
			f3 -= 1;
			f7 -= 1;
			f8 -= 1;
			f9 -= 1;
			f4 -= 1;
			f5 -= 1;
			f6 -= 1;
			f10 -= 1;
			f11 -= 1;
			f12 -= 1;
			for (int i = 0; i < cnt; i++) {

				verticesArray.push_back(vertices[f1 * 3]);
				verticesArray.push_back(vertices[f1 * 3 + 1]);
				verticesArray.push_back(vertices[f1 * 3 + 2]);
				verticesArray.push_back(texCoords[f2 * 2]);
				verticesArray.push_back(texCoords[f2 * 2 + 1]);
				verticesArray.push_back(vertices[num_of_vertices * 3 + f3 * 3]);
				verticesArray.push_back(vertices[num_of_vertices * 3 + f3 * 3 + 1]);
				verticesArray.push_back(vertices[num_of_vertices * 3 + f3 * 3 + 2]);



				verticesArray.push_back(vertices[f7 * 3]);
				verticesArray.push_back(vertices[f7 * 3 + 1]);
				verticesArray.push_back(vertices[f7 * 3 + 2]);
				verticesArray.push_back(texCoords[f8 * 2]);
				verticesArray.push_back(texCoords[f8 * 2 + 1]);
				verticesArray.push_back(vertices[num_of_vertices * 3 + f9 * 3]);
				verticesArray.push_back(vertices[num_of_vertices * 3 + f9 * 3 + 1]);
				verticesArray.push_back(vertices[num_of_vertices * 3 + f9 * 3 + 2]);

				if (i == 0) {

					int num = num_of_vertices * 3 + 2 * num_of_texCoord + f9 * 3 + 2;
					int size = vertices.size();
					verticesArray.push_back(vertices[f4 * 3]);
					verticesArray.push_back(vertices[f4 * 3 + 1]);
					verticesArray.push_back(vertices[f4 * 3 + 2]);
					verticesArray.push_back(texCoords[f5 * 2]);
					verticesArray.push_back(texCoords[f5 * 2 + 1]);
					verticesArray.push_back(vertices[num_of_vertices * 3 + f6 * 3]);
					verticesArray.push_back(vertices[num_of_vertices * 3 + f6 * 3 + 1]);
					verticesArray.push_back(vertices[num_of_vertices * 3 + f6 * 3 + 2]);
				}
				else {

					verticesArray.push_back(vertices[f10 * 3]);
					verticesArray.push_back(vertices[f10 * 3 + 1]);
					verticesArray.push_back(vertices[f10 * 3 + 2]);
					verticesArray.push_back(texCoords[f11 * 2]);
					verticesArray.push_back(texCoords[f11 * 2 + 1]);
					verticesArray.push_back(vertices[num_of_vertices * 3 + f12 * 3]);
					verticesArray.push_back(vertices[num_of_vertices * 3 + f12 * 3 + 1]);
					verticesArray.push_back(vertices[num_of_vertices * 3 + f12 * 3 + 2]);

				}
			}
			num_polyg++;
		}
	}

	float* arr = &verticesArray[0];

	*geometry = new MeshGeometry;
	(*geometry)->texture = pgr::createTexture(textureName);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

	//glBufferData(GL_ARRAY_BUFFER, num_of_vertices * 3 * sizeof(float)*2 + num_of_vertices * sizeof(float) * 2, arr, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, num_polyg * 8 * 3 * sizeof(float) * 2, arr, GL_STATIC_DRAW);


	CHECK_GL_ERROR();

	glEnableVertexAttribArray(shaderL.posLocation);
	glVertexAttribPointer(shaderL.posLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

	glEnableVertexAttribArray(shaderL.texCoordLocation);

	glVertexAttribPointer(shaderL.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(shaderL.normalLocation);

	glVertexAttribPointer(shaderL.normalLocation, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

	(*geometry)->ambient = glm::vec3(0.9f, 0.9f, 0.9f);
	(*geometry)->diffuse = glm::vec3(0.2f, 0.2f, 0.2f);
	(*geometry)->specular = glm::vec3(0.2f, 0.2f, 0.2f);
	(*geometry)->shininess = 5.0f;


	glBindVertexArray(0);

	(*geometry)->numTriangles = num_polyg * 2;

	fclose(myfile);
	return true;
}
bool loadSingleMesh(const std::string& fileName, SCommonShaderProgram& shader, MeshGeometry** geometry) {
	Assimp::Importer importer;

	// Unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		*geometry = NULL;
		return false;
	}

	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
	if (scn->mNumMeshes != 1) {
		std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
		*geometry = NULL;
		return false;
	}

	// in this phase we know we have one mesh in our loaded scene, we can directly copy its data to OpenGL ...
	const aiMesh* mesh = scn->mMeshes[0];

	*geometry = new MeshGeometry;

	// vertex buffer object, store all vertex positions and normals
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
	// first store all vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
	// then store all normals
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

	// just texture 0 for now
	float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
	float* currentTextureCoord = textureCoords;

	// copy texture coordinates
	aiVector3D vect;

	if (mesh->HasTextureCoords(0)) {
		// we use 2D textures with 2 coordinates and ignore the third coordinate
		for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
			vect = (mesh->mTextureCoords[0])[idx];
			*currentTextureCoord++ = vect.x;
			*currentTextureCoord++ = vect.y;
		}
	}

	// finally store all texture coordinates
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

	// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
	unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}

	// copy our temporary index array to OpenGL and free the array
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

	delete[] indices;

	// copy the material info to MeshGeometry structure
	const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
	aiColor4D color;
	aiString name;
	aiReturn retValue = AI_SUCCESS;

	// Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
	mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	(*geometry)->diffuse = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->ambient = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->specular = glm::vec3(color.r, color.g, color.b);

	ai_real shininess, strength;
	unsigned int max;	// changed: to unsigned

	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
		shininess = 1.0f;
	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
		strength = 1.0f;
	(*geometry)->shininess = shininess * strength;

	(*geometry)->texture = 0;

	// load texture image
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		// get texture name 
		aiString path; // filename

		aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		std::string textureName = path.data;

		size_t found = fileName.find_last_of("/\\");
		// insert correct texture file path 
		if (found != std::string::npos) { // not found
		  //subMesh_p->textureName.insert(0, "/");
			textureName.insert(0, fileName.substr(0, found + 1));
		}

		std::cout << "Loading texture file: " << textureName << std::endl;
		(*geometry)->texture = pgr::createTexture(textureName);
		std::cout << (*geometry)->texture << std::endl;
	}
	CHECK_GL_ERROR();

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

	glEnableVertexAttribArray(shader.posLocation);
	glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (useLighting == true) {
		glEnableVertexAttribArray(shader.normalLocation);
		glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));
	}
	else {
		glDisableVertexAttribArray(shader.colorLocation);
		// following line is problematic on AMD/ATI graphic cards
		// -> if you see black screen (no objects at all) than try to set color manually in vertex shader to see at least something
		glVertexAttrib3f(shader.colorLocation, color.r, color.g, color.b);
	}

	glEnableVertexAttribArray(shader.texCoordLocation);
	glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
	CHECK_GL_ERROR();

	glBindVertexArray(0);

	(*geometry)->numTriangles = mesh->mNumFaces;

	return true;
}

bool loadSingleMesh(const std::string& fileName, GlassShaderProgram& shader, MeshGeometry** geometry) {
	Assimp::Importer importer;

	// Unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// Load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(fileName.c_str(), 0
		| aiProcess_Triangulate             // Triangulate polygons (if any).
		| aiProcess_PreTransformVertices    // Transforms scene hierarchy into one root with geometry-leafs only. For more see Doc.
		| aiProcess_GenSmoothNormals        // Calculate normals per vertex.
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "assimp error: " << importer.GetErrorString() << std::endl;
		*geometry = NULL;
		return false;
	}

	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
	if (scn->mNumMeshes != 1) {
		std::cerr << "this simplified loader can only process files with only one mesh" << std::endl;
		*geometry = NULL;
		return false;
	}

	// in this phase we know we have one mesh in our loaded scene, we can directly copy its data to OpenGL ...
	const aiMesh* mesh = scn->mMeshes[0];

	*geometry = new MeshGeometry;

	// vertex buffer object, store all vertex positions and normals
	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW); // allocate memory for vertices, normals, and texture coordinates
	// first store all vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);
	// then store all normals
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);

	// just texture 0 for now
	float* textureCoords = new float[2 * mesh->mNumVertices];  // 2 floats per vertex
	float* currentTextureCoord = textureCoords;

	// copy texture coordinates
	aiVector3D vect;

	if (mesh->HasTextureCoords(0)) {
		// we use 2D textures with 2 coordinates and ignore the third coordinate
		for (unsigned int idx = 0; idx < mesh->mNumVertices; idx++) {
			vect = (mesh->mTextureCoords[0])[idx];
			*currentTextureCoord++ = vect.x;
			*currentTextureCoord++ = vect.y;
		}
	}

	// finally store all texture coordinates
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords);

	// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
	unsigned int* indices = new unsigned int[mesh->mNumFaces * 3];
	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}

	// copy our temporary index array to OpenGL and free the array
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned) * mesh->mNumFaces, indices, GL_STATIC_DRAW);

	delete[] indices;

	// copy the material info to MeshGeometry structure
	const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
	aiColor4D color;
	aiString name;
	aiReturn retValue = AI_SUCCESS;

	// Get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
	mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing. Must be aiString type!

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	(*geometry)->diffuse = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->ambient = glm::vec3(color.r, color.g, color.b);

	if ((retValue = aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	(*geometry)->specular = glm::vec3(color.r, color.g, color.b);

	ai_real shininess, strength;
	unsigned int max;	// changed: to unsigned

	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max)) != AI_SUCCESS)
		shininess = 1.0f;
	max = 1;
	if ((retValue = aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)) != AI_SUCCESS)
		strength = 1.0f;
	(*geometry)->shininess = shininess * strength;

	(*geometry)->texture = 0;

	// load texture image
	if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		// get texture name 
		aiString path; // filename

		aiReturn texFound = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		std::string textureName = path.data;

		size_t found = fileName.find_last_of("/\\");
		// insert correct texture file path 
		if (found != std::string::npos) { // not found
		  //subMesh_p->textureName.insert(0, "/");
			textureName.insert(0, fileName.substr(0, found + 1));
		}

		std::cout << "Loading texture file: " << textureName << std::endl;
		(*geometry)->texture = pgr::createTexture(textureName);
		std::cout << (*geometry)->texture << std::endl;
	}
	CHECK_GL_ERROR();

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

	glEnableVertexAttribArray(shader.posLocation);
	glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (useLighting == true) {
		glEnableVertexAttribArray(shader.normalLocation);
		glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));
	}

	glEnableVertexAttribArray(shader.texCoordLocation);
	glVertexAttribPointer(shader.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));
	CHECK_GL_ERROR();

	glBindVertexArray(0);

	(*geometry)->numTriangles = mesh->mNumFaces;

	return true;
}

void initTerrainGeometry(GLuint shader, SCommonShaderProgram& shaderL, MeshGeometry** geometry) {

	createTerrainVertices(&terrainTrianglesCount, num_of_sides, terrainVertices, terrainIndices, &count_vertices);


	*geometry = new MeshGeometry;
	(*geometry)->texture = pgr::createTexture(TERRAIN_TEXTURE_NAME);
	(*geometry)->textureMap = pgr::createTexture(TERRAIN_TEXTURE_MAP_NAME);
	(*geometry)->textureDirt = pgr::createTexture(TERRAIN_DIRT_TEXTURE_NAME);
	(*geometry)->textureRock = pgr::createTexture(TERRAIN_ROCK_TEXTURE_NAME);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);

	glBufferData(GL_ARRAY_BUFFER, count_vertices * 3 * sizeof(float) * 2 + count_vertices * 2 * sizeof(float), terrainVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * terrainTrianglesCount, terrainIndices, GL_STATIC_DRAW);
	CHECK_GL_ERROR();


	glEnableVertexAttribArray(terrainShaderProgram.posLocation);
	// vertices of triangles - start at the beginning of the array (interlaced array)
	glVertexAttribPointer(terrainShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(terrainShaderProgram.texCoordLocation);
	// texture coordinates are placed just after the position of each vertex (interlaced array)
	glVertexAttribPointer(terrainShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(count_vertices * 3 * sizeof(float)));
	glEnableVertexAttribArray(terrainShaderProgram.normalLocation);
	// normals of vertices start after the colors
	glVertexAttribPointer(terrainShaderProgram.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)((count_vertices * 5) * sizeof(float)));

	glEnableVertexAttribArray(shaderL.posLocation);
	// vertices of triangles - start at the beginning of the array (interlaced array)
	glVertexAttribPointer(shaderL.posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderL.texCoordLocation);
	// texture coordinates are placed just after the position of each vertex (interlaced array)
	glVertexAttribPointer(shaderL.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)(count_vertices * 3 * sizeof(float)));
	glEnableVertexAttribArray(shaderL.normalLocation);
	// normals of vertices start after the colors
	glVertexAttribPointer(shaderL.normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)((count_vertices * 5) * sizeof(float)));

	(*geometry)->ambient = glm::vec3(0.7f, 0.7f, 0.7f);
	(*geometry)->diffuse = glm::vec3(0.2f, 0.2f, 0.2f);
	(*geometry)->specular = glm::vec3(0.2f, 0.2f, 0.2f);
	(*geometry)->shininess = 5.0f;


	glBindVertexArray(0);

	(*geometry)->numTriangles = terrainTrianglesCount;
}

void initStoneGeometry(SCommonShaderProgram& shader, MeshGeometry** geometry) {

	*geometry = new MeshGeometry;

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, stoneVerticesCount * 6 * sizeof(float), stoneVertices, GL_STATIC_DRAW);

	// copy our temporary index array to opengl and free the array
	glGenBuffers(1, &((*geometry)->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*geometry)->elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * stoneTrianglesCount, stoneIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(shader.posLocation);
	// vertices of triangles - start at the beginning of the array
	glVertexAttribPointer(shader.posLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);

	glEnableVertexAttribArray(shader.normalLocation);
	// normal of vertex starts after the color (interlaced array)
	glVertexAttribPointer(shader.normalLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	(*geometry)->ambient = glm::vec3(0.6f, 0.6f, 0.6f);
	(*geometry)->diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
	(*geometry)->specular = glm::vec3(0.3f, 0.3f, 0.3f);
	(*geometry)->shininess = 5.0f;
	(*geometry)->texture = 0;

	glBindVertexArray(0);

	(*geometry)->numTriangles = stoneTrianglesCount;
}

void initMoonGeometry(GLuint shader, MeshGeometry** geometry) {

	*geometry = new MeshGeometry;

	(*geometry)->texture = pgr::createTexture(MOON_TEXTURE_NAME);
	glBindTexture(GL_TEXTURE_2D, (*geometry)->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cloudVertexData), cloudVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(moonShaderProgram.posLocation);
	glEnableVertexAttribArray(moonShaderProgram.texCoordLocation);
	// vertices of triangles - start at the beginning of the interlaced array
	glVertexAttribPointer(moonShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	// texture coordinates of each vertices are stored just after its position
	glVertexAttribPointer(moonShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = cloudNumQuadVertices;
}

void initFireGeometry(GLuint shader, MeshGeometry** geometry) {

	*geometry = new MeshGeometry;

	(*geometry)->texture = pgr::createTexture(FIRE_TEXTURE_NAME);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fireVertexData), fireVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(fireShaderProgram.posLocation);
	// vertices of triangles - start at the beginning of the array (interlaced array)
	glVertexAttribPointer(fireShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(fireShaderProgram.texCoordLocation);
	// texture coordinates are placed just after the position of each vertex (interlaced array)
	glVertexAttribPointer(fireShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = fireNumQuadVertices;
}

void initStatusBarGeometry(GLuint shader, MeshGeometry** geometry) {

	*geometry = new MeshGeometry;

	(*geometry)->texture = pgr::createTexture(STATUS_BAR_TEXTURE_NAME1);
	(*geometry)->texture2 = pgr::createTexture(STATUS_BAR_TEXTURE_NAME2);
	(*geometry)->texture3 = pgr::createTexture(STATUS_BAR_TEXTURE_NAME3);
	(*geometry)->texture4 = pgr::createTexture(STATUS_BAR_TEXTURE_NAME4);

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(statusBarVertexData), statusBarVertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(barShaderProgram.posLocation);
	// vertices of triangles - start at the beginning of the array (interlaced array)
	glVertexAttribPointer(barShaderProgram.posLocation, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	glEnableVertexAttribArray(barShaderProgram.texCoordLocation);
	// texture coordinates are placed just after the position of each vertex (interlaced array)
	glVertexAttribPointer(barShaderProgram.texCoordLocation, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindVertexArray(0);

	(*geometry)->numTriangles = fireNumQuadVertices;
}

void initSkyboxGeometry(GLuint shader, MeshGeometry** geometry) {

	*geometry = new MeshGeometry;

	glGenVertexArrays(1, &((*geometry)->vertexArrayObject));
	glBindVertexArray((*geometry)->vertexArrayObject);

	glGenBuffers(1, &((*geometry)->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, (*geometry)->vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoordsUV), screenCoordsUV, GL_STATIC_DRAW);


	glEnableVertexAttribArray(skyboxShaderProgram.screenCoordLocation);
	glVertexAttribPointer(skyboxShaderProgram.screenCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	glUseProgram(0);
	CHECK_GL_ERROR();

	(*geometry)->numTriangles = 2;

	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &((*geometry)->texture));
	glBindTexture(GL_TEXTURE_CUBE_MAP, (*geometry)->texture);

	GLuint textureMap[] = {
	  GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	  GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for (int i = 0; i < 6; i++) {
		std::string texName = std::string(SKYBOX_CUBE_TEXTURE_FILE_PREFIX) + "_" + suffixes[i] + ".bmp";

		if (!pgr::loadTexImage2D(texName, textureMap[i])) {
			pgr::dieWithError("Skybox cube map loading failed!");
		}
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERROR();
}

/** Initialize vertex buffers and vertex arrays for all objects.
 */
void initializeModels() {

	loadWithoutAssimp(CAT_OBJ, CAT_TEXTURE, shaderProgram, &catGeometry, CAT_NUM_OF_VERTICES, CAT_NUM_OF_POLYGONS);

	if (loadSingleMesh(TREE_MODEL_NAME, shaderProgram, &treeGeometry) != true) {
		std::cerr << "initializeModels(): Tree model loading failed." << std::endl;
	}
	CHECK_GL_ERROR();

	if (loadSingleMesh(COTTAGE_MODEL_NAME, shaderProgram, &cottageGeometry) != true) {
		std::cerr << "initializeModels(): Cottage model loading failed." << std::endl;
	}
	CHECK_GL_ERROR();

	if (loadSingleMesh(GLASS_MODEL_NAME, shaderProgram, &glassGeometry) != true) {
		std::cerr << "initializeModels(): Glass model loading failed." << std::endl;
	}
	CHECK_GL_ERROR();

	if (loadSingleMesh(LOG_MODEL_NAME, shaderProgram, &logGeometry) != true) {
		std::cerr << "initializeModels(): Log model loading failed." << std::endl;
	}
	CHECK_GL_ERROR();

	if (loadSingleMesh(BUG_MODEL_NAME, shaderProgram, &bugGeometry) != true) {
		std::cerr << "initializeModels(): Bug model loading failed." << std::endl;
	}
	CHECK_GL_ERROR();

	// load space ship model from external file
	if (loadSingleMesh(HUMAN_MODEL_NAME, shaderProgram, &humanGeometry) != true) {
		std::cerr << "initializeModels(): Human model loading failed." << std::endl;
	}
	CHECK_GL_ERROR();


	initStoneGeometry(shaderProgram, &stoneGeometry);

	initFireGeometry(fireShaderProgram.program, &fireGeometry);
	
	initStatusBarGeometry(barShaderProgram.program, &statusBarGeometry);

	initMoonGeometry(moonShaderProgram.program, &moonGeometry);

	initSkyboxGeometry(skyboxShaderProgram.program, &skyboxGeometry);

	initTerrainGeometry(terrainShaderProgram.program, shaderProgram, &terrainGeometry);
}

void cleanupGeometry(MeshGeometry* geometry) {

	glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
	glDeleteBuffers(1, &(geometry->elementBufferObject));
	glDeleteBuffers(1, &(geometry->vertexBufferObject));

	if (geometry->texture != 0)
		glDeleteTextures(1, &(geometry->texture));
}

void cleanupModels() {

	cleanupGeometry(humanGeometry);
	cleanupGeometry(treeGeometry);
	cleanupGeometry(bugGeometry);
	cleanupGeometry(cottageGeometry);
	cleanupGeometry(glassGeometry);
	cleanupGeometry(logGeometry);
	cleanupGeometry(stoneGeometry);
	cleanupGeometry(terrainGeometry);

	cleanupGeometry(fireGeometry);
	cleanupGeometry(moonGeometry);
	cleanupGeometry(skyboxGeometry);
	cleanupGeometry(catGeometry);
}
