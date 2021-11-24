//----------------------------------------------------------------------------------------
/**
 * \file    vesnice.cpp
 * \author  veverlu4
 * \date    2020
 * \brief   main file that manages whole scene and main loop
 */
 //----------------------------------------------------------------------------------------

#include <iostream>
#include <time.h>
#include <list>
#include "pgr.h"
#include "render_stuff.h"
#include "spline.h"
#include "Terrain.h"
#include "Light.h"
#include <map>

extern SCommonShaderProgram shaderProgram;
extern TerrainShaderProgram terrainShaderProgram;
extern SkyboxShaderProgram skyboxShaderProgram;

extern bool useLighting;
extern float heightData[TERRAIN_HEIGHT * 2][TERRAIN_WIDTH * 2];
std::string dataFileName[] = { "data/configure/data.txt", "data/configure/dataReload.txt" };
typedef std::list<void*> GameObjectsList;
extern Pixel* img;
float	COTTAGES_COUNT;
float	TREES_COUNT;
float	SPACESHIP_VIEW_ANGLE_DELTA;
float	HUMAN_SIZE;
float	HUMAN_HEIGHT;
float	HUMAN_WIDTH;
float	HUMAN_LENGTH;
float FIRE_BAR_SIZE;
float FIRE_BAR_TIME;
float	TREE_SIZE;
float TREE_HEIGHT;
float	TREE_WIDTH;
float	TREE_LENGTH;
float	COTTAGE_SIZE;
float	COTTAGE_HEIGHT;
float COTTAGE_WIDTH;
float COTTAGE_LENGTH;
float GLASS_SIZE;
float	CAT_SIZE;
float	FIRE_SIZE;
float	MOON_SIZE;
float	TERRAIN_SIZE;
float LOG_SIZE;
float	BUG_SIZE;
float	STONES_COUNT_MIN;
float HUMAN_SPEED_INCREMENT;
float HUMAN_SPEED_MAX;
float	CAT_NUM_OF_VERTICES;
float	CAT_NUM_OF_VERTEX_NORMALS;
float	CAT_NUM_OF_TEXTURE_COORDS;
float	CAT_NUM_OF_POLYGONS;
float STONE_SIZE;
float treesPos[5 * 3];
float cottagesPos[5*3];
struct GameState {

	int windowWidth;    // set by reshape callback
	int windowHeight;   // set by reshape callback
	int logs;

	bool freeCameraMode;
	bool staticCamera1;
	bool fastenTime;// false;
	float cameraElevationAngle; // in degrees = initially 0.0f
	float cameraElevationAngleX; // in degrees = initially 0.0f

	bool verticalMoving;
	bool movingCamera;
	bool gameOver;              // false;
	bool keyMap[KEYS_COUNT];    // false

	bool cameraBug = false;
	float elapsedTime;
	bool bugMoves = false;
	int numOfReloads = 0;
	bool isFog;

} gameState;

struct GameObjects {

	HumanObject* human; // NULL
	GameObjectsList cottages;
	GameObjectsList trees;
	GameObjectsList logs;
	GameObjectsList stones;
	CatObject* cat; // NULL
	GlassObject* glass;
	BugObject* bug; // NULL
	TerrainObject* terrain; // NULL

	FireObject* fire;
	StatusBarObject* statusBar;
	MoonObject* moon; // NULL;
} gameObjects;

SpotLight spotLight;
DirectionalLight directionalLight;
PointLight pointLight;
float angle = 67.5f;
bool textureChanged = false;

void startFire(const glm::vec3& position) {

	printf("starting fire\n");
	FireObject* newFire = new FireObject;

	newFire->speed = 0.0f;
	newFire->destroyed = false;

	newFire->startTime = gameState.elapsedTime;
	newFire->currentTime = newFire->startTime;

	newFire->size = FIRE_SIZE;
	newFire->direction = glm::vec3(0.0f, 0.0f, 1.0f);

	newFire->frameDuration = 0.1f;
	newFire->textureFrames = 16;

	newFire->position = position;

	gameObjects.fire = newFire;
}

void createStatusBar(const glm::vec3& position, LogObject *log) {

	StatusBarObject* statusBar = new StatusBarObject;

	statusBar->destroyed = false;

	statusBar->startTime = gameState.elapsedTime;
	statusBar->currentTime = statusBar->startTime;

	statusBar->size = FIRE_BAR_SIZE;
	statusBar->direction = glm::vec3(0.0f, 0.0f, 1.0f);
	statusBar->numOfTexture = 0;
	statusBar->log = log;

	statusBar->position = position;

	gameObjects.statusBar = statusBar;
}

void increaseHumanSpeed(float deltaSpeed = HUMAN_SPEED_INCREMENT) {

	gameObjects.human->speed =
		std::min(gameObjects.human->speed + deltaSpeed, HUMAN_SPEED_MAX);
}

void decreaseHumanSpeed(float deltaSpeed = HUMAN_SPEED_INCREMENT) {

	gameObjects.human->speed =
		std::max(gameObjects.human->speed - deltaSpeed, 0.0f);
}

void turnHumanLeft(float deltaAngle) {

	gameObjects.human->viewAngle += deltaAngle;

	if (gameObjects.human->viewAngle > 360.0f)
		gameObjects.human->viewAngle -= 360.0f;

	float angle = glm::radians(gameObjects.human->viewAngle);

	gameObjects.human->direction.x = cos(angle);
	gameObjects.human->direction.y = sin(angle);
}

void turnHumanRight(float deltaAngle) {

	gameObjects.human->viewAngle -= deltaAngle;

	if (gameObjects.human->viewAngle < 0.0f)
		gameObjects.human->viewAngle += 360.0f;

	float angle = glm::radians(gameObjects.human->viewAngle);

	gameObjects.human->direction.x = cos(angle);
	gameObjects.human->direction.y = sin(angle);
}

void cleanUpObjects(void) {
	while (!gameObjects.cottages.empty()) {
		delete gameObjects.cottages.back();
		gameObjects.cottages.pop_back();
	}

	while (!gameObjects.logs.empty()) {
		delete gameObjects.logs.back();
		gameObjects.logs.pop_back();
	}

	while (!gameObjects.trees.empty()) {
		delete gameObjects.trees.back();
		gameObjects.trees.pop_back();
	}

	while (!gameObjects.stones.empty()) {
		delete gameObjects.stones.back();
		gameObjects.stones.pop_back();
	}

	if (gameObjects.moon != NULL) {
		delete gameObjects.moon;
		gameObjects.moon = NULL;
	}

	if (gameObjects.glass != NULL) {
		delete gameObjects.glass;
		gameObjects.glass = NULL;
	}

	if (gameObjects.fire != NULL) {
		delete gameObjects.fire;
		gameObjects.fire = NULL;
	}

	if (gameObjects.statusBar != NULL) {
		delete gameObjects.statusBar;
		gameObjects.statusBar = NULL;
	}

	if (gameObjects.bug != NULL) {
		delete gameObjects.bug;
		gameObjects.bug = NULL;
	}
}

TreeObject* createTree(int i) {
	TreeObject* newTree = new TreeObject;

	newTree->destroyed = false;

	newTree->startTime = gameState.elapsedTime;
	newTree->currentTime = newTree->startTime;

	newTree->size = TREE_SIZE;
	newTree->height = TREE_HEIGHT;
	newTree->width = TREE_WIDTH;
	newTree->length = TREE_LENGTH;

	// generate motion direction randomly in range -1.0f ... 1.0f
	newTree->direction = glm::vec3(
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		0.0f
	);
	newTree->direction = glm::normalize(newTree->direction);

	// position is generated randomly as well
	i *= 3;
	newTree->position = glm::vec3(treesPos[i], treesPos[i + 1], treesPos[i + 2] - 0.96f);

	return newTree;
}

BugObject* createBug() {
	BugObject* newBug = new BugObject;

	newBug->destroyed = false;

	newBug->startTime = gameState.elapsedTime;
	newBug->currentTime = newBug->startTime;

	newBug->size = BUG_SIZE;

	// generate motion direction randomly in range -1.0f ... 1.0f

	newBug->direction.x = -sin(angle);
	newBug->direction.y = cos(angle);
	newBug->direction.z = 0;

	// position is generated randomly as well
	newBug->position = glm::vec3(2, -3.5f, -0.18f);
	newBug->initPosition = glm::vec3(2, -3.5f, -0.18f);

	// motion speed 0.0f ... 1.0f
	// rotation speed 0.0f ... 1.0f

	return newBug;
}

CottageObject* createCottage(int index) {
	CottageObject* cottage = new CottageObject;

	cottage->destroyed = false;

	cottage->startTime = gameState.elapsedTime;
	cottage->currentTime = cottage->startTime;

	cottage->size = COTTAGE_SIZE;
	cottage->height = COTTAGE_HEIGHT;
	cottage->length = COTTAGE_LENGTH;
	cottage->width = COTTAGE_WIDTH;

	// generate motion direction randomly in range -1.0f ... 1.0f
	if (index == 0)
		cottage->direction = glm::vec3(-1, 0, 0);
	else
		cottage->direction = glm::vec3(0, -1, 0);

	// position is generated randomly as well
	//int num2 = (rand() % (1 + 11 + 1)) - 11;
	//int num1 = (rand() % (4 - 1 + 1)) + 1;
	cottage->position = glm::vec3(cottagesPos[index*3], cottagesPos[index*3+1], cottagesPos[index*3+2]);

	return cottage;
}

GlassObject* createGlass(void) {
	GlassObject* glass = new GlassObject;

	glass->destroyed = false;

	glass->startTime = gameState.elapsedTime;
	glass->currentTime = glass->startTime;

	glass->size = GLASS_SIZE;
	glass->height = COTTAGE_HEIGHT;
	glass->length = COTTAGE_LENGTH;
	glass->width = COTTAGE_WIDTH;

	// generate motion direction randomly in range -1.0f ... 1.0f
	glass->direction = glm::vec3(
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		0.0f
	);
	glass->direction = glm::normalize(glass->direction);

	glass->position = glm::vec3(4, -3.5f, 0);

	return glass;
}

LogObject* createLog(glm::vec3 position) {
	LogObject* log = new LogObject;

	log->destroyed = false;

	log->startTime = gameState.elapsedTime;
	log->currentTime = log->startTime;

	log->size = LOG_SIZE;

	// generate motion direction randomly in range -1.0f ... 1.0f
	log->direction = glm::vec3(
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		0.0f
	);
	log->direction = glm::normalize(log->direction);

	// position is generated randomly as well
	log->position = position;

	return log;
}


StoneObject* createStone(int i) {
	StoneObject* newStone = new StoneObject;

	newStone->destroyed = false;

	newStone->startTime = gameState.elapsedTime;
	newStone->currentTime = newStone->startTime;

	newStone->size = STONE_SIZE;

	// generate initial position randomly
	// random speed in range 0.0f ... 1.0f
	i *= 3;
	newStone->position = glm::vec3(stonesPosition[i], stonesPosition[i + 1], stonesPosition[i + 2]);
	// generate randomly in range -1.0f ... 1.0f
	newStone->direction = glm::vec3(
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		(float)(2.0 * (rand() / (double)RAND_MAX) - 1.0),
		0.0f
	);
	newStone->direction = glm::normalize(newStone->direction);

	return newStone;
}

void loadData() {
	FILE* myfile;
	char name[120];
	strcpy(name, dataFileName[0].c_str());
	std::map<std::string, float> variables;
	fopen_s(&myfile, name, "rb");
	if (myfile == nullptr) {
		std::cout << "WARNING! File could not be loaded!\n";
		fclose(myfile);
		return;
	}
	char varName[120];
	char c[20];
	std::string str;
	float value;
	while (fscanf(myfile, "%s", &c) != EOF) {
		if (strcmp(c, "v") == 0) {
			fscanf(myfile, "%s = %f\n", &varName, &value);
			int a_size = sizeof(varName) / sizeof(char);
			variables.insert(std::make_pair(varName, value));
		}
		else if (strcmp(c, "p") == 0) {
			fscanf(myfile, "%s = ", &varName);
			if (strcmp(varName, "TREES_POS") == 0) {
				for (int i = 0; i < 5 * 3; i++) {
					fscanf(myfile, "%f, ", &value);
					treesPos[i] = value;
				}
			}
			if (strcmp(varName, "COTTAGES_POS") == 0) {
				COTTAGES_COUNT = variables["COTTAGES_COUNT"];
				for (int i = 0; i < COTTAGES_COUNT * 3; i++) {
					fscanf(myfile, "%f, ", &value);
					cottagesPos[i] = value;
				}
			}
		}
	}
	fclose(myfile);
	TREES_COUNT= variables["TREES_COUNT"];
	SPACESHIP_VIEW_ANGLE_DELTA = variables["SPACESHIP_VIEW_ANGLE_DELTA"];
	HUMAN_SIZE = variables["HUMAN_SIZE"];
	HUMAN_HEIGHT = variables["HUMAN_HEIGHT"];
	HUMAN_WIDTH = variables["HUMAN_WIDTH"];
	HUMAN_LENGTH = variables["HUMAN_LENGTH"];
	FIRE_BAR_SIZE = variables["FIRE_BAR_SIZE"];
	FIRE_BAR_TIME = variables["FIRE_BAR_TIME"];
	TREE_SIZE = variables["TREE_SIZE"];
	TREE_HEIGHT = variables["TREE_HEIGHT"];
	TREE_WIDTH = variables["TREE_WIDTH"];
	TREE_LENGTH = variables["TREE_LENGTH"];
	COTTAGE_SIZE = variables["COTTAGE_SIZE"];
	COTTAGE_HEIGHT = variables["COTTAGE_HEIGHT"];
	COTTAGE_WIDTH = variables["COTTAGE_WIDTH"];
	COTTAGE_LENGTH = variables["COTTAGE_LENGTH"];
	GLASS_SIZE = variables["GLASS_SIZE"];
	CAT_SIZE = variables["CAT_SIZE"];
	FIRE_SIZE = variables["FIRE_SIZE"];
	MOON_SIZE = variables["CLOUD_SIZE"];
	TERRAIN_SIZE = variables["TERRAIN_SIZE"];
	LOG_SIZE = variables["LOG_SIZE"];
	BUG_SIZE = variables["BUG_SIZE"];
	STONES_COUNT_MIN = variables["STONES_COUNT_MIN"];
	HUMAN_SPEED_INCREMENT = variables["HUMAN_SPEED_INCREMENT"];
	HUMAN_SPEED_MAX = variables["HUMAN_SPEED_MAX"];
	CAT_NUM_OF_VERTICES = variables["CAT_NUM_OF_VERTICES"];
	CAT_NUM_OF_VERTEX_NORMALS = variables["CAT_NUM_OF_VERTEX_NORMALS"];
	CAT_NUM_OF_TEXTURE_COORDS = variables["CAT_NUM_OF_TEXTURE_COORDS"];
	CAT_NUM_OF_POLYGONS = variables["CAT_NUM_OF_POLYGONS"];
	STONE_SIZE = variables["STONE_SIZE"];
	
}

void restartGame(void) {
	cleanUpObjects();
	loadData();
	gameState.numOfReloads++;
	gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds
	gameState.fastenTime = false;
	gameState.movingCamera = false;
	gameState.isFog = false;
	gameState.logs = 0;
	gameState.verticalMoving = false;
	gameState.staticCamera1 = false;
	gameState.freeCameraMode = false;

	spotLight = setUpSpotLight(glm::vec3(0.0f, 0.0f, 0.0f));
	directionalLight = setUpLightDirectional();
	pointLight = setUpPointLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	if (gameObjects.terrain == NULL)
		gameObjects.terrain = new TerrainObject;

	gameObjects.terrain->position = glm::vec3(0.0f, 0.0f, 0.0f);
	gameObjects.terrain->size = TERRAIN_SIZE;
	gameObjects.terrain->direction = glm::vec3(cos(glm::radians(90.0f)), sin(glm::radians(90.0f)), 0.0f);
	gameObjects.terrain->startTime = gameState.elapsedTime;
	gameObjects.terrain->currentTime = gameObjects.terrain->startTime;


	if (gameObjects.human == NULL)
		gameObjects.human = new HumanObject;

	gameObjects.human->position = glm::vec3(0.5f, -0.5f, 0.0f);
	gameObjects.human->viewAngle = -90.0f; // degrees
	gameObjects.human->direction = glm::vec3(cos(glm::radians(gameObjects.human->viewAngle)), sin(glm::radians(gameObjects.human->viewAngle)), 0.0f);
	gameObjects.human->speed = 0.0f;
	gameObjects.human->size = HUMAN_SIZE;
	gameObjects.human->width = HUMAN_WIDTH;
	gameObjects.human->height = HUMAN_HEIGHT;
	gameObjects.human->length = HUMAN_LENGTH;
	gameObjects.human->destroyed = false;
	gameObjects.human->startTime = gameState.elapsedTime;
	gameObjects.human->currentTime = gameObjects.human->startTime;

	if (gameObjects.cat == NULL)
		gameObjects.cat = new CatObject;

	gameObjects.cat->position = glm::vec3(2.0f, -2.0f, 0.0f);
	gameObjects.cat->direction = glm::vec3(cos(glm::radians(gameObjects.human->viewAngle)), sin(glm::radians(gameObjects.human->viewAngle)), 0.0f);
	gameObjects.cat->size = CAT_SIZE;
	gameObjects.cat->destroyed = false;
	gameObjects.cat->startTime = gameState.elapsedTime;
	gameObjects.cat->currentTime = gameObjects.human->startTime;

	if (gameObjects.bug == NULL) {
		gameObjects.bug = new BugObject;
		gameObjects.bug = createBug();
	}

	if (gameObjects.glass == NULL)
		gameObjects.glass = createGlass();


	for (int i = 0; i < TREES_COUNT; i++) {
		TreeObject* newTree = createTree(i);

		gameObjects.trees.push_back(newTree);
	}

	for (int i = 0; i < COTTAGES_COUNT; i++) {
		CottageObject* newCottage = createCottage(i);

		gameObjects.cottages.push_back(newCottage);
	}

	for (int i = 0; i < STONES_COUNT_MIN; i++) {
		StoneObject* newStone = createStone(i);
		gameObjects.stones.push_back(newStone);
	}

	if (gameState.freeCameraMode == true) {
		gameState.freeCameraMode = false;
		glutPassiveMotionFunc(NULL);
	}
	gameState.cameraElevationAngle = 0.0f;
	gameState.cameraElevationAngleX = 0.0f;

	// reset key map
	for (int i = 0; i < KEYS_COUNT; i++)
		gameState.keyMap[i] = false;

	gameState.gameOver = false;
}


MoonObject* createMoon(void) {
	MoonObject* newMoon = new MoonObject;

	newMoon->size = MOON_SIZE;
	newMoon->position = glm::vec3(0.5, 0.5, 0);
	newMoon->direction = glm::vec3(0.0f, 1.0f, 0.0f);

	newMoon->destroyed = false;

	newMoon->startTime = gameState.elapsedTime;
	newMoon->currentTime = newMoon->startTime;

	return newMoon;
}

bool pointInBlock(const glm::vec3& point, const glm::vec3& center, float height, float baseLength, float baseWidth) {
	if (point.x <= center.x + baseLength / 2 && point.x >= center.x - baseLength / 2 &&
		point.y <= center.y + baseWidth / 2 && point.y >= center.y - baseWidth / 2 &&
		point.z <= center.z + height / 2 && point.z >= center.z - height / 2) return true;
	return false;
}

bool blocksIntersection(const glm::vec3& center1, float height1, float baseLength1, float baseWidth1, const glm::vec3& center2, float height2, float baseLength2, float baseWidth2) {
	bool cond1 = center1.x + baseLength1 / 2 < center2.x - baseLength2 / 2;
	bool cond2 = center2.x + baseLength2 / 2 < center1.x - baseLength1 / 2;
	bool cond3 = center1.y + baseWidth1 / 2 < center2.y - baseWidth2 / 2;
	bool cond4 = center2.y + baseWidth2 / 2 < center1.y - baseWidth1 / 2;
	bool cond5 = center1.z + height1 / 2 < center2.z - height2 / 2;
	bool cond6 = center2.z + height2 / 2 < center1.z - height1 / 2;
	return !(cond1 || cond2 || cond3 || cond4 || cond5 || cond6);
}


bool checkCollisions(glm::vec3 position) {
	// test collisions between asteroid and spaceship
	GameObjectsList::iterator it;

	for (it = gameObjects.trees.begin(); it != gameObjects.trees.end(); ++it) {
		TreeObject* tree = (TreeObject*)(*it);

		if (tree->destroyed == false) {
			// check whether a given asteroid collides with spaceship or not
			// calls spheresIntersection() function with appropriate parameters
			if (blocksIntersection(position, gameObjects.human->height, gameObjects.human->length, gameObjects.human->width,
				tree->position, tree->height, tree->length, tree->width) == true) {
				return true;
			}
		}

	}

	for (it = gameObjects.cottages.begin(); it != gameObjects.cottages.end(); ++it) {
		CottageObject* cottage = (CottageObject*)(*it);

		if (cottage->destroyed == false) {
			// check whether a given asteroid collides with spaceship or not
			// calls spheresIntersection() function with appropriate parameters
			if (blocksIntersection(position, gameObjects.human->height, gameObjects.human->length, gameObjects.human->width,
				cottage->position, cottage->height, cottage->length, cottage->width) == true) {
				return true;
			}
		}
	}

	if (position.x <= -0.1f || position.x >= 11.6f) return true;
	if (position.y >= 0.1 || position.y <= -23) return true;

	return false;
}



void drawWindowContents() {
	glClearColor(0.5f, 0.5f, 0.5f, 1);
	CHECK_GL_ERROR();
	glm::vec3 view;
	glm::mat4 orthoViewMatrix;
	//glm::mat4 viewMatrix;
	//glm::mat4 projectionMatrix;
	// setup parallel projection
	glm::mat4 orthoProjectionMatrix = glm::ortho(
		-SCENE_WIDTH, SCENE_WIDTH,
		-SCENE_HEIGHT, SCENE_HEIGHT,
		-10.0f * SCENE_DEPTH, 10.0f * SCENE_DEPTH
	);
	view = glm::vec3(0.0f, 0.0f, 0.0f) - glm::vec3(0.0f, 0.0f, 1.0f);
		glm::mat4 orthoViewMatrixOrig = glm::lookAt(
			glm::vec3(gameObjects.human->position),
			glm::vec3(gameObjects.human->position) + view,
			glm::vec3(0.0f, 1.0f, 0.0f)
	);
		
		if (gameState.staticCamera1) {
			orthoViewMatrix = glm::lookAt(
				glm::vec3(5, -6, 1),
				glm::normalize(glm::vec3(-1, -1, -2)),
				glm::vec3(0.0f, 0.0f, 1.0f)
			);
		}
		else {
			orthoViewMatrix = glm::lookAt(
				gameObjects.cat->position + glm::vec3(0, -0.1, 0.1),
				gameObjects.cat->position + glm::vec3(0, -1, 0),
				glm::vec3(0.0f, 0.0f, 1.0f)
			);
		}

		gameObjects.terrain->refPos = glm::vec3(gameObjects.human->position);

		glm::mat4 viewMatrix = orthoViewMatrix;
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);
	
	if (gameState.freeCameraMode == true) {

		glm::vec3 cameraPosition;
		glm::vec3 cameraUpVector;
		glm::vec3 cameraCenter;

		glm::vec3 cameraViewDirection = gameObjects.human->direction;
		if (gameState.cameraBug) {
			cameraPosition = gameObjects.bug->position + glm::vec3(0, 0, 0.1f);
			cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);

			cameraViewDirection = gameObjects.bug->direction;

		}
		else if (gameState.movingCamera) {
			float curveParamT = (gameObjects.human->currentTime - gameObjects.human->startTime)/2;
			cameraPosition = glm::vec3(3.5f, -2.0f, 0.1f) + evaluateClosedCurve(curveData, curveSize, curveParamT);
			cameraViewDirection = glm::normalize(evaluateClosedCurve_1stDerivative(curveData, curveSize, curveParamT));
			cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
		}
		else {
			cameraPosition = gameObjects.human->position + glm::vec3(0.0f, 0.0f, 0.1f);
			cameraUpVector = glm::vec3(0.0f, 0.0f, 1.0f);

			cameraViewDirection = gameObjects.human->direction;
		}
		glm::mat4 cameraTransform;

		if (gameState.verticalMoving) {
			glm::vec3 rotationAxisX = glm::cross(cameraViewDirection, glm::vec3(1.0f, 0.0f, 0.0f));
			if (cameraViewDirection.y <= 0)
				cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(gameState.cameraElevationAngle), -rotationAxisX);
			else
				cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(gameState.cameraElevationAngleX), rotationAxisX);
		}
		else {
			glm::vec3 rotationAxis = glm::cross(cameraViewDirection, glm::vec3(0.0f, 0.0f, 1.0f));
			cameraTransform = glm::rotate(glm::mat4(1.0f), glm::radians(gameState.cameraElevationAngle), rotationAxis);
		}
		//printf("x: %f y: %f z; %f\n", cameraViewDirection.x, cameraViewDirection.y, cameraViewDirection.z);
		//printf("Y: %f x: %f\n", gameState.cameraElevationAngle, gameState.cameraElevationAngleX);
		cameraUpVector = glm::vec3(cameraTransform * glm::vec4(cameraUpVector, 0.0f));
		//if(gameState.cameraElevationAngle !=0 || gameState.cameraElevationAngleX !=0)
		cameraViewDirection = glm::vec3(cameraTransform * glm::vec4(cameraViewDirection, 0.0f));
		
		cameraCenter = cameraPosition + cameraViewDirection;

		viewMatrix = glm::lookAt(
			cameraPosition,
			cameraCenter,
			cameraUpVector
		);
		projectionMatrix = glm::perspective(glm::radians(60.0f), gameState.windowWidth / (float)gameState.windowHeight, 0.1f, 10.0f);
	}

	CHECK_GL_ERROR();
	if (gameState.isFog) {
		glUseProgram(shaderProgram.program);
		GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "fog");
		//std::cout << myUniformLocation << "myUnifLoc\n";
		if (myUniformLocation != -1)
			glUniform1i(myUniformLocation, 1);

		glUseProgram(0);
		glUseProgram(skyboxShaderProgram.program);
		myUniformLocation = glGetUniformLocation(skyboxShaderProgram.program, "isFog");
		glUniform1i(myUniformLocation, 1);
		glUseProgram(0);
	}
	else {
		glUseProgram(shaderProgram.program);
		GLint myUniformLocation = glGetUniformLocation(shaderProgram.program, "fog");
		//std::cout << myUniformLocation << "myUnifLoc\n";
		if (myUniformLocation != -1)
			glUniform1i(myUniformLocation, 0);
		glUseProgram(0);

		glUseProgram(skyboxShaderProgram.program);
		myUniformLocation = glGetUniformLocation(skyboxShaderProgram.program, "isFog");
		glUniform1i(myUniformLocation, 0);
		glUseProgram(0);
	}

	CHECK_GL_ERROR();
	glUseProgram(shaderProgram.program);
	glUniform1f(shaderProgram.timeLocation, gameState.elapsedTime);
	glUniform1f(shaderProgram.fastenTimeLocation, gameState.fastenTime);
	glUniform3fv(shaderProgram.reflectorPositionLocation, 1, glm::value_ptr(gameObjects.human->position));
	glUniform3fv(shaderProgram.reflectorDirectionLocation, 1, glm::value_ptr(gameObjects.human->direction));
	glUseProgram(0);

	glUseProgram(terrainShaderProgram.program);
	glUniform1f(terrainShaderProgram.timeLocation, gameState.elapsedTime);
	glUniform1f(terrainShaderProgram.fastenTimeLocation, gameState.fastenTime);
	glUseProgram(0);

	glUseProgram(skyboxShaderProgram.program);
	glUniform1f(skyboxShaderProgram.timeLocation, gameState.elapsedTime);
	glUniform1f(skyboxShaderProgram.fastenTimeLocation, gameState.fastenTime);
	glUseProgram(0);

	initShaderDirectionalLight(shaderProgram, directionalLight);
	initShaderPointLight(shaderProgram, pointLight);
	initShaderSpotLight(shaderProgram, spotLight);

	CHECK_GL_ERROR();
	// draw space ship

	drawSkybox(viewMatrix, projectionMatrix);
	drawTerrain(gameObjects.terrain, viewMatrix, projectionMatrix, gameObjects.human->position, gameObjects.human->direction, gameState.isFog);
	drawHuman(gameObjects.human, viewMatrix, projectionMatrix);

	CHECK_GL_ERROR();

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	int id = 1;
	for (GameObjectsList::iterator it = gameObjects.stones.begin(); it != gameObjects.stones.end(); ++it) {
		glStencilFunc(GL_ALWAYS, id, -1);
		StoneObject* stone = (StoneObject*)(*it);
		drawStone(stone, viewMatrix, projectionMatrix);
		id++;
	}
	for (GameObjectsList::iterator it = gameObjects.trees.begin(); it != gameObjects.trees.end(); ++it) {
		CHECK_GL_ERROR();

		glStencilFunc(GL_ALWAYS, id, -1);

		TreeObject* tree = (TreeObject*)(*it);
		drawTree(tree, viewMatrix, projectionMatrix);
		id++;
	}

	for (GameObjectsList::iterator it = gameObjects.logs.begin(); it != gameObjects.logs.end(); ++it) {
		CHECK_GL_ERROR();

		glStencilFunc(GL_ALWAYS, id, -1);
		LogObject* log = (LogObject*)(*it);
		drawLog(log, viewMatrix, projectionMatrix);
		id++;
	}

	glDisable(GL_STENCIL_TEST);

	for (GameObjectsList::iterator it = gameObjects.cottages.begin(); it != gameObjects.cottages.end(); ++it) {
		CHECK_GL_ERROR();

		CottageObject* cottage = (CottageObject*)(*it);
		drawCottage(cottage, viewMatrix, projectionMatrix);
	}

	glDisable(GL_DEPTH_TEST);
	if (gameObjects.fire != NULL)
		drawFire(gameObjects.fire, viewMatrix, projectionMatrix);
	if (gameObjects.statusBar != NULL)
		drawStatusBar(gameObjects.statusBar, viewMatrix, projectionMatrix);

	glEnable(GL_DEPTH_TEST);

	drawCat(gameObjects.cat, viewMatrix, projectionMatrix);
	drawBug(gameObjects.bug, viewMatrix, projectionMatrix);
	drawGlass(gameObjects.glass, viewMatrix, projectionMatrix);
	if (gameObjects.moon != NULL && gameState.fastenTime)
		drawMoon(gameObjects.moon, orthoViewMatrixOrig, orthoProjectionMatrix);

	CHECK_GL_ERROR();
}

// Called to update the display. You should call glutSwapBuffers after all of your
// rendering to display what you rendered.
void displayCallback() {
	GLbitfield mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	mask |= GL_STENCIL_BUFFER_BIT;

	glClear(mask);
	drawWindowContents();

	glutSwapBuffers();
}

// Called whenever the window is resized. The new window size is given, in pixels.
// This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void reshapeCallback(int newWidth, int newHeight) {

	gameState.windowWidth = newWidth;
	gameState.windowHeight = newHeight;

	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
}

void getHeightOfTerrain(float worldX, float worldY, float* terrainX, float* terrainY) {
	*terrainX = worldX - gameObjects.terrain->position.x;
	*terrainY = worldY - gameObjects.terrain->position.y;
}

void updateObjects(float elapsedTime) {

	float timeDelta = elapsedTime - gameObjects.human->currentTime;
	gameObjects.human->currentTime = elapsedTime;
	//float terrainX, terrainY;
	//getHeightOfTerrain(gameObjects.human->position.x, gameObjects.human->position.y, &terrainX, &terrainY);
	//float height = MIN_HEIGHT;
	//height += heightData[int(gameObjects.human->position.x) * 10][int(gameObjects.human->position.y) * 10];

	//gameObjects.human->position = glm::vec3(gameObjects.human->position.x, gameObjects.human->position.y,height*0.2);
	if (!checkCollisions(gameObjects.human->position + timeDelta * gameObjects.human->speed * gameObjects.human->direction))
		gameObjects.human->position += timeDelta * gameObjects.human->speed * gameObjects.human->direction;
	if (gameObjects.moon != NULL)
		gameObjects.moon->position = gameObjects.human->position + glm::vec3(0.5f, 0.5f, 0);
	//printf("x: %f y: %f z: %f\n", gameObjects.human->position.x, gameObjects.human->position.y, gameObjects.human->position.z);

	gameObjects.terrain->currentTime = elapsedTime;
	gameObjects.cat->currentTime = elapsedTime;

	if (gameObjects.fire != NULL) {
		gameObjects.fire->currentTime = elapsedTime;
		if (gameObjects.fire->destroyed == true) {
			gameObjects.fire = NULL;
		}
	}

	if (gameObjects.statusBar != NULL) {
		gameObjects.statusBar->currentTime = elapsedTime;
		//std::cout << fmod(gameObjects.statusBar->currentTime - gameObjects.statusBar->startTime, FIRE_BAR_TIME) << " elapsed time\n";
		if (fmod(gameObjects.statusBar->currentTime - gameObjects.statusBar->startTime, FIRE_BAR_TIME) < 1)
			textureChanged = false;
		if (fmod(gameObjects.statusBar->currentTime - gameObjects.statusBar->startTime, FIRE_BAR_TIME) > FIRE_BAR_TIME-0.1f && !textureChanged) {
			textureChanged = true;
			if (gameObjects.statusBar->numOfTexture + 1 > 3) {
				gameObjects.statusBar->log->destroyed = true;
				gameObjects.statusBar = NULL;
				gameObjects.fire = NULL;
				spotLight.turnedOn = false;
			}
			else
				gameObjects.statusBar->numOfTexture++;
		}
	}

	GameObjectsList::iterator it = gameObjects.trees.begin();
	while (it != gameObjects.trees.end()) {
		TreeObject* tree = (TreeObject*)(*it);

		if (tree->destroyed == true) {
			it = gameObjects.trees.erase(it);
		}
		else {
			++it;
		}
	}

	it = gameObjects.logs.begin();
	while (it != gameObjects.logs.end()) {
		LogObject* log = (LogObject*)(*it);

		if (log->destroyed == true) {
			it = gameObjects.logs.erase(it);
		}
		else {
			++it;
		}
	}
	if (!gameState.bugMoves) {
		if (glm::length(glm::vec3(gameObjects.human->position - gameObjects.bug->position)) < 1)
			gameState.bugMoves = true;
	}
	if (gameState.bugMoves) {
		angle += 0.04f;
		angle = fmod(angle, 360);
		gameObjects.bug->position.x = gameObjects.bug->initPosition.x + cos(angle) / 2;
		gameObjects.bug->position.y = gameObjects.bug->initPosition.y + 0.5f + sin(angle) / 2;
		gameObjects.bug->direction.x = -sin(angle) / 2;
		gameObjects.bug->direction.y = cos(angle) / 2;

	}

	if ((gameObjects.moon != NULL)) {
		gameObjects.moon->currentTime = gameState.elapsedTime;
	}
	if (gameObjects.moon == NULL && gameState.fastenTime) {
		// if game over and banner still not created -> create banner
		gameObjects.moon = createMoon();
	}

	pointLight.position = gameObjects.human->position;
	pointLight.direction = gameObjects.human->direction;

}

void timerCallback(int) {

	// update scene time
	gameState.elapsedTime = 0.001f * (float)glutGet(GLUT_ELAPSED_TIME); // milliseconds => seconds

	// call appropriate actions according to the currently pressed keys in key map
	// (combinations of keys are supported but not used in this implementation)
	if (gameState.keyMap[KEY_RIGHT_ARROW] == true)
		turnHumanRight(SPACESHIP_VIEW_ANGLE_DELTA);

	if (gameState.keyMap[KEY_LEFT_ARROW] == true)
		turnHumanLeft(SPACESHIP_VIEW_ANGLE_DELTA);

	if (gameState.keyMap[KEY_UP_ARROW] == true)
		increaseHumanSpeed();

	if (gameState.keyMap[KEY_DOWN_ARROW] == true)
		decreaseHumanSpeed();

	updateObjects(gameState.elapsedTime);

	glutTimerFunc(33, timerCallback, 0);

	glutPostRedisplay();
}

// Called when mouse is moving while no mouse buttons are pressed.
void passiveMouseMotionCallback(int mouseX, int mouseY) {
	
	if (mouseY != gameState.windowHeight / 2) {

		float cameraElevationAngleDelta = 0.5f * (mouseY - gameState.windowHeight / 2);

		if (fabs(gameState.cameraElevationAngle + cameraElevationAngleDelta) < CAMERA_ELEVATION_MAX)
			gameState.cameraElevationAngle += cameraElevationAngleDelta;
		
	}
	if (mouseX != gameState.windowWidth / 2) {

		float cameraElevationAngleDelta = 0.5f * (mouseX - gameState.windowWidth / 2);

		if (fabs(gameState.cameraElevationAngleX + cameraElevationAngleDelta) < CAMERA_ELEVATION_MAX)
			gameState.cameraElevationAngleX += cameraElevationAngleDelta;

	}
	glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);

	glutPostRedisplay();
}

void mouseCallback(int buttonPressed, int buttonState, int mouseX, int mouseY) {

	
	if ((buttonPressed == GLUT_LEFT_BUTTON) && (buttonState == GLUT_DOWN)) {

		
		unsigned char objID = 0;
		
		glReadPixels(mouseX, gameState.windowHeight - mouseY - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE,
			&objID);
		if (objID == 0) {
			printf("Clicked on background\n");
		}
		else if (objID <= STONES_COUNT_MIN) {
			printf("Clicked on stone with ID: %d\n", (int)objID);
			if (gameState.logs > 0) {
				printf("creating log\n");
				LogObject* newLog = createLog(glm::vec3(2.98f, -4.01f, -0.165f));
				newLog->size = newLog->size * 2;
				gameObjects.logs.push_back(newLog);
				gameState.logs--;
				startFire(glm::vec3(2.98f, -4.01f, 0));
				createStatusBar(glm::vec3(2.98f, -4.01f, 0.2), newLog);
				spotLight.position = glm::vec3(2.98f, -4.01f, -0.1);
				spotLight.turnedOn = true;
			}
		}
		else if (objID <= gameObjects.trees.size() + STONES_COUNT_MIN) {
			printf("Clicked on tree with ID: %d\n", (int)objID);

			GameObjectsList::iterator it = gameObjects.trees.begin();
			std::advance(it, objID - STONES_COUNT_MIN - 1); // now it points to the nth asteroid of the list (counting from zero)
			TreeObject* tree = (TreeObject*)(*it);

			if (tree->destroyed == false) {
				tree->destroyed = true;	          // remove asteroid
				LogObject* newLog = createLog(tree->position - glm::vec3(0, 0, 0.225f));

				gameObjects.logs.push_back(newLog);
			}
		}
		else {
			printf("Clicked on log with ID: %d\n", (int)objID);
			GameObjectsList::iterator it = gameObjects.logs.begin();
			std::advance(it, objID - gameObjects.trees.size() - STONES_COUNT_MIN - 1); // now it points to the nth asteroid of the list (counting from zero)
			LogObject* log = (LogObject*)(*it);
			double worldX, worldY, worldZ;
			if (log->destroyed == false) {
				log->destroyed = true;
				gameState.logs++;
			}
		}
	}
}

void keyboardCallback(unsigned char keyPressed, int mouseX, int mouseY) {

	switch (keyPressed) {
	case 27: // escape
#ifndef __APPLE__
		glutLeaveMainLoop();
#else
		exit(0);
#endif
		break;
	case 'r': // restart game
		restartGame();
		break;
	case 'c': // switch camera
		gameState.cameraBug = false;
		gameState.freeCameraMode = !gameState.freeCameraMode;
		if (gameState.freeCameraMode == true) {
			glutPassiveMotionFunc(passiveMouseMotionCallback);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		}
		else {
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			glutPassiveMotionFunc(NULL);
		}
		break;
	case 'x': //speed up time
		if (!gameState.fastenTime)
			gameState.fastenTime = true;
		else
			gameState.fastenTime = false;
		break;
	case 'm':
		if (!gameState.movingCamera) {
			gameState.freeCameraMode = true;
			glutPassiveMotionFunc(passiveMouseMotionCallback);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			gameState.movingCamera = true;
		}
		else {
			gameState.freeCameraMode = false;
			glutPassiveMotionFunc(NULL);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			gameState.movingCamera = false;
		}
		break;
	case 'b': // switch camera

		if (gameState.cameraBug) {
			gameState.freeCameraMode = false;
			glutPassiveMotionFunc(NULL);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			gameState.cameraBug = false;

		}
		else {
			gameState.freeCameraMode = true;
			glutPassiveMotionFunc(passiveMouseMotionCallback);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			gameState.cameraBug = true;
		}

		break;
	case'f': {
		if (gameState.isFog)
			gameState.isFog = false;
		else
			gameState.isFog = true;
		break;
	}
	case'v': 
		gameState.verticalMoving = !gameState.verticalMoving;
		gameState.cameraElevationAngle = 0;
		gameState.cameraElevationAngleX = 0;
		break;
	default:
		printf("Unrecognized key pressed\n");
	}
}

// Called whenever a key on the keyboard was released. The key is given by
// the "keyReleased" parameter, which is in ASCII. 
void keyboardUpCallback(unsigned char keyReleased, int mouseX, int mouseY) {

	switch (keyReleased) {
	case ' ':
		gameState.keyMap[KEY_SPACE] = false;
		break;
	default:
		; // printf("Unrecognized key released\n");
	}
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are pressed.
void specialKeyboardCallback(int specKeyPressed, int mouseX, int mouseY) {

	if (gameState.gameOver == true)
		return;

	switch (specKeyPressed) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = true;
		break;
	default:
		; // printf("Unrecognized special key pressed\n");
	}
}

// The special keyboard callback is triggered when keyboard function or directional
// keys are released.
void specialKeyboardUpCallback(int specKeyReleased, int mouseX, int mouseY) {

	if (gameState.gameOver == true)
		return;

	switch (specKeyReleased) {
	case GLUT_KEY_RIGHT:
		gameState.keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		gameState.keyMap[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP:
		gameState.keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		gameState.keyMap[KEY_DOWN_ARROW] = false;
		break;
	default:
		; // printf("Unrecognized special key released\n");
	}
}

void menu(int param) {
	bool wasPrev;
	switch (param) {
	case 6: // restart game
		restartGame();
		break;
	case 1: // switch camera
		gameState.cameraBug = false;
		if (gameState.freeCameraMode)
			gameState.freeCameraMode = false;
		if (gameState.staticCamera1)
			gameState.freeCameraMode = true;
		gameState.staticCamera1 = !gameState.staticCamera1;
		if (gameState.freeCameraMode == true) {
			glutPassiveMotionFunc(passiveMouseMotionCallback);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		}
		else {
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			glutPassiveMotionFunc(NULL);
		}
		break;
	case 7: // switch camera2
		gameState.cameraBug = false;
		wasPrev = gameState.freeCameraMode;
		if (gameState.freeCameraMode)
			gameState.freeCameraMode = false;
		if (!gameState.staticCamera1 && !wasPrev)
			gameState.freeCameraMode = true;
		else if(gameState.staticCamera1 && !wasPrev)
			gameState.staticCamera1 = false;
		if (gameState.freeCameraMode == true) {
			glutPassiveMotionFunc(passiveMouseMotionCallback);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		}
		else {
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			glutPassiveMotionFunc(NULL);
		}
		break;
	case 2: //speed up time
		if (!gameState.fastenTime)
			gameState.fastenTime = true;
		else
			gameState.fastenTime = false;
		break;
	case 3:
		gameState.cameraBug = false;
		gameState.staticCamera1 = false;
		if (!gameState.movingCamera) {
			gameState.freeCameraMode = true;
			glutPassiveMotionFunc(passiveMouseMotionCallback);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			gameState.movingCamera = true;
		}
		else {
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			gameState.freeCameraMode = false;
			glutPassiveMotionFunc(NULL);
			gameState.movingCamera = false;
		}
		break;
	case 4: // switch camera
		gameState.movingCamera = false;
		gameState.staticCamera1 = false;
		if (gameState.cameraBug) {
			gameState.freeCameraMode = false;
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			glutPassiveMotionFunc(NULL);
			gameState.cameraBug = false;
		}
		else {
			gameState.freeCameraMode = true;
			glutPassiveMotionFunc(passiveMouseMotionCallback);
			glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
			gameState.cameraBug = true;
		}

		break;
	case 5:
		if (gameState.isFog)
			gameState.isFog = false;
		else
			gameState.isFog = true;
		break;
	case 8:
		gameState.staticCamera1 = false;
		gameState.cameraBug = false;
		gameState.movingCamera = false;
		gameState.freeCameraMode = true;
		glutPassiveMotionFunc(passiveMouseMotionCallback);
		//glutWarpPointer(gameState.windowWidth / 2, gameState.windowHeight / 2);
		break;
	}
	glutPostRedisplay();
}

void createMenu(void) {
	int submenuCamera = glutCreateMenu(menu);
	glutAddMenuEntry("Static camera 1 ON/OFF", 1);
	glutAddMenuEntry("Moving camera ON/OFF", 3);
	glutAddMenuEntry("Bug camera ON/OFF", 4);
	glutAddMenuEntry("Static camera cat ON/OFF", 7);
	glutAddMenuEntry("Human camera", 8);

	int submenuNight = glutCreateMenu(menu);
	glutAddMenuEntry("Night ON/OFF", 2);

	int submenuFog = glutCreateMenu(menu);
	glutAddMenuEntry("Fog ON/OFF", 5);

	int submenuRestart = glutCreateMenu(menu);
	glutAddMenuEntry("Restart Scene", 6);

	glutCreateMenu(menu);
	glutAddSubMenu("Camera", submenuCamera);
	glutAddSubMenu("Fog", submenuFog);
	glutAddSubMenu("Night", submenuNight);
	glutAddSubMenu("Restart", submenuRestart);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

