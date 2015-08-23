#define GLM_FORCE_RADIANS
#define _USE_MATH_DEFINES

#include <WinSock2.h>
#include <Windows.h>

#include "OpenGL/glew.h"
#include "OpenGL/GLU.h"
#include "OpenGL/GL.h"
#include "SDL2-2.0.3\include\SDL.h"
#include "SDL2-2.0.3\include\SDL_thread.h"
#include "SDL2-2.0.3\include\SDL_opengl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"	
#include "btBulletDynamicsCommon.h"
#include "CollisionObject.h"

#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

#include "InputManager.h"
#include "Camera.h"
#include "Utilities.h"
#include "Settings.h"
#include "AssetManager.h"
#include "SoundManager.h"
#include "Shader.h"
#include "Texture.h"
#include "Model.h"
#include "GameObject.h"
//#include "FrameBuffer.h"
#include "PhysicsObject.h"
#include "PropObject.h"
#include "BoxObject.h"
#include "HeightFieldObject.h"
#include "PlayerObject.h"
#include "Man.h"
#include "Soldier.h"
//#include "AI.h"
//#include "ClientThread.h"

#define DEBUG 0

#if DEBUG
	#include <vld.h>
#endif

#define FULLSCREEN	0
#define WINDOWED	1
#define BORDERLESS	2

using std::string;
using std::getline;
using std::stringstream;
using std::ostringstream;
using std::ifstream;
using std::ofstream;
using namespace Utilities;

class Game
{
public:
	Game(char* gameName);
	~Game(void);

	int init();
	int start();
	int cleanup();

private:
	int update(long elapsedTime);
	int renderFrame(long time);
	int render2D(long time);
	int loadLevel();

	void addPlane(glm::vec2 pos, glm::vec2 normal);

	void loadConfig();
	void saveConfig();
	
	int setViewport(int width, int height);
	int resizeWindow(int width, int height);
	void startOrtho();
	void endOrtho();

	GLvoid buildFont();
	GLvoid killFont();
	int drawText(int x, int y, const char * s);

	int initOpenGL();
	int initGlew();
	int initSDL();
	int initGLContexts();
	int initObjects();
	int cleanupObjects();
	int killSDL();

	int initBullet(void);
	int cleanBullet(void);
	int killBullet(void);
	int applyCustomContacts();

	//Shortcut Functions
	inline bool isKeyDown(long e) { return inputManager.isKeyDown(e); }
	inline bool isKeyUp(long e) { return inputManager.isKeyUp(e); }
	inline bool isKeyPressed(long e) { return inputManager.isKeyPressed(e); }
	inline bool isKeyReleased(long e) { return inputManager.isKeyReleased(e); }
	#define activeShader Shader::getActiveShader()
	#define activeCamera Camera::getActiveCamera()

	bool activeWindow, running;
	short bits;
	int flags;
	char title[128];
	bool wireFrame;
	void toggleWireFrame();
	
	HDC diplayWindow;
	HGLRC primaryContext, loadingThreadContext;
	SDL_Window * displayWindow;
	SDL_GLContext displayContext;
	SDL_Thread * assetLoaderThread;
	static int startAssetLoader(void*);
	HANDLE clientThreadHandle;

	GLuint base;
	long time;
	InputManager inputManager;
	static AssetManager * assetManager;
	static SoundManager * soundManager;
	list<GameObject *> * gameObjects;
	list<GameObject *> * deadObjects;
	PlayerObject * player;
	Character * char1;

	GameObject * testbox;
	GameObject * testbox2;
	GameObject * testbox3;
	GameObject * testbox4;
	GameObject * testbox5;

	//Client * client;
	//void setUsername(string x);
	//void changeUsername(string x);

	//Bullet Variables
	btAlignedObjectArray<btCollisionShape*>	collisionShapes;
	btDefaultCollisionConfiguration * collisionConfiguration;
	btCollisionDispatcher * dispatcher;
	btBroadphaseInterface * broadphase;
	btSequentialImpulseConstraintSolver * solver;
	btDiscreteDynamicsWorld * dynamicsWorld;
	static bool myContactProcessedCallback(btManifoldPoint& cp, void * body0, void * body1);
	void initTestCube();

	double frameRate;
	#define MAXFRAMESAMPLES 100
	int tickindex;
	double ticksum;
	double ticklist[MAXFRAMESAMPLES];

	double calcFps(double newtick);

	glm::mat4 view, projection;
	Camera camera;

	int state;
	Model * quad;
	Texture * startScreen;
	Texture * errorTexture;
	Shader * defaultShader;
	Shader * textShader;
	Shader * quadShader;

	int level;
	list<GameObject*> * webbedMen;
	glm::vec2 webPos;
	int calcNumCaptured();
	int numCaptured, numRequired;
	
	void draw3DRect(float x, float y, float z);
	void draw3DRect(float x, float y, float z, float rotation);
	void draw3DRect(glm::vec3 lookAt, glm::vec3 position, glm::vec3 up, glm::vec3 size);
	void draw2DRect(float x, float y, float width, float height, Texture * t);
	void draw2DRect(float x, float y, int width, int height, Texture * t);
	void draw2DRect(int x, int y, float width, float height, Texture * t);
	void draw2DRect(int x, int y, int width, int height, Texture * t);
};