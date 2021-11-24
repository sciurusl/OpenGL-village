//----------------------------------------------------------------------------------------
/**
 * \file    Terrain.h
 * \author  veverlu4
 * \date    2020
 * \brief   Creating terrain
 */
 //----------------------------------------------------------------------------------------
#ifndef __TERRAIN_H
#define __TERRAIN_H

#pragma once
#include "data.h"
typedef struct {
	float r, g, b;
}Pixel;

void createTerrainVertices(int* terrainTrianglesCount, int num_of_sides, float* terrainVertices, unsigned int* terrainIndices, int* count_vertices);
const int  MAX_HEIGHT = 5;
const float MIN_HEIGHT = -1.4f;
const int MAX_PIXEL_COLOR = 256*256*256;
#endif //__TERRAIN_H