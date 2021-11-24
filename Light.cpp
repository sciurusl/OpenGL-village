//----------------------------------------------------------------------------------------
/**
 * \file    Light.cpp
 * \author  veverlu4
 * \date    2020
 * \brief   Creation of lights and sending parameters to shader
 */
 //----------------------------------------------------------------------------------------

#include <iostream>
#include <time.h>
#include <list>
#include "pgr.h"
#include "render_stuff.h"
#include "spline.h"
#include "Light.h"

extern TerrainShaderProgram terrainShaderProgram;

DirectionalLight setUpLightDirectional() {
	DirectionalLight light;
	light.ambient = glm::vec3(0.0f);
	light.diffuse = glm::vec3(1.0f, 1.0f, 0.5f);
	light.specular = glm::vec3(1.0f);
	light.direction = glm::vec3(0.0f, 1.0f, 0.0f);
	light.shines = true;
	return light;
}

PointLight setUpPointLight(glm::vec3 position, glm::vec3 direction) {
	PointLight light;
	light.ambient = glm::vec3(0.2f);
	light.diffuse = glm::vec3(1.0);
	light.specular = glm::vec3(1.0);
	light.cutOff = 0.95f;
	light.exponent = 6.0f;
	light.attenuation = glm::vec3(0.0f, 1.0f, 0.0f);
	light.position = position;
	light.direction = direction;
	return light;
}


SpotLight setUpSpotLight(glm::vec3 position) {
	SpotLight light;
	light.ambient = glm::vec3(0.2f);
	light.diffuse = glm::vec3(1.0);
	light.specular = glm::vec3(1.0);
	light.turnedOn = false;
	light.position = position;
	light.attenuation = glm::vec3(0.0f, 0.0f, 2.0f);
	return light;
}
	
void initShaderDirectionalLight(SCommonShaderProgram& shader, DirectionalLight light) {
	glUseProgram(shader.program);

	glUniform3fv(shader.direction, 1, glm::value_ptr(light.direction));
	glUniform1i(shader.shines, light.shines);
	glUniform3fv(shader.ambientDirectionalLight, 1, glm::value_ptr(light.ambient));
	glUniform3fv(shader.specularDirectionalLight, 1, glm::value_ptr(light.specular));
	glUniform3fv(shader.diffuseDirectionalLight, 1, glm::value_ptr(light.diffuse));

	glUseProgram(0);
	CHECK_GL_ERROR();
	glUseProgram(terrainShaderProgram.program);
	glUniform3fv(terrainShaderProgram.direction, 1, glm::value_ptr(light.direction));
	glUniform1i(terrainShaderProgram.shines, light.shines);
	glUniform3fv(terrainShaderProgram.ambientDirectionalLight, 1, glm::value_ptr(light.ambient));
	glUniform3fv(terrainShaderProgram.specularDirectionalLight, 1, glm::value_ptr(light.specular));
	glUniform3fv(terrainShaderProgram.diffuseDirectionalLight, 1, glm::value_ptr(light.diffuse));

	glUseProgram(0);
	CHECK_GL_ERROR();
	return;
}


void initShaderPointLight(SCommonShaderProgram& shader, PointLight light) {
	glUseProgram(shader.program);

	glUniform3fv(shader.directionPointLight, 1, glm::value_ptr(light.direction));
	glUniform1f(shader.exponent, light.exponent);
	glUniform1f(shader.cutOff, light.cutOff);
	glUniform3fv(shader.ambientPointLight, 1, glm::value_ptr(light.ambient));
	glUniform3fv(shader.specularPointLight, 1, glm::value_ptr(light.specular));
	glUniform3fv(shader.diffusePointLight, 1, glm::value_ptr(light.diffuse));
	glUniform3fv(shader.attenuationPointLight, 1, glm::value_ptr(light.attenuation));
	glUniform3fv(shader.positionPointLight, 1, glm::value_ptr(light.position));

	glUseProgram(0);
	glUseProgram(terrainShaderProgram.program);

	glUniform3fv(terrainShaderProgram.directionPointLight, 1, glm::value_ptr(light.direction));
	glUniform1f(terrainShaderProgram.exponent, light.exponent);
	glUniform1f(terrainShaderProgram.cutOff, light.cutOff);
	glUniform3fv(terrainShaderProgram.ambientPointLight, 1, glm::value_ptr(light.ambient));
	glUniform3fv(terrainShaderProgram.specularPointLight, 1, glm::value_ptr(light.specular));
	glUniform3fv(terrainShaderProgram.diffusePointLight, 1, glm::value_ptr(light.diffuse));
	glUniform3fv(terrainShaderProgram.attenuationPointLight, 1, glm::value_ptr(light.attenuation));
	glUniform3fv(terrainShaderProgram.positionPointLight, 1, glm::value_ptr(light.position));

	glUseProgram(0);
	return;
}

void initShaderSpotLight(SCommonShaderProgram& shader, SpotLight light) {
	glUseProgram(shader.program);

	glUniform3fv(shader.ambientSpotLight, 1, glm::value_ptr(light.ambient));
	glUniform3fv(shader.specularSpotLight, 1, glm::value_ptr(light.specular));
	glUniform3fv(shader.diffuseSpotLight, 1, glm::value_ptr(light.diffuse));
	glUniform3fv(shader.attenuationSpotLight, 1, glm::value_ptr(light.attenuation));
	glUniform3fv(shader.positionSpotLight, 1, glm::value_ptr(light.position));
	glUniform1f(shader.turnedOn, light.turnedOn);

	glUseProgram(0);

	glUseProgram(terrainShaderProgram.program);

	glUniform3fv(terrainShaderProgram.ambientSpotLight, 1, glm::value_ptr(light.ambient));
	glUniform3fv(terrainShaderProgram.specularSpotLight, 1, glm::value_ptr(light.specular));
	glUniform3fv(terrainShaderProgram.diffuseSpotLight, 1, glm::value_ptr(light.diffuse));
	glUniform3fv(terrainShaderProgram.attenuationSpotLight, 1, glm::value_ptr(light.attenuation));
	glUniform3fv(terrainShaderProgram.positionSpotLight, 1, glm::value_ptr(light.position));
	glUniform1f(terrainShaderProgram.turnedOn, light.turnedOn);

	glUseProgram(0);
	return;
}

void initializeLightShader(SCommonShaderProgram& shader) {

	std::vector<GLuint> shaderList;

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "light.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "light.frag"));

	// create the shader program with two shaders
	shader.program = pgr::createProgram(shaderList);

	shader.ambientSpotLight = glGetUniformLocation(shader.program, "spotLight.ambient");
	shader.specularSpotLight = glGetUniformLocation(shader.program, "spotLight.specular");
	shader.diffuseSpotLight = glGetUniformLocation(shader.program, "spotLight.diffuse");
	shader.attenuationSpotLight = glGetUniformLocation(shader.program, "spotLight.attenuation");
	shader.positionSpotLight = glGetUniformLocation(shader.program, "spotLight.position");
	shader.turnedOn = glGetUniformLocation(shader.program, "spotLight.turnedOn");

	shader.ambientDirectionalLight = glGetUniformLocation(shader.program, "directionalLight.ambient");
	shader.specularDirectionalLight = glGetUniformLocation(shader.program, "directionalLight.specular");
	shader.diffuseDirectionalLight = glGetUniformLocation(shader.program, "directionalLight.diffuse");
	shader.direction = glGetUniformLocation(shader.program, "directionalLight.direction");
	shader.shines = glGetUniformLocation(shader.program, "directionalLight.shines");
	
	shader.ambientPointLight = glGetUniformLocation(shader.program, "pointLight.ambient");
	shader.specularPointLight = glGetUniformLocation(shader.program, "pointLight.specular");
	shader.diffusePointLight = glGetUniformLocation(shader.program, "pointLight.diffuse");
	shader.attenuationPointLight = glGetUniformLocation(shader.program, "pointLight.attenuation");
	shader.positionPointLight = glGetUniformLocation(shader.program, "pointLight.position");
	shader.directionPointLight = glGetUniformLocation(shader.program, "pointLight.direction");
	shader.cutOff = glGetUniformLocation(shader.program, "pointLight.cutOff");
	shader.exponent = glGetUniformLocation(shader.program, "pointLight.exponent");

	shaderList.clear();
}

void initializeTerrainLightShader() {
	std::vector<GLuint> shaderList;

	shaderList.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, "terrain.vert"));
	shaderList.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, "terrain.frag"));

	terrainShaderProgram.ambientSpotLight = glGetUniformLocation(terrainShaderProgram.program, "spotLight.ambient");
	terrainShaderProgram.specularSpotLight = glGetUniformLocation(terrainShaderProgram.program, "spotLight.specular");
	terrainShaderProgram.diffuseSpotLight = glGetUniformLocation(terrainShaderProgram.program, "spotLight.diffuse");
	terrainShaderProgram.attenuationSpotLight = glGetUniformLocation(terrainShaderProgram.program, "spotLight.attenuation");
	terrainShaderProgram.positionSpotLight = glGetUniformLocation(terrainShaderProgram.program, "spotLight.position");
	terrainShaderProgram.turnedOn = glGetUniformLocation(terrainShaderProgram.program, "spotLight.turnedOn");

	terrainShaderProgram.ambientDirectionalLight = glGetUniformLocation(terrainShaderProgram.program, "directionalLight.ambient");
	terrainShaderProgram.specularDirectionalLight = glGetUniformLocation(terrainShaderProgram.program, "directionalLight.specular");
	terrainShaderProgram.diffuseDirectionalLight = glGetUniformLocation(terrainShaderProgram.program, "directionalLight.diffuse");
	terrainShaderProgram.direction = glGetUniformLocation(terrainShaderProgram.program, "directionalLight.direction");
	terrainShaderProgram.shines = glGetUniformLocation(terrainShaderProgram.program, "directionalLight.shines");

	terrainShaderProgram.ambientPointLight = glGetUniformLocation(terrainShaderProgram.program, "pointLight.ambient");
	terrainShaderProgram.specularPointLight = glGetUniformLocation(terrainShaderProgram.program, "pointLight.specular");
	terrainShaderProgram.diffusePointLight = glGetUniformLocation(terrainShaderProgram.program, "pointLight.diffuse");
	terrainShaderProgram.attenuationPointLight = glGetUniformLocation(terrainShaderProgram.program, "pointLight.attenuation");
	terrainShaderProgram.positionPointLight = glGetUniformLocation(terrainShaderProgram.program, "pointLight.position");
	terrainShaderProgram.directionPointLight = glGetUniformLocation(terrainShaderProgram.program, "pointLight.direction");
	terrainShaderProgram.cutOff = glGetUniformLocation(terrainShaderProgram.program, "pointLight.cutOff");
	terrainShaderProgram.exponent = glGetUniformLocation(terrainShaderProgram.program, "pointLight.exponent");

	shaderList.clear();
}