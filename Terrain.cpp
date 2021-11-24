//----------------------------------------------------------------------------------------
/**
 * \file    Terrain.cpp
 * \author  veverlu4
 * \date    2020
 * \brief   Creating terrain
 */
 //----------------------------------------------------------------------------------------
#include <cstdlib>
#include <time.h>
#include "pgr.h"
#include "Terrain.h"
#include <iostream>

Pixel* img;

float heightData[TERRAIN_HEIGHT * 2][TERRAIN_WIDTH * 2] = {};
Pixel* loadImage() {
	int width = 0;
	int height = 0;
	int a = 0;
	char b = 0;
	char mezera = 0;
	int i = 0;
	int size_ = 0;
	int tmp1 = 0, tmp2 = 0, tmp3 = 0, tmp4 = 0, tmp5 = 0;


	unsigned char* pole;

	Pixel* tempor;

	FILE* file = fopen("data/heightmap.ppm", "r");
	if (file == NULL)
		std::cout << "PROBLEM!!\n";
	fseek(file, 0, SEEK_END);
	size_ = ftell(file);
	fseek(file, 0, SEEK_SET);
	pole = (unsigned char*)malloc((size_));
	if (fread(pole, 1, size_, file) == 0) {
		return NULL;
	}
	fclose(file);

	i = i + 3;
	while (1) {
		b = pole[i++];
		if (b == ' ' || b == '\n') {
			mezera++;
			if (mezera == 1) {
				width = a;
				a = 0;
			}
			if (mezera == 2) {
				height = a;
				break;
			}

		}
		else {
			a = a * 10 + (b - '0');
		}
	}
	std::cout << height << " height\n";
	std::cout << width << " width\n";
	i = i + 4;
	//width = height = 512;
	img = (Pixel*)malloc(height * width * sizeof(Pixel));
	tempor = img;
	//std::cout << height << " height\n";
	//std::cout << width << " width\n";

	for (int l = 0; l < width; l++) {
		for (int p = 0; p < height; p++) {
			//printf("pole: %d\n", pole[i]);
			img->r = float(pole[i++] / 255.0f);
			//printf("img-r: %f pole: %f\n", img->r, pole[i-1]/255.0f);
			img->g = pole[i++];
			img->b = pole[i++];
			img++;
		}
	}
	img = tempor;
	/*FILE* fp = fopen("output.ppm", "wb");
	fprintf(fp, "P6\n%d\n%d\n255\n", width, height);
	fwrite(img, sizeof(Pixel), width * height, fp);
	fclose(fp);*/
	return img;
}

float getHeight(int x, Pixel* img) {
	//printf("x: %d r: %d\n", x, img[x].r);
	//int rgb = ((img[x].r & 0x0ff) << 16) | ((img[x].g & 0x0ff) << 8) | (img[x].b & 0x0ff);
	//float height = rgb;
	float height = MIN_HEIGHT;
	height += float(MAX_HEIGHT * img[x].r);
	//printf("height: %d\n", height);
	return height;

}

void generateHeightData(int width, int height) {
	//printf("width: %d height: %d count_vertices %d\n", width, height);
	GLenum map = NULL;
	Pixel* img = loadImage();
	int vertex_count = 512;
	int count = 0;
	//printf("width: %d heght: %d\n", width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width * 2; j++) {
			heightData[i][j] = getHeight(count++, img);
		}
	}
	/*for (size_t i = 0; i < height + 1; i++)
	{

		for (size_t j = 0; j < width / 20; j++)
		{
			heightData[i][j] = 5.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (6.0f - 5.0f)));
		}
		for (size_t j = width / 20; j < width / 18; j++)
		{
			heightData[i][j] = 4.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5.0f - 4.0f)));
		}for (size_t j = width / 18; j < width / 16; j++)
		{
			heightData[i][j] = 3.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (4.0f - 3.0f)));
		}for (size_t j = width / 16; j < width / 14; j++)
		{
			heightData[i][j] = 2.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3.0f - 2.0f)));
		}for (size_t j = width / 14; j < width / 12; j++)
		{
			heightData[i][j] = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2.0f - 1.0f)));
		}for (size_t j = width / 12; j < width / 10; j++)
		{
			heightData[i][j] = 0.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - 0.0f)));
		}for (size_t j = width / 10; j < width / 8; j++)
		{
			heightData[i][j] = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.0f + 1.0f)));
		}
		for (size_t j = width / 8; j < width / 3; j++)
		{
			heightData[i][j] = -2.05f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-2 + 2.05f)));
		}
		for (size_t j = width / 3; j < width * 2 / 3; j++)
		{
			heightData[i][j] = -2.00001f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-2 + 2.00001f)));
		}
		for (size_t j = width * 2 / 3; j < width - width / 10; j++)
		{
			heightData[i][j] = -2.05f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-2 + 2.05f)));
		}
		for (size_t j = width - width / 10; j < width + 1; j++)
		{
			heightData[i][j] = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (6.0f - 1.0f)));
		}
	}*/
	//printf("-----------------\n");
}



void createTerrainVertices(int* terrainTrianglesCount, int num_of_sides, float terrainVertices[], unsigned int terrainIndices[], int* count_vertices) {
	int width = TERRAIN_WIDTH;
	int height = TERRAIN_HEIGHT;
	if (width > 1) {
		*terrainTrianglesCount = (2 * width - 1) * (height - 1) * 2;
	}
	else
		*terrainTrianglesCount = width * (height - 1) * 2;
	generateHeightData(width, height);
	//terrainVertices = (float*)malloc(sizeof(float) * width * height * 3);
	float pz = -1;
	float px;
	float dx = 0.5;
	float dz = 0.5;

	int count = 0;

	int cnt = 0;
	int num_of_vert = 6;
	int index = 0;
	int index_uv = 0;
	int dx_dz[4][2] = { {1,1},{1,-1}, {-1, 1}, {-1, -1} };
	for (size_t l = 0; l < num_of_sides; l++)
	{
		dx = dx * dx_dz[l][0];
		dz = dz * dx_dz[l][1];

		for (size_t i = 0; i < width; i++)
		{
			px = -1.0f;
			for (size_t j = 0; j < height; j++)
			{
				j = j * num_of_vert;


				terrainVertices[index++] = px;
				terrainVertices[index++] = heightData[i][j / num_of_vert];
				terrainVertices[index++] = pz;
				terrainVertices[index++] = px;
				terrainVertices[index++] = heightData[i + 1][j / num_of_vert];
				terrainVertices[index++] = pz + dz;


				if (i > 0) {
					if (count + 4 < (height - 1) * 2 * 3 + 2 * i * (height - 1) * 2 * 3) {
						//printf("vert: %d, height: %d\n", count_vertices, height);
						terrainIndices[count++] = *count_vertices - (height - 1) * 2 - 1;
						terrainIndices[count++] = *count_vertices + 2;
						terrainIndices[count++] = *count_vertices;
						terrainIndices[count++] = *count_vertices - (height - 1) * 2 - 1;
						terrainIndices[count++] = *count_vertices + 2;
						terrainIndices[count++] = *count_vertices - (height - 1) * 2 - 1 + 2;
						terrainIndices[count++] = *count_vertices;
						terrainIndices[count++] = *count_vertices + 3;
						terrainIndices[count++] = *count_vertices + 1;
						terrainIndices[count++] = *count_vertices;
						terrainIndices[count++] = *count_vertices + 3;
						terrainIndices[count++] = *count_vertices + 2;
					}

				}
				else {
					if (count + 2 < (height - 1) * 2 * 3) {
						terrainIndices[count++] = cnt * 2;
						terrainIndices[count++] = cnt * 2 + 3;
						terrainIndices[count++] = cnt * 2 + 2;
						terrainIndices[count++] = cnt * 2;
						terrainIndices[count++] = cnt * 2 + 1;
						terrainIndices[count++] = cnt * 2 + 3;
						cnt += 1;
					}
				}
				*count_vertices += 2;
				px += dx;
				j = j / num_of_vert;

				terrainVertices[index_uv++ + (height) * (width) * 2 * 3] = j;
				terrainVertices[index_uv++ + (height) * (width) * 2 * 3] = i * 2;
				terrainVertices[index_uv++ + (height) * (width) * 2 * 3] = j;
				terrainVertices[index_uv++ + (height) * (width) * 2 * 3] = i * 2 + 1;

				/*terrainVertices[index - 6 + (height) * (width) * 2 * 5] = 0.0f;
				terrainVertices[index - 5 + (height) * (width) * 2 * 5] = 1.0f;
				terrainVertices[index - 4 + (height) * (width) * 2 * 5] = 0.0f;
				terrainVertices[index - 3 + (height) * (width) * 2 * 5] = 0.0f;
				terrainVertices[index - 2 + (height) * (width) * 2 * 5] = 1.0f;
				terrainVertices[index - 1 + (height) * (width) * 2 * 5] = 0.0f;*/





			}

			pz += 2 * dz;
		}

		//terrainTrianglesCount = count_vertices / 2;

	}
	for (size_t i = 0; i < *terrainTrianglesCount * 3; i += 3)
	{
		glm::vec3 v0 = glm::vec3(terrainVertices[terrainIndices[i] * 3], terrainVertices[terrainIndices[i] * 3 + 1], terrainVertices[terrainIndices[i] * 3 + 2]);
		glm::vec3 v1 = glm::vec3(terrainVertices[terrainIndices[i + 1] * 3], terrainVertices[terrainIndices[i + 1] * 3 + 1], terrainVertices[terrainIndices[i + 1] * 3 + 2]);
		glm::vec3 v2 = glm::vec3(terrainVertices[terrainIndices[i + 2] * 3], terrainVertices[terrainIndices[i + 2] * 3 + 1], terrainVertices[terrainIndices[i + 2] * 3 + 2]);

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
		terrainVertices[terrainIndices[i] * 3 + *count_vertices * 5] = normal[0];
		terrainVertices[terrainIndices[i] * 3 + *count_vertices * 5 + 1] = normal[1];
		terrainVertices[terrainIndices[i] * 3 + *count_vertices * 5 + 2] = normal[2];
		terrainVertices[terrainIndices[i + 1] * 3 + *count_vertices * 5] = normal[0];
		terrainVertices[terrainIndices[i + 1] * 3 + *count_vertices * 5 + 1] = normal[1];
		terrainVertices[terrainIndices[i + 1] * 3 + *count_vertices * 5 + 2] = normal[2];
		terrainVertices[terrainIndices[i + 2] * 3 + *count_vertices * 5] = normal[0];
		terrainVertices[terrainIndices[i + 2] * 3 + *count_vertices * 5 + 1] = normal[1];
		terrainVertices[terrainIndices[i + 2] * 3 + *count_vertices * 5 + 2] = normal[2];
	}
	for (size_t i = 0; i < *count_vertices * 3; i += 3)
	{
		glm::vec3 v = glm::vec3(terrainVertices[i + *count_vertices * 5], terrainVertices[i + 1 + *count_vertices * 5], terrainVertices[i + 2 + *count_vertices * 5]);
		v = glm::normalize(v);
		terrainVertices[i + *count_vertices * 5] = v.x;
		terrainVertices[i + 1 + *count_vertices * 5] = v.y;
		terrainVertices[i + 2 + *count_vertices * 5] = v.z;
		//std::cout << v.x << " v.x\n";
		//std::cout << v.y << " v.y\n";
		//std::cout << v.z << " v.z\n";
	}
	printf("vertices: %d triangles: %d\n", *count_vertices, terrainTrianglesCount);
	int b = 1;
}