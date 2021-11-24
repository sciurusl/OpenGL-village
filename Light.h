//----------------------------------------------------------------------------------------
/**
 * \file    Light.h
 * \author  veverlu4
 * \date    2020
 * \brief   Creation of lights and sending parameters to shader
 */
 //----------------------------------------------------------------------------------------

#ifndef __LIGHT_H
#define __LIGHT_H
#pragma once
#include "data.h"

#include <pgr.h>

typedef struct Light{

	glm::vec3  ambient;       
	glm::vec3  diffuse;       
	glm::vec3  specular;      
	glm::vec3  direction;

} Light;

typedef struct SpotLight : public Light {
	bool turnedOn;
	glm::vec3  position;
	glm::vec3 attenuation;
	

} SpotLight;

typedef struct DirectionalLight : public Light {
	bool shines;

} DirectionalLight;

typedef struct PointLight : public Light {
	float cutOff;
	float exponent;
	float effect;
	glm::vec3 attenuation;
	glm::vec3  position;
} PointLight;

SpotLight setUpSpotLight(glm::vec3 position);
PointLight setUpPointLight(glm::vec3 position, glm::vec3 direction);
DirectionalLight setUpLightDirectional();
void initShaderDirectionalLight(SCommonShaderProgram& shader, DirectionalLight light);
void initShaderPointLight(SCommonShaderProgram& shader, PointLight light);
void initShaderSpotLight(SCommonShaderProgram& shader, SpotLight light);
void initializeLightShader(SCommonShaderProgram& shader);
void initializeTerrainLightShader();
#endif
