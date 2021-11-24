//----------------------------------------------------------------------------------------
/**
 * \file    data.h
 * \author  veverlu4
 * \date    2020
 * \brief   Basic defines and data structures.
 */
 //----------------------------------------------------------------------------------------

#ifndef __DATA_H
#define __DATA_H
#define WINDOW_WIDTH   750
#define WINDOW_HEIGHT  750
#define WINDOW_TITLE   "Vesnice"

extern float COTTAGES_COUNT;
extern float TREES_COUNT;

extern float SPACESHIP_VIEW_ANGLE_DELTA;

extern float HUMAN_SIZE;
extern float HUMAN_HEIGHT;
extern float HUMAN_WIDTH;
extern float HUMAN_LENGTH;

extern float FIRE_BAR_SIZE;
extern float FIRE_BAR_TIME;

extern float TREE_SIZE;
extern float TREE_HEIGHT;
extern float TREE_WIDTH;
extern float TREE_LENGTH;

extern float COTTAGE_SIZE;
extern float COTTAGE_HEIGHT;
extern float COTTAGE_WIDTH;
extern float COTTAGE_LENGTH;
extern float GLASS_SIZE;

extern float STONE_SIZE;
extern float CAT_SIZE;
extern float FIRE_SIZE;
extern float MOON_SIZE;

extern float TERRAIN_SIZE;
#define TERRAIN_WIDTH 120
#define TERRAIN_HEIGHT 120

extern float LOG_SIZE;
extern float BUG_SIZE;

extern float STONES_COUNT_MIN;

extern float HUMAN_SPEED_INCREMENT;
extern float HUMAN_SPEED_MAX;

#define SCENE_WIDTH  1.0f
#define SCENE_HEIGHT 1.0f
#define SCENE_DEPTH  1.0f

#define CAMERA_ELEVATION_MAX 45.0f

extern float CAT_NUM_OF_VERTICES;
extern float CAT_NUM_OF_VERTEX_NORMALS;
extern float CAT_NUM_OF_TEXTURE_COORDS;
extern float CAT_NUM_OF_POLYGONS;
// keys used in the key map
enum { KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_SPACE, KEYS_COUNT };


const std::string skyboxFarPlaneVertexShaderSrc(
	"#version 140\n"
	"\n"
	"uniform mat4 inversePVmatrix;\n"
	"in vec2 screenCoord;\n"
	"out vec3 texCoord_v;\n"
	"out vec4 color_f;\n"
	"\n"
	"void main() {\n"
	"  vec4 farplaneCoord = vec4(screenCoord, 0.9999, 1.0);\n"
	"  vec4 worldViewCoord = inversePVmatrix * farplaneCoord;\n"
	"  texCoord_v = worldViewCoord.xyz / worldViewCoord.w;\n"
	"  color_f = vec4(0.0, 0.0, 1.0, 1.0);\n"
	"  gl_Position = farplaneCoord;\n"
	"}\n"
);


const std::string skyboxFarPlaneFragmentShaderSrc(
	"#version 140\n"
	"\n"
	"uniform samplerCube skyboxSampler;\n"
	"in vec3 texCoord_v;\n"
	"uniform float time;\n"
	"out vec4 color_f;\n"
	"uniform bool isFog;\n"
	" float var;\n"
	"uniform bool fastenTime;\n"
	"\n"
	"void main() {\n"
	" float pct = 0;\n"
	" if (fastenTime){\n"
	"      pct = 1;}\n"
	" else{\n"
	"	var = 40;\n"
	"	pct = abs(sin((time-25)/var));}\n"
	" color_f = texture(skyboxSampler, texCoord_v);\n"
	"  color_f = mix(color_f, vec4(0.0,0.0, 0.0, 1), pct);\n"
	" if(isFog)\n"
	"     color_f = vec4(0.5, 0.5,0.5, 1.0);\n"
	"}\n"
);

const std::string glassVertexShaderSrc(
	"#version 140\n"
	"\n"
	"uniform mat4 inversePVmatrix;\n"
	"in vec2 screenCoord;\n"
	"\n"
	"void main() {\n"
	"  vec4 farplaneCoord = vec4(screenCoord, 0.9999, 1.0);\n"
	"  vec4 worldViewCoord = inversePVmatrix * farplaneCoord;\n"
	"  gl_Position = farplaneCoord;\n"
	"}\n"
);

const std::string glassFragmentShaderSrc(
	"#version 140\n"
	"\n"
	"uniform samplerCube skyboxSampler;\n"
	"out vec4 color_f;\n"
	"\n"
	"void main() {\n"
	" color_f = vec4(0.5, 0.5,0.5, 1.0);\n"
	"}\n"
);



const int fireNumQuadVertices = 4;
const float fireVertexData[fireNumQuadVertices * 5] = {
	  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	   1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};

const int statusBarNumQuadVertices = 4;
const float statusBarVertexData[statusBarNumQuadVertices * 5] = {
	  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	   8.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	   8.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};


const int cloudNumQuadVertices = 4;
const float cloudVertexData[cloudNumQuadVertices * 5] = {

	  //  x      y     z     u     v
	  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	   1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};

extern float treesPos[5 * 3];
extern float cottagesPos[];

const int stoneTrianglesCount = 32;
const int stoneVerticesCount = 18;

const float stone = (float)(1 / sqrt(2));
const float stoneVertices[] = {
	0.0f, 1.25f, 0.0f, 0.0f, 1.0f, 0.0f, //0 
	0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.25f,//1
	0.0f, 1.0f, -1.0f, 0.0f, 1.0f, -1.25f,//2
	1.0f, 1.0f, 0.0f, 1.25f, 1.0f, 0.0f,//3
	-1.0f, 1.0f, 0.0f, -1.25f, 1.0f, 0.0f,//4
	stone, 1.0f, stone, stone*1.25f, 1.0f, stone*1.25f,//5
	stone, 1.0f, -stone, stone * 1.25f, 1.0f, -stone * 1.25f,//6
	-stone, 1.0f, stone, -stone * 1.25f, 1.0f, stone * 1.25f,//7
	-stone, 1.0f, -stone, -stone * 1.25f, 1.0f, -stone * 1.25f,//8

	0.0f, -0.25f, 0.0f, 0.0f, -1.0f, 0.0f,//9
	0.0f, 0.0f, 1.0f,  0.0f, -1.0f, -0.25f,//10
	0.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.25f,//11
	1.0f, 0.0f, 0.0f, -0.25f, -1.0f, 0.0f,//12
	-1.0f, 0.0f, 0.0f, 0.25f, -1.0f, 0.0f,//13
	stone, 0.0f, stone, -stone * 0.25f, -1.0f, -stone * 0.25f,//14
	stone, 0.0f, -stone, -stone * 0.25f, -1.0f, stone * 0.25f,//15
	-stone, 0.0f, stone, stone * 0.25f, -1.0f, -stone * 0.25f,//16
	-stone, 0.0f, -stone, stone * 0.25f, -1.0f, stone * 0.25f,//17
};

const unsigned int stoneIndices[] = {
	0,1,7,
	0,7,4,
	0,4,8,
	0,8,2,
	0,2,6,
	0,6,3,
	0,3,5,
	0,5,1,

	9,10,16,
	9,16,13,
	9,13,17,
	9,17,11,
	9,11,15,
	9,15,12,
	9,12,14,
	9,14,10,

	17,11,2,
	17,2,8,
	11,6,2,
	11,15,6,
	15,12,3,
	15,3,6,
	12,5,3,
	12,14,5,
	14,10,1,
	14,1,5,
	10,16,7,
	10,7,1,
	16,13,4,
	16,4,7,
	13,17,8,
	13,8,4,
};

const float stonesPosition[]{
	2.9, -4.18, -0.2,
	3.09, -4, -0.2,
	3.04, -4.18, -0.2,
	2.85, -4, -0.2,
	2.98, -3.9, -0.2
};

static const float screenCoordsUV[] = {
	  -1.0f, -1.0f,
	   1.0f, -1.0f,
	  -1.0f,  1.0f,
	   1.0f,  1.0f
};
#endif // __DATA_H
