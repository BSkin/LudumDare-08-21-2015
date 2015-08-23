#include "Game.h"

AssetManager * Game::assetManager = NULL;
SoundManager * Game::soundManager = NULL;

void initConsole()
{
	/*AllocConsole();

	long lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	int hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	FILE *fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	fclose(fp);*/
}

void deleteConsole()
{
	//FreeConsole();
}

Game::Game(char* gameName)
{
	title[127] = '\0';
	if (title == NULL) this->title[0] = '\0';
	else strncpy(this->title, gameName,127);
	frameRate = 0;
	activeWindow = false;
	running = true;
	bits = 32;
	flags = 0;
	title[128];
	base = 0;
	player = 0;
	
	#if DEBUG
		initConsole();
	#endif

	displayWindow =	NULL;
	displayContext = NULL;
	assetLoaderThread = NULL;

	state = 0;

	tickindex=ticksum=0;
	for (int i = 0; i < MAXFRAMESAMPLES; i++) ticklist[i] = 0;
}

Game::~Game()
{
	#if DEBUG
		deleteConsole();
	#endif
}

void Game::loadConfig()
{
	if (!dirExists("Config")) {
		SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, false};
		CreateDirectory("Config", &sa);
	}

	FILE * file = NULL;
	char * configName = "Config\\settings.ini";
	file = fopen(configName, "r");
	if (file == NULL) {
		fclose(file);
		return;
	} //no file found, just use defaults
	fclose(file);

	ifstream fileStream(configName);
	string s;
	string var, val;
	while (fileStream.peek() != -1)
	{
		s = var = val = "";
		getline(fileStream, s);
		bool eq = false;
		int i = 0;

		while (i < s.size())
		{
			if (s[i] == '=')	eq = true;
			else if (eq)		val += s[i];
			else				var += s[i];
			i++;
		}

		if (var == "maxFps")			Settings::setMaxFPS(atoi(val.c_str()));
		else if (var == "windowWidth")	Settings::setWindowWidth(atoi(val.c_str()));
		else if (var == "windowHeight")	Settings::setWindowHeight(atoi(val.c_str()));
		else if (var == "windowState")	Settings::setWindowState(atoi(val.c_str()));
	}
	
	fileStream.close();
}

void Game::saveConfig()
{
	ofstream output;
	char * configName = "Config\\settings.ini";
	output.open(configName);
	
	const unsigned char * ver = glGetString(GL_VERSION);
	string v = "";
	int i = 0;
	while (ver[i] != ' ') { v += ver[i]; i++; }

	float version = atof(v.c_str());

	output 
		<< "maxFps=" << Settings::getMaxFPS() << '\n'
		<< "windowWidth=" << Settings::getWindowWidth() << '\n'
		<< "windowHeight=" << Settings::getWindowHeight() << '\n'
		<< "windowState=" << Settings::getWindowState();

	output.close();
}

GLvoid Game::buildFont()
{
	HFONT   font;                       // Windows Font ID
    HFONT   oldfont;                    // Used For Good House Keeping
 
    base = glGenLists(96);                  // Storage For 96 Characters ( NEW )

	font = CreateFont(  
		-24,						//Height Of Font ( NEW ) -indicates height instead of width
		0,							//Width Of Font
		0,							//Angle Of Escapement
		0,							//Orientation Angle
		FW_NORMAL,					//Font Weight
		FALSE,						//Italic
		FALSE,						//Underline
		FALSE,						//Strikeout
		OUT_TT_PRECIS,				// Output Precision
		ANSI_CHARSET,				//Character Set Identifier
		CLIP_DEFAULT_PRECIS,		//Clipping Precision
		ANTIALIASED_QUALITY,		//Output Quality
		FF_DONTCARE|DEFAULT_PITCH,	//Family And Pitch
		"Arial"); 				//Font Name

	HDC deviceContext = GetDC (NULL);

	oldfont = (HFONT)SelectObject(deviceContext, font);       // Selects The Font We Want
    wglUseFontBitmaps(deviceContext, 32, 96, base);           // Builds 96 Characters Starting At Character 32
    SelectObject(deviceContext, oldfont);             // Selects The Font We Want
    DeleteObject(font);                 // Delete The Font
}

GLvoid Game::killFont()
{
    glDeleteLists(base, 96);                // Delete All 96 Characters ( NEW )
}

int Game::drawText(int x, int y, const char * s)
{
	if (s == NULL) return -1;
	
	char        text[256];              // Holds Our String
	va_list     ap;                 // Pointer To List Of Arguments
	
	va_start(ap, s);                  // Parses The String For Variables
		vsprintf(text, s, ap);                // And Converts Symbols To Actual Numbers
	va_end(ap);                     // Results Are Stored In Text
	
	//Determine the end of the string and convert each occurrence of '\n' to '\0'.
	char *end = text + strlen (text);
	for (char *next = text; *next != '\0'; next++) {
		if (*next == '\n') *next = '\0';
	}

	glColor4f(0.95f, 0.95f, 0.8f, 1.0f);

	long yOffset = y;
	for (char *line = text; line < end; line += strlen (line) + 1) {
		glWindowPos2i(x, y);//(yOffset-=32));
		//glRasterPos2i (x, yOffset); yOffset -= 32;
		glPushAttrib(GL_LIST_BIT);              // Pushes The Display List Bits     ( NEW )
		glListBase(base - 32);                  // Sets The Base Character to 32    ( NEW )

		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);  // Draws The Display List Text  ( NEW )
		glPopAttrib();     
	}

	return 0;
}

int Game::init()
{
	loadConfig();
	initSDL();
	initGlew();
	initBullet();
	initGLContexts();
	initObjects();
	time = 0;

	return 0;
}

int Game::initSDL()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	Uint32 windowFlags;

	if (Settings::getWindowState() == FULLSCREEN)		windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN;
	else if (Settings::getWindowState() == WINDOWED)	windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
	else if (Settings::getWindowState() == BORDERLESS) windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS;

	displayWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Settings::getWindowWidth(), Settings::getWindowHeight(), windowFlags);
	
	displayContext = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, displayContext); 
	
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); 

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, bits);
	
    initOpenGL();
    resizeWindow(Settings::getWindowWidth(), Settings::getWindowHeight());

	SDL_SetRelativeMouseMode(SDL_FALSE);
    
	return 0;
}

void Game::toggleWireFrame()
{
	wireFrame = !wireFrame;
	if (wireFrame)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

int Game::initOpenGL()
{
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);  // clockwise oriented polys are front faces
	glCullFace(GL_BACK); // cull out the inner polygons... Set Culling property appropriately

	glShadeModel( GL_SMOOTH );							// Enable smooth shading
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );				// Set the background black
	glClearDepth( 1.0f );								// Depth buffer setup
	glEnable( GL_DEPTH_TEST );							// Enables Depth Testing
	glDepthFunc( GL_LEQUAL );							// The Type Of Depth Test To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);			// filled polys
	wireFrame = false;

	buildFont();

	return 0;
}

int Game::initGLContexts()
{
	diplayWindow = wglGetCurrentDC();
	primaryContext = wglGetCurrentContext();
	loadingThreadContext = wglCreateContext(diplayWindow);
	wglMakeCurrent(NULL, NULL);
	bool error = wglShareLists(primaryContext, loadingThreadContext);
	wglMakeCurrent(diplayWindow, primaryContext);

	return 0;
}

int Game::initObjects()
{
	assetManager = new AssetManager();
	assetManager->setStatics(&diplayWindow, &loadingThreadContext, &primaryContext);
	soundManager = new SoundManager();
	//assetLoaderThread = SDL_CreateThread(startAssetLoader, "AssetLoaderThread", (void*)NULL);
	inputManager.init(&time);
	gameObjects = new list<GameObject*>();
	deadObjects = new list<GameObject*>();
	webbedMen = new list<GameObject*>();
	assetManager->forceLoadModel("Models\\quad.obj");
	RenderObject::setStatics(&view, &projection, assetManager->getModel("Models\\quad.obj"), assetManager, soundManager);
	GameObject::setStatics(gameObjects);
	PhysicsObject::setStatics(&collisionShapes, dynamicsWorld);
	PlayerObject::setStatics(&inputManager, &camera, webbedMen);
	RigidObject::populateCollisionLibrary();
	//Event::setStatics();
	/*client = new Client();
	ClientThread::setStatics(client);
	setUsername(Settings::getUsername());
	*/
	camera.activate();

	return 0;
}

int Game::cleanupObjects()
{
	webbedMen->clear();
	delete webbedMen;

	while (gameObjects->size() > 0) {
		delete gameObjects->front();
		gameObjects->pop_front();
	}

	delete gameObjects;
	deadObjects->clear();
	delete deadObjects;

	RigidObject::deleteCollisionLibrary();
	assetManager->shutdown();
	SDL_WaitThread(assetLoaderThread, NULL);
	delete soundManager;
	delete assetManager;
	return 0;
}

int Game::initGlew()
{
	static bool alreadyExecuted = false;
	if (alreadyExecuted) return -1;
	alreadyExecuted = true;

	const bool loggingExtensions = true; //Set to true to have extensions logged...
	char *extensions = (char *) glGetString (GL_EXTENSIONS);

	//bool isARBVertexBufferObjectExtensionPresent = isExtensionSupported (extensions, "GL_ARB_vertex_buffer_object");
	//if (isARBVertexBufferObjectExtensionPresent) {
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress ("glBindBufferARB");
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress ("glDeleteBuffersARB");
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress ("glGenBuffersARB");
		glIsBufferARB = (PFNGLISBUFFERARBPROC) wglGetProcAddress ("glIsBufferARB");
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress ("glBufferDataARB");
		glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC) wglGetProcAddress ("glBufferSubDataARB");
		glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC) wglGetProcAddress ("glGetBufferSubDataARB");
		glMapBufferARB = (PFNGLMAPBUFFERARBPROC) wglGetProcAddress ("glMapBufferARB");
		glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC) wglGetProcAddress ("glUnmapBufferARB");
		glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC) wglGetProcAddress ("glGetBufferParameterivARB");
		glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC) wglGetProcAddress ("glGetBufferPointervARB");
	//}
	
	//bool isGL_ARB_shader_objectsExtensionPresent = isExtensionSupported (extensions, "GL_ARB_shader_objects");
	//if (isGL_ARB_shader_objectsExtensionPresent) {
		glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress ("glAttachObjectARB");
		glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress ("glCompileShaderARB");
		glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress ("glCreateProgramObjectARB");
		glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress ("glCreateShaderObjectARB");
		glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress ("glDeleteObjectARB");
		glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)wglGetProcAddress ("glDetachObjectARB");
		glGetActiveUniformARB = (PFNGLGETACTIVEUNIFORMARBPROC)wglGetProcAddress ("glGetActiveUniformARB");
		glGetAttachedObjectsARB = (PFNGLGETATTACHEDOBJECTSARBPROC)wglGetProcAddress ("glGetAttachedObjectsARB");
		glGetHandleARB = (PFNGLGETHANDLEARBPROC)wglGetProcAddress ("glGetHandleARB");
		glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress ("glGetInfoLogARB");
		glGetObjectParameterfvARB = (PFNGLGETOBJECTPARAMETERFVARBPROC)wglGetProcAddress ("glGetObjectParameterfvARB");
		glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress ("glGetObjectParameterivARB");
		glGetShaderSourceARB = (PFNGLGETSHADERSOURCEARBPROC)wglGetProcAddress ("glGetShaderSourceARB");
		glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress ("glGetUniformLocationARB");
		glGetUniformfvARB = (PFNGLGETUNIFORMFVARBPROC)wglGetProcAddress ("glGetUniformfvARB");
		glGetUniformivARB = (PFNGLGETUNIFORMIVARBPROC)wglGetProcAddress ("glGetUniformivARB");
		glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress ("glLinkProgramARB");
		glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress ("glShaderSourceARB");
		glUniform1fARB = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress ("glUniform1fARB");
		glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)wglGetProcAddress ("glUniform1fvARB");
		glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress ("glUniform1iARB");
		glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)wglGetProcAddress ("glUniform1ivARB");
		glUniform2fARB = (PFNGLUNIFORM2FARBPROC)wglGetProcAddress ("glUniform2fARB");
		glUniform2fvARB = (PFNGLUNIFORM2FVARBPROC)wglGetProcAddress ("glUniform2fvARB");
		glUniform2iARB = (PFNGLUNIFORM2IARBPROC)wglGetProcAddress ("glUniform2iARB");
		glUniform2ivARB = (PFNGLUNIFORM2IVARBPROC)wglGetProcAddress ("glUniform2ivARB");
		glUniform3fARB = (PFNGLUNIFORM3FARBPROC)wglGetProcAddress ("glUniform3fARB");
		glUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)wglGetProcAddress ("glUniform3fvARB");
		glUniform3iARB = (PFNGLUNIFORM3IARBPROC)wglGetProcAddress ("glUniform3iARB");
		glUniform3ivARB = (PFNGLUNIFORM3IVARBPROC)wglGetProcAddress ("glUniform3ivARB");
		glUniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress ("glUniform4fARB");
		glUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)wglGetProcAddress ("glUniform4fvARB");
		glUniform4iARB = (PFNGLUNIFORM4IARBPROC)wglGetProcAddress ("glUniform4iARB");
		glUniform4ivARB = (PFNGLUNIFORM4IVARBPROC)wglGetProcAddress ("glUniform4ivARB");
		glUniformMatrix2fvARB = (PFNGLUNIFORMMATRIX2FVARBPROC)wglGetProcAddress ("glUniformMatrix2fvARB");
		glUniformMatrix3fvARB = (PFNGLUNIFORMMATRIX3FVARBPROC)wglGetProcAddress ("glUniformMatrix3fvARB");
		glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)wglGetProcAddress ("glUniformMatrix4fvARB");
		glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress ("glUseProgramObjectARB");
		glValidateProgramARB = (PFNGLVALIDATEPROGRAMARBPROC)wglGetProcAddress ("glValidateProgramARB");

		// Other Shader Stuff
		glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress ("glCompileShader");
		glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress ("glCreateProgram");
		glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress ("glCreateShader");
		glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress ("glDeleteProgram");
		glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress ("glDeleteShader");
		glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress ("glDetachShader");
		glGetAttachedShaders = (PFNGLGETATTACHEDSHADERSPROC)wglGetProcAddress ("glGetAttachedShaders");
		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress ("glGetUniformLocation");
		glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress ("glUniform1f");
		glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress ("glUniform2f");
		glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress ("glUniform3f");
		glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress ("glUniform4f");
		glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress ("glUniform1i");		
	//}
	glewInit();

	return 0;
}

void Game::initTestCube()
{
	/*
	// Add Ground
	//btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),0);
	btCollisionShape * groundShape = new btBoxShape(btVector3(10.0f, 0.1f, 10.0f));
	collisionShapes.push_back(groundShape);

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,-0.1,0));

	btScalar mass(0.);
	btScalar friction(0.5);
	btVector3 localInertia(0,0,0);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,groundShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	dynamicsWorld->addRigidBody(body, COL_DEFAULT, COL_DEFAULT);
	body->setFriction(friction);
	new GameObject("Models\\floor.obj", "Textures\\floor.tga", "Shaders\\default.glsl", glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(0,1,0));


	//btCollisionShape* groundShape2 = new btStaticPlaneShape(btVector3(0,-1,0),0);
	btCollisionShape * groundShape2 = new btBoxShape(btVector3(10.0f, 0.1, 10.0f));
	collisionShapes.push_back(groundShape2);

	btTransform groundTransform2;
	groundTransform2.setIdentity();
	groundTransform2.setOrigin(btVector3(0,20.1,0));

	mass = 0.;
	btVector3 localInertia2(0,0,0);

	btDefaultMotionState* myMotionState2 = new btDefaultMotionState(groundTransform2);
	btRigidBody::btRigidBodyConstructionInfo rbInfo2(mass,myMotionState2,groundShape2,localInertia2);
	btRigidBody* body2 = new btRigidBody(rbInfo2);

	dynamicsWorld->addRigidBody(body2, COL_DEFAULT, COL_DEFAULT);
	body->setFriction(friction);
	new GameObject("Models\\floor.obj", "Textures\\floor.tga", "Shaders\\default.glsl", glm::vec3(0,20,0), glm::vec3(0,0,1), glm::vec3(0,-1,0));

	//btCollisionShape* groundShape2 = new btStaticPlaneShape(btVector3(0,-1,0),0);
	btCollisionShape * groundShape3 = new btBoxShape(btVector3(10.0f, 10.0, 0.1f));
	collisionShapes.push_back(groundShape3);

	btTransform groundTransform3;
	groundTransform3.setIdentity();
	groundTransform3.setOrigin(btVector3(0,10,-10.1f));

	mass = 0.;
	btVector3 localInertia3(0,0,0);

	btDefaultMotionState* myMotionState3 = new btDefaultMotionState(groundTransform3);
	btRigidBody::btRigidBodyConstructionInfo rbInfo3(mass,myMotionState3,groundShape3,localInertia3);
	btRigidBody* body3 = new btRigidBody(rbInfo3);

	dynamicsWorld->addRigidBody(body3, COL_DEFAULT, COL_DEFAULT);
	body->setFriction(friction);
	new GameObject("Models\\floor.obj", "Textures\\floor.tga", "Shaders\\default.glsl", glm::vec3(0,10,-10.0), glm::vec3(1,0,0), glm::vec3(0,0,1));

	//btCollisionShape* groundShape2 = new btStaticPlaneShape(btVector3(0,-1,0),0);
	btCollisionShape * groundShape4 = new btBoxShape(btVector3(10.0f, 10.0, 0.1f));
	collisionShapes.push_back(groundShape4);

	btTransform groundTransform4;
	groundTransform4.setIdentity();
	groundTransform4.setOrigin(btVector3(0,10,10.1f));

	mass = 0.;
	btVector3 localInertia4(0,0,0);

	btDefaultMotionState* myMotionState4 = new btDefaultMotionState(groundTransform4);
	btRigidBody::btRigidBodyConstructionInfo rbInfo4(mass,myMotionState4,groundShape4,localInertia4);
	btRigidBody* body4 = new btRigidBody(rbInfo4);

	dynamicsWorld->addRigidBody(body4, COL_DEFAULT, COL_DEFAULT);
	body->setFriction(friction);
	new GameObject("Models\\floor.obj", "Textures\\floor.tga", "Shaders\\default.glsl", glm::vec3(0,10,10.0), glm::vec3(-1,0,0), glm::vec3(0,0,-1));

	//btCollisionShape* groundShape2 = new btStaticPlaneShape(btVector3(0,-1,0),0);
	btCollisionShape * groundShape5 = new btBoxShape(btVector3(0.1f, 10.0, 10.0f));
	collisionShapes.push_back(groundShape5);

	btTransform groundTransform5;
	groundTransform5.setIdentity();
	groundTransform5.setOrigin(btVector3(10.1f,10,0));

	mass = 0.;
	btVector3 localInertia5(0,0,0);

	btDefaultMotionState* myMotionState5 = new btDefaultMotionState(groundTransform5);
	btRigidBody::btRigidBodyConstructionInfo rbInfo5(mass,myMotionState5,groundShape5,localInertia5);
	btRigidBody* body5 = new btRigidBody(rbInfo5);

	dynamicsWorld->addRigidBody(body5, COL_DEFAULT, COL_DEFAULT);
	body->setFriction(friction);
	new GameObject("Models\\floor.obj", "Textures\\floor.tga", "Shaders\\default.glsl", glm::vec3(10,10,0), glm::vec3(0,0,1), glm::vec3(-1,0,0));

	//btCollisionShape* groundShape2 = new btStaticPlaneShape(btVector3(0,-1,0),0);
	btCollisionShape * groundShape6 = new btBoxShape(btVector3(0.1f, 10.0, 10.0f));
	collisionShapes.push_back(groundShape6);

	btTransform groundTransform6;
	groundTransform6.setIdentity();
	groundTransform6.setOrigin(btVector3(-10.1f,10,0));

	mass = 0.;
	btVector3 localInertia6(0,0,0);

	btDefaultMotionState* myMotionState6 = new btDefaultMotionState(groundTransform6);
	btRigidBody::btRigidBodyConstructionInfo rbInfo6(mass,myMotionState6,groundShape6,localInertia6);
	btRigidBody* body6 = new btRigidBody(rbInfo6);

	dynamicsWorld->addRigidBody(body6, COL_DEFAULT, COL_DEFAULT);
	body->setFriction(friction);
	new GameObject("Models\\floor.obj", "Textures\\floor.tga", "Shaders\\default.glsl", glm::vec3(-10,10,0), glm::vec3(0,0,1), glm::vec3(1,0,0));
	*/
	//new PropObject("Models\\ballworld.obj", "Textures\\floor.tga", "Shaders\\default.glsl",
	//	glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(1,0,0), "model");
}

bool Game::myContactProcessedCallback(btManifoldPoint& cp, void * body0, void * body1)
{
	if (((btRigidBody*)body0)->getOwner() != NULL && ((btRigidBody*)body1)->getOwner() != NULL) {
		((RigidObject*)((btRigidBody*)body0)->getOwner())->collisionCallback((RigidObject*)((btRigidBody*)body1)->getOwner());
		((RigidObject*)((btRigidBody*)body1)->getOwner())->collisionCallback((RigidObject*)((btRigidBody*)body0)->getOwner());
		return true;
	}
	return false;
}

int Game::initBullet()
{
	/// collision configuration contains default setup for memory , collision setup . Advanced users can create their own configuration .
	collisionConfiguration = new btDefaultCollisionConfiguration ();

	///use the default collision dispatcher . For parallel processing you can use a diffent dispatcher ( see Extras / BulletMultiThreaded )
	dispatcher = new btCollisionDispatcher ( collisionConfiguration );

	/// btDbvtBroadphase is a good general purpose broadphase . You can also try out btAxis3Sweep .
	broadphase = new btDbvtBroadphase ();
	//broadphase = new btAxisSweep3 ();

	///the default constraint solver . For parallel processing you can use a different solver ( see Extras / BulletMultiThreaded )
	solver = new btSequentialImpulseConstraintSolver;
	
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration );
	//ghostPairCallback = new btGhostPairCallback();
	//dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(ghostPairCallback);
	dynamicsWorld->setGravity(btVector3(0 , -98.1*2.0f ,0));
	gContactProcessedCallback = (ContactProcessedCallback)myContactProcessedCallback;

	return 0;
}

int Game::cleanBullet()
{	
	dynamicsWorld->clearForces();

	for (int i = dynamicsWorld->getNumConstraints() -1; i >= 0; i--) {
		btTypedConstraint * cons = dynamicsWorld->getConstraint(i);
		dynamicsWorld->removeConstraint(cons);
	}
	
	for (int i = dynamicsWorld->getNumCollisionObjects() -1; i >= 0; i--) {
		btCollisionObject * obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody * body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		dynamicsWorld -> removeCollisionObject ( obj );
		delete obj;
	}
	
	for (int j = 0; j < collisionShapes.size(); j++) {
		btCollisionShape * shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	return 0;
}

int Game::killBullet()
{
	cleanBullet();

	//delete ghostPairCallback;
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;

	collisionShapes.clear();

	return 0;
}

void Game::addPlane(glm::vec2 pos, glm::vec2 normal)
{
	btCollisionShape * colShape = new btStaticPlaneShape(btVector3(normal.x, normal.y, 0), 1);

	btTransform startTransform;
	startTransform.setIdentity();

	startTransform.setOrigin(btVector3(pos.x, pos.y, 0.0f));
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody * body = new btRigidBody(0.0f, myMotionState, colShape, btVector3(0,0,0));

	dynamicsWorld->addRigidBody(body, COL_DEFAULT, COL_DEFAULT);
}

int Game::loadLevel()
{
	if (level > 3) level = 0;
	if (level == 0) {
		new GameObject("Textures\\cavebg.tga", "Shaders\\global.glsl", glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 1000.0f));
		new GameObject("Textures\\tutorial1.tga", "Shaders\\default.glsl", glm::vec2(150.0f, 50.0f), 0.0f, glm::vec2(30.0f, 15.0f));
		new GameObject("Textures\\tutorial2.tga", "Shaders\\default.glsl", glm::vec2(0, 35), 0.0f, glm::vec2(30.0f, 15.0f));

		webPos = glm::vec2(88.3f, 52.94f);
		new GameObject("Textures\\web.tga", "Shaders\\default.glsl", webPos, 0.0f, glm::vec2(50.0f, 50.0f));
		webbedMen->push_back(new GameObject("Textures\\webbedman.tga", "Shaders\\default.glsl", webPos - glm::vec2(0, 18), 0.0f, glm::vec2(10.0f, 10.0f)));
		webbedMen->push_back(new GameObject("Textures\\webbedman.tga", "Shaders\\default.glsl", glm::vec2(0, 15), 0.0f, glm::vec2(10.0f, 10.0f)));

		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-47.85f, 6.3f), -15.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-74.94f, 33.38f), 15.0f, glm::vec2(20.0f, 50.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-81.24f, 81.24f), 0.0f, glm::vec2(20.0f, 50.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-60.8f, 107.88f), 15.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(12.06f, 114.18f), 0.0f, glm::vec2(100.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(62.41f, 98.57f), 15.0f, glm::vec2(20.0f, 50.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(47.85f, 6.3f), 15.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(83.3f, 19.94f), 0.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(82.41f, 88.54f), 0.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(117.3f, 54.69f), 0.0f, glm::vec2(20.0f, 90.0f), 0.0f);
			
		player = new PlayerObject("Textures\\spider.tga", "Shaders\\sprite.glsl", webPos, glm::vec2(20.0f, 20.0f), 1.0f);
		Soldier::setStatics(player);
		Man::setStatics(player);

		numRequired = 2;
		numCaptured = 1;
	}

	else if (level == 1) {
		new GameObject("Textures\\cavebg.tga", "Shaders\\global.glsl", glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 1000.0f));
		new GameObject("Textures\\tutorial3.tga", "Shaders\\default.glsl", glm::vec2(50, 100), 0.0f, glm::vec2(30.0f, 15.0f));

		webPos = glm::vec2(104.17, 100.73f);
		new GameObject("Textures\\web.tga", "Shaders\\default.glsl", webPos, 0.0f, glm::vec2(50.0f, 50.0f));
		webbedMen->push_back(new GameObject("Textures\\webbedman.tga", "Shaders\\default.glsl", webPos - glm::vec2(0, 20), 0.0f, glm::vec2(10.0f, 10.0f)));

		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-46.56f, 6.13f), -15.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-93.12f, 12.26f), 0.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-114.93f, 29.0f), 15.0f, glm::vec2(20.0f, 50.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-121.06f, 75.56f), 0.0f, glm::vec2(20.0f, 50.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-61.91f, 89.32f), 0.0f, glm::vec2(138.31f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(4.06f, 106.05f), -15.0f, glm::vec2(20.0f, 50), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(27.61f, 129.6f), 15.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(89.17f, 135.73f), 0.0f, glm::vec2(80.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(139.17f, 100.73f), 0.0f, glm::vec2(20.0f, 90.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(100.63f, 65.73f), 0.0f, glm::vec2(57.07f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(72.7f, 39.34f), -15.0f, glm::vec2(20.0f, 70.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(46.56f, 6.13f), 15.0f, glm::vec2(50.0f, 20.0f), 0.0f);

		new Man("Textures\\man.tga", "Shaders\\sprite.glsl", glm::vec2(0, 20), glm::vec2(20, 20), 1.0f);
		new Man("Textures\\man.tga", "Shaders\\sprite.glsl", glm::vec2(-80, 32.26), glm::vec2(20, 20), 1.0f);

		player = new PlayerObject("Textures\\spider.tga", "Shaders\\sprite.glsl", webPos, glm::vec2(20.0f, 20.0f), 1.0f);
		Soldier::setStatics(player);
		Man::setStatics(player);

		numRequired = 3;
		numCaptured = 1;
	}

	else if (level == 2) {
		new GameObject("Textures\\cavebg.tga", "Shaders\\global.glsl", glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(1000.0f, 1000.0f));
		new GameObject("Textures\\tutorial4.tga", "Shaders\\default.glsl", glm::vec2(63.62f, 58.47f), 0.0f, glm::vec2(30.0f, 15.0f));
		new GameObject("Textures\\tutorial5.tga", "Shaders\\default.glsl", glm::vec2(-23.28f, 75.97f), 0.0f, glm::vec2(30.0f, 15.0f));

		webPos = glm::vec2(118.62, 68.47f);
		new GameObject("Textures\\web.tga", "Shaders\\default.glsl", webPos, 0.0f, glm::vec2(50.0f, 50.0f));
		webbedMen->push_back(new GameObject("Textures\\webbedman.tga", "Shaders\\default.glsl", webPos - glm::vec2(0, 20), 0.0f, glm::vec2(10.0f, 10.0f)));
		webbedMen->push_back(new GameObject("Textures\\webbedman.tga", "Shaders\\default.glsl", glm::vec2(-142.24f, 15.0f), 0.0f, glm::vec2(10.0f, 10.0f)));

		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(0.0f, 0.0f), 0.0f, glm::vec2(200.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-96.81f, 16.74f), 15.0f, glm::vec2(20.0f, 50.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-103.62f, 33.47f), 0.0f, glm::vec2(20.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-110.43f, 16.74f), -15.0f, glm::vec2(20.0f, 50.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-142.24f, 0.0f), 0.0f, glm::vec2(70.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-174.06f, 16.74f), 15.0f, glm::vec2(20.0f, 50.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-180.19f, 65.88f), 0.0f, glm::vec2(20.0f, 55.18f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-115.19f, 103.47f), 0.0f, glm::vec2(150.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-50.19f, 163.47f), 0.0f, glm::vec2(20.0f, 100.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-23.28f, 223.47f), 0.0f, glm::vec2(76.81f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(3.62f, 163.47f), 0.0f, glm::vec2(20.0f, 100.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(68.62f, 103.47f), 0.0f, glm::vec2(150.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(153.62f, 68.47f), 0.0f, glm::vec2(20.0f, 90.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(118.62f, 33.47f), 0.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(96.81f, 16.74f), -15.0f, glm::vec2(20.0f, 50.0f), 0.0f);

		new Soldier("Textures\\soldier.tga", "Shaders\\sprite.glsl", glm::vec2(-50, 20), glm::vec2(20, 20), 1.0f);
		new Soldier("Textures\\soldier.tga", "Shaders\\sprite.glsl", glm::vec2(0, 20), glm::vec2(20, 20), 1.0f);
		new Soldier("Textures\\soldier.tga", "Shaders\\sprite.glsl", glm::vec2(50, 20), glm::vec2(20, 20), 1.0f);

		player = new PlayerObject("Textures\\spider.tga", "Shaders\\sprite.glsl", webPos, glm::vec2(20.0f, 20.0f), 1.0f);
		Soldier::setStatics(player);
		Man::setStatics(player);

		numRequired = 2;
		numCaptured = 1;
	}

	else if (level == 3) {
		addPlane(glm::vec2(-45.0f, 0.0f), glm::vec2(1,0));
		addPlane(glm::vec2(875.0f, 0.0f), glm::vec2(-1, 0));

		//Burrow
		new GameObject("Textures\\cavebg.tga", "Shaders\\global.glsl", glm::vec2(606.05f, -51.21f), 0.0f, glm::vec2(1791.67f, 192.42f));
		webPos = glm::vec2(0.0f, 0.0f);
		new GameObject("Textures\\web.tga", "Shaders\\default.glsl", webPos, 0.0f, glm::vec2(50.0f, 50.0f));
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(0.0f, -35.0f), 0.0f, glm::vec2(50.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(-35.0f, 0.0f), 0.0f, glm::vec2(20.0f, 90.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(25.0f, 35.0f), 0.0f, glm::vec2(100.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(111.59f, 12.94f), 0.0f, glm::vec2(20.0f, 64.12f), 0.0f);
		new BoxObject("Textures\\rock.tga", "Shaders\\global.glsl", glm::vec2(70.71f, -22.4f), 15.0f, glm::vec2(100.0f, 20.0f), 0.0f);

		//House 1
		new GameObject("Textures\\plaster.tga", "Shaders\\global.glsl", glm::vec2(390.0f, 135.0f), 0.0f, glm::vec2(200.0f, 140.0f));
		new GameObject("Textures\\plaster.tga", "Shaders\\global.glsl", glm::vec2(390.0f, 216.27f), 0.0f, glm::vec2(133.85f, 22.53f));
		new GameObject("Textures\\plaster.tga", "Shaders\\global.glsl", glm::vec2(390.0f, 237.74f), 0.0f, glm::vec2(55.79f, 20.41f));
		new BoxObject("Textures\\wood.tga", "Shaders\\global.glsl", glm::vec2(360.0f, 125.0f), 0.0f, glm::vec2(100.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\wood.tga", "Shaders\\global.glsl", glm::vec2(430.74f, 78.72f), 15.0f, glm::vec2(118.52f, 20.0f), 0.0f);
		new BoxObject("Textures\\brick.tga", "Shaders\\global.glsl", glm::vec2(300.0f, 102.5f), 0.0f, glm::vec2(20.0, 75.0f), 0.0f);
		new BoxObject("Textures\\brick.tga", "Shaders\\global.glsl", glm::vec2(300.0f, 192.5f), 0.0f, glm::vec2(20.0, 25.0f), 0.0f);
		new BoxObject("Textures\\brick.tga", "Shaders\\global.glsl", glm::vec2(480.0f, 102.5f), 0.0f, glm::vec2(20.0, 75.0f), 0.0f);
		new BoxObject("Textures\\brick.tga", "Shaders\\global.glsl", glm::vec2(480.0f, 192.5f), 0.0f, glm::vec2(20.0, 25.0f), 0.0f);
		new BoxObject("Textures\\shingles.tga", "Shaders\\global.glsl", glm::vec2(333.41f, 222.95f), 30.0f, glm::vec2(111.8f, 20.0f), 0.0f);
		new BoxObject("Textures\\shingles.tga", "Shaders\\global.glsl", glm::vec2(389.24f, 249.56f), 0.0f, glm::vec2(24.85f, 20.0f), 0.0f);
		new BoxObject("Textures\\shingles.tga", "Shaders\\global.glsl", glm::vec2(445.08, 222.95f), -30.0f, glm::vec2(111.8f, 20.0f), 0.0f);
		new BoxObject("Textures\\shingles.tga", "Shaders\\global.glsl", glm::vec2(515.0f, 125.0f), 0.0f, glm::vec2(50.0f, 20.0f), 0.0f);

		//House 2
		new GameObject("Textures\\plaster.tga", "Shaders\\global.glsl", glm::vec2(715.0f, 135.0f), 0.0f, glm::vec2(200.0f, 140.0f));
		new GameObject("Textures\\plaster.tga", "Shaders\\global.glsl", glm::vec2(715.0f, 216.27f), 0.0f, glm::vec2(133.85f, 22.53f));
		new GameObject("Textures\\plaster.tga", "Shaders\\global.glsl", glm::vec2(715.0f, 237.74f), 0.0f, glm::vec2(55.79f, 20.41f));
		new GameObject("Textures\\plaster.tga", "Shaders\\global.glsl", glm::vec2(659.17f, 222.95f), 30.0f, glm::vec2(111.8f, 20.0f));
		new BoxObject("Textures\\wood.tga", "Shaders\\global.glsl", glm::vec2(715.0f, 125.0f), 0.0f, glm::vec2(160.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\brick.tga", "Shaders\\global.glsl", glm::vec2(625.0f, 135.0f), 0.0f, glm::vec2(20.0, 140.0f), 0.0f);
		new BoxObject("Textures\\brick.tga", "Shaders\\global.glsl", glm::vec2(805.0f, 135.0f), 0.0f, glm::vec2(20.0, 140.0f), 0.0f);

		new BoxObject("Textures\\shingles.tga", "Shaders\\global.glsl", glm::vec2(623.75, 202.5f), 30.0f, glm::vec2(30.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\shingles.tga", "Shaders\\global.glsl", glm::vec2(694.59f, 243.4f), 30.0f, glm::vec2(30.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\shingles.tga", "Shaders\\global.glsl", glm::vec2(715.0f, 249.56f), 0.0f, glm::vec2(24.85f, 20.0f), 0.0f);
		new BoxObject("Textures\\shingles.tga", "Shaders\\global.glsl", glm::vec2(770.83f, 222.95f), -30.0f, glm::vec2(111.8f, 20.0f), 0.0f);
		new BoxObject("Textures\\shingles.tga", "Shaders\\global.glsl", glm::vec2(590.0f, 125.0f), 0.0f, glm::vec2(50.0f, 20.0f), 0.0f);

		//Grass
		new BoxObject("Textures\\grass.tga", "Shaders\\global.glsl", glm::vec2(-75.0f, 55.0f), 0.0f, glm::vec2(300.0f, 20.0f), 0.0f);
		new BoxObject("Textures\\grass.tga", "Shaders\\global.glsl", glm::vec2(601.59f, 55.0f), 0.0f, glm::vec2(1000.0f, 20.0f), 0.0f);

		new Man("Textures\\man.tga", "Shaders\\sprite.glsl", glm::vec2(342.0f, 75.0f), glm::vec2(20, 20), 1.0f);
		new Man("Textures\\man.tga", "Shaders\\sprite.glsl", glm::vec2(360.0f, 145.0f), glm::vec2(20, 20), 1.0f);
		new Man("Textures\\man.tga", "Shaders\\sprite.glsl", glm::vec2(680.0f, 145.0f), glm::vec2(20, 20), 1.0f);
		new Man("Textures\\man.tga", "Shaders\\sprite.glsl", glm::vec2(750.0f, 145.0f), glm::vec2(20, 20), 1.0f);

		new Soldier("Textures\\soldier.tga", "Shaders\\sprite.glsl", glm::vec2(510.0f, 75.0f), glm::vec2(20, 20), 1.0f);
		new Soldier("Textures\\soldier.tga", "Shaders\\sprite.glsl", glm::vec2(595.0f, 75.0f), glm::vec2(20, 20), 1.0f);

		player = new PlayerObject("Textures\\spider.tga", "Shaders\\sprite.glsl", webPos, glm::vec2(20.0f, 20.0f), 1.0f);
		Soldier::setStatics(player);
		Man::setStatics(player);

		numRequired = 4;
		numCaptured = 0;
	}

	//char1 = new AI("Models\\player.obj", "Textures\\error.tga", "Shaders\\default.glsl", 
	//	glm::vec3(0,10,0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), "sphere2");
	
	return 0;
}

int Game::start()
{
	double elapsedTime = 0.0;
	double timePerFrame = 1000.0 / Settings::getMaxFPS();
	long lastTime = 0;
	
	double targetSleep = 0.0;
	double sleepRemainder = 0.0;
	double startSleep = 0.0;
	double sleepTime = 0.0;
	
	assetManager->forceLoadModel("Models\\quad.obj");
	quad = assetManager->getModel("Models\\quad.obj");
	assetManager->forceLoadTexture("Textures\\tutorial1.tga");
	assetManager->forceLoadTexture("Textures\\tutorial2.tga");
	assetManager->forceLoadTexture("Textures\\tutorial3.tga");
	assetManager->forceLoadTexture("Textures\\tutorial4.tga");
	assetManager->forceLoadTexture("Textures\\tutorial5.tga");
	assetManager->forceLoadTexture("Textures\\rock.tga");
	assetManager->forceLoadTexture("Textures\\grass.tga");
	assetManager->forceLoadTexture("Textures\\wood.tga");
	assetManager->forceLoadTexture("Textures\\brick.tga");
	assetManager->forceLoadTexture("Textures\\plaster.tga");
	assetManager->forceLoadTexture("Textures\\shingles.tga");
	assetManager->forceLoadTexture("Textures\\cavebg.tga");
	assetManager->forceLoadTexture("Textures\\webbedman.tga");
	assetManager->forceLoadTexture("Textures\\web.tga");
	assetManager->forceLoadTexture("Textures\\man.tga");
	assetManager->forceLoadTexture("Textures\\soldier.tga");
	assetManager->forceLoadTexture("Textures\\title.tga");
	startScreen = assetManager->getTexture("Textures\\title.tga");
	assetManager->forceLoadSound("Sounds\\grunt.wav");
	assetManager->forceLoadSound("Sounds\\leap.wav");
	assetManager->forceLoadSound("Sounds\\scream.wav");
	assetManager->forceLoadSound("Sounds\\wrap.wav");
	assetManager->forceLoadShader("Shaders\\text.glsl");
	assetManager->forceLoadShader("Shaders\\quad.glsl");
	assetManager->forceLoadShader("Shaders\\sprite.glsl");
	defaultShader = assetManager->getShader("Shaders\\quad.glsl");
	defaultShader->activate();
	textShader = assetManager->getShader("Shaders\\text.glsl");
	quadShader = assetManager->getShader("Shaders\\quad.glsl");

	assetManager->forceLoadModel("Models\\error.obj");

	assetManager->forceLoadTexture("Textures\\ctest.tga");
	assetManager->forceLoadTexture("Textures\\error.tga");
	assetManager->forceLoadTexture("Textures\\pigaligator.tga");
	assetManager->forceLoadTexture("Textures\\spider.tga");
	
	errorTexture = assetManager->getTexture("Textures\\error.tga");
	
	while(running)
	{
		time++;
		lastTime = SDL_GetTicks();

		inputManager.clearTempValues();
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)	return 1;
			else if (e.type == SDL_MOUSEBUTTONDOWN) inputManager.mouseKeyDown(e.button.button);
			else if (e.type == SDL_MOUSEBUTTONUP)	inputManager.mouseKeyUp(e.button.button);
			else if (e.type == SDL_KEYDOWN)			inputManager.keyDown(e.key.keysym.sym);
			else if (e.type == SDL_KEYUP)			inputManager.keyUp(e.key.keysym.sym);
			else if (e.type == SDL_MOUSEMOTION)		inputManager.updateMouse(e.motion.xrel, e.motion.yrel, e.motion.x, e.motion.y); 
			else if (e.type == SDL_MOUSEWHEEL)		{ inputManager.updateScroll(e.wheel.y); }
			else {}
		}
		
		if (update(elapsedTime+sleepTime) == -1) break;
		renderFrame(SDL_GetTicks());
		
		double averageElapsedTime = calcFps(elapsedTime+sleepTime);
		frameRate = 1000.0/averageElapsedTime;

		//Framerate Limit Calculations
		elapsedTime = SDL_GetTicks() - lastTime;
		targetSleep = timePerFrame - elapsedTime + sleepRemainder;
		if (targetSleep > 0) sleepRemainder = targetSleep - (Uint32)targetSleep;

		startSleep = SDL_GetTicks();
		while ((Uint32)(startSleep+targetSleep) > SDL_GetTicks());
		sleepTime = SDL_GetTicks() - startSleep;
	}
    return 0;
}

int Game::startAssetLoader(void * data)
{
	assetManager->startAssetLoader();
	return 0;
}

int Game::cleanup()
{
	//ClientThread::cleanup();
	//delete client;
	Settings::cleanup();
	cleanupObjects();
	defaultShader = 0;
	quad = 0;
	startScreen = 0;

	saveConfig();
	
	killBullet();
	killSDL();
	killFont();

	//#if DEBUG
	//	_CrtDumpMemoryLeaks();
	//#endif

	PostQuitMessage(0);
	return 0;
}

int Game::killSDL() 
{
	wglDeleteContext(loadingThreadContext);
	SDL_GL_DeleteContext(displayContext);
    SDL_DestroyWindow(displayWindow);
	SDL_Quit();

	return 0;
}

int Game::resizeWindow(int width, int height) {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	if (width == desktop.right && height == desktop.bottom)
		width++;
	
	SDL_SetWindowSize(displayWindow, width, height); 

	//Setup a new viewport.
	glViewport (0, 0, width, height);
	Settings::setWindowWidth(width); 
	Settings::setWindowHeight(height);

	//Setup a new perspective matrix.
	GLdouble verticalFieldOfViewInDegrees = 40;
	GLdouble aspectRatio = height == 0 ? 1.0 : (GLdouble) width / (GLdouble) height;
	GLdouble nearDistance = 1.0;
	GLdouble farDistance = 2000.0;

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (verticalFieldOfViewInDegrees, aspectRatio, nearDistance, farDistance);

	//Get back to default mode.
	glMatrixMode (GL_MODELVIEW);

	return 0;
}

double Game::calcFps(double newtick)
{
	ticksum-=ticklist[tickindex];  /* subtract value falling off */
    ticksum+=newtick;              /* add new value */
    ticklist[tickindex]=newtick;   /* save new value so it can be subtracted later */
    if(++tickindex==MAXFRAMESAMPLES)    /* inc buffer index */
        tickindex=0;

    /* return average */
    return((ticksum)/MAXFRAMESAMPLES);
}

void Game::startOrtho() {
	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
		glLoadIdentity ();
		glOrtho (0.0, (GLfloat) Settings::getWindowWidth(), 0.0, (GLfloat) Settings::getWindowHeight(), -100.0f, 100.0f);
		glMatrixMode (GL_MODELVIEW);
		glPushMatrix ();
			glLoadIdentity ();
}

void Game::endOrtho() {
			glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
}

/*
void Game::changeUsername(string x) //notifies the server
{
	Settings::setUsername(x);
	client->aquireMutex();
		client->setName(x);
		//push changeName() to sendBuffer
	client->releaseMutex();
}

void Game::setUsername(string x) //does not notify the server, only use on startup
{
	Settings::setUsername(x);
	client->aquireMutex();
		client->setName(x);
	client->releaseMutex();
}*/

int Game::calcNumCaptured()
{
	int n = 0;
	for (list<GameObject*>::iterator it = webbedMen->begin(); it != webbedMen->end(); it++) {
		if (magSqr(webPos - (*it)->getPosition()) < 1000) n++;
	}
	return n;
}

int Game::update(long elapsedTime)
{
	if (isKeyPressed(IM_ESCAPE)) {
		/*if (ClientThread::isRunning()) {
			ClientThread::disconnectFromServer();
			WaitForSingleObject(clientThreadHandle, INFINITE);
		}*/
		return -1;
	}

	if (isKeyPressed(IM_NUM7)) toggleWireFrame();

	if (state == 0)	{
		if (isKeyPressed(IM_SPACE)) {
			state = 1;
			level = 0;
			loadLevel();
		}
		return 0;
	}
	else if (state == 2) {
		if (isKeyPressed(IM_SPACE)) {
			webbedMen->clear();
			while (gameObjects->size() > 0) {
				delete gameObjects->front();
				gameObjects->pop_front();
			}
			cleanBullet();

			state = 1;
			level++;
			loadLevel();
		}
		return 0;
	}
	else if (state == 3) {
		if (isKeyPressed(IM_SPACE)) {
			webbedMen->clear();
			while (gameObjects->size() > 0) {
				delete gameObjects->front();
				gameObjects->pop_front();
			}
			cleanBullet();

			state = 1;
			loadLevel();
		}
		return 0;
	}
		
	if (isKeyPressed(IM_O)) 
	{
		webbedMen->clear();
		while (gameObjects->size() > 0) {
			delete gameObjects->front();
			gameObjects->pop_front();
		}
		cleanBullet();
		loadLevel();
	}

	/*if (isKeyPressed(IM_P) && !ClientThread::isRunning()) 
		clientThreadHandle = ClientThread::connectToServer();

	if (isKeyPressed(IM_L))
		ClientThread::disconnectFromServer();

	if (isKeyPressed(IM_M)) {
		client->aquireMutex();
			client->pushSendData("Testing Testing 123\n", 20);
		client->releaseMutex();
	}*/
	
	camera.setPosition(btToGLM3(&player->getPosition()) + glm::vec3(0, 0, 0));
	//camera.setPosition(glm::vec3(0, 0, 0));
	camera.setLookAtVector(glm::vec3(0, 0, -1));
	
	dynamicsWorld->stepSimulation(1.0f / 60.0f, 100, 1.0f / 60.0f); // (float)elapsedTime*0.001f); //1.0f/(float)Settings::getMaxFPS());

	for (list<GameObject*>::iterator it = gameObjects->begin(); it != gameObjects->end(); it++)
		(*it)->update(elapsedTime);
	
	deadObjects->clear();
	for (list<GameObject*>::iterator it = gameObjects->begin(); it != gameObjects->end(); it++) {
		if (!(*it)->isAlive())
			deadObjects->push_back(*it);
	}

	while (deadObjects->size() > 0) {
		bool playerIsDead = deadObjects->front() == player;
		gameObjects->remove(deadObjects->front());
		delete deadObjects->front();
		deadObjects->pop_front();
		if (playerIsDead) player = 0;
	}
	
	soundManager->update();

	numCaptured = calcNumCaptured();
	if (numCaptured == numRequired) {
		state = 2;
	}

	if (player == 0)
		state = 3;

	return 0;
}

int Game::renderFrame(long totalElapsedTime) {
	//FrameBuffer::setActiveFrameBuffer(0);
	glClearColor (0.4, 0.6, 1.0, 1.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);

	if (state == 0)
	{
		draw2DRect(0.0f, 0.0f, 1.0f, 1.0f, startScreen);
	}
	else if (state == 1)
	{
		camera.calculateView(&view); 
		
		float aspectRatio = Settings::getAspectRatio();
		if (aspectRatio > 1)
			camera.calculateOrthographicProjection(&projection, 100.0f, 100.0f*aspectRatio);
		else
			camera.calculateOrthographicProjection(&projection, 100.0f/aspectRatio, 100.0f);

		int i = 0;
		for (list<GameObject*>::iterator it = gameObjects->begin(); it != gameObjects->end(); it++)
			(*it)->render(i++, totalElapsedTime);
		render2D(time);
	}
	else if (state == 2) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		textShader->activate();
		textShader->setUniformf3("textColour", 0.9f, 0.9f, 0.9f);

		ostringstream s;
		if (level == 3) {
			s << "You've satisfied your hunger for now... Press Space to Play again.";
			drawText(Settings::getWindowWidth() / 4, Settings::getWindowHeight() / 2, s.str().c_str());
		}
		else {
			s << "Level " << level << " Completed. Press Space to Continue.";
			drawText(Settings::getWindowWidth() / 4, Settings::getWindowHeight() / 2, s.str().c_str());
		}
	}
	else if (state == 3) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		textShader->activate();
		textShader->setUniformf3("textColour", 0.9f, 0.9f, 0.9f);

		ostringstream s;
		s << "You Died. Press Space to Continue.";
		drawText(Settings::getWindowWidth() / 4, Settings::getWindowHeight() / 2, s.str().c_str());
	}
	
	SDL_GL_SwapWindow(displayWindow);
	return 0;
}

void Game::draw3DRect(float x, float y, float z)
{
	glm::mat4 world;
	world = glm::translate(glm::vec3(x,y,z));
	glm::mat4 matrix = projection * view * world;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
	activeShader->setUniformMatrixf4("worldViewProj", matrix);
	activeShader->setUniformMatrixf4("normalMatrix", normalMatrix);

	quad->render();
}

void Game::draw3DRect(glm::vec3 direction, glm::vec3 position, glm::vec3 up, glm::vec3 size)
{
	glm::mat4 world, scale;
	world = glm::inverse((glm::lookAt(position, position+direction, up)));
	scale = glm::scale(size);
	glm::mat4 matrix = projection * view * world * scale;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
	activeShader->setUniformMatrixf4("worldViewProj", matrix);
	activeShader->setUniformMatrixf4("normalMatrix", normalMatrix);

	quad->render();
}

void Game::draw3DRect(float x, float y, float z, float rotation)
{
	glm::mat4 world;
	world = glm::translate(glm::vec3(x,y,z));
	world = glm::rotate(world, rotation, glm::vec3(0,1,0));
	glm::mat4 matrix = projection * view * world;
	glm::mat4 normalMatrix = glm::transpose(glm::inverse(world));
	activeShader->setUniformMatrixf4("worldViewProj", matrix);
	activeShader->setUniformMatrixf4("normalMatrix", normalMatrix);

	quad->render();
}

void Game::draw2DRect(float x, float y, float width, float height, Texture * t)
{
	Shader * prevShader = activeShader;
	quadShader->activate();
		
	glActiveTexture(GL_TEXTURE0);
	if (t != NULL) t->bindTexture();
	quadShader->setUniformTexture("textureSampler", 0);
	quadShader->setUniformf1("width", width*2.0f);
	quadShader->setUniformf1("height", height*2.0f);
	quadShader->setUniformf2("position", x*2.0f, y*2.0f);

	quad->render();

	if (prevShader != NULL) prevShader->activate();
}

void Game::draw2DRect(float x, float y, int width, int height, Texture * t) { 
	draw2DRect(x, y, (float)width/(float)Settings::getWindowWidth(), (float)height/(float)Settings::getWindowHeight(), t); 
}
void Game::draw2DRect(int x, int y, float width, float height, Texture * t) { 
	draw2DRect((float)x/(float)Settings::getWindowWidth(), (float)y/(float)Settings::getWindowHeight(), width, height, t); 
}
void Game::draw2DRect(int x, int y, int width, int height, Texture * t) { 
	draw2DRect((float)x/(float)Settings::getWindowWidth(), (float)y/(float)Settings::getWindowHeight(), (float)width/(float)Settings::getWindowWidth(), (float)height/(float)Settings::getWindowHeight(), t); 
}

int Game::render2D(long time)
{
	startOrtho();
		glDepthFunc(GL_ALWAYS);		
			textShader->activate();
			textShader->setUniformf3("textColour", 0.9f, 0.9f, 0.9f); 
	
			ostringstream s;
			s.str(string());
			s << "Level " << level << ": " << numCaptured << " / " << numRequired;
			drawText(Settings::getWindowWidth() - 200, Settings::getWindowHeight() - 70, s.str().c_str());

			/*
			const unsigned char * ver = glGetString(GL_VERSION);
			string v = "";
			int i = 0;
			while (ver[i] != ' ') { v += ver[i]; i++; }

			float version = atof(v.c_str());

			s.str(string());
			s << "OpenGL Version: " << version;
			drawText(210, Settings::getWindowHeight()-30, s.str().c_str());
			*/

			#if DEBUG //Debug text
				s.str(string());
				s << "FPS:" << (int)(frameRate + 0.5);
				drawText(10, Settings::getWindowHeight() - 30, s.str().c_str());

				s.str(string());
				s << "Sound Sources: " << soundManager->getNumSoundSources();
				drawText(400, Settings::getWindowHeight() - 30, s.str().c_str());

				s.str(string());
				s << "Ground Normal: " << player->getGroundNormal().x << "," << player->getGroundNormal().y << "," << player->getGroundNormal().z;
				drawText(400, Settings::getWindowHeight() - 60, s.str().c_str());

				s.str(string());
				s << "Cursor World Pos: " << inputManager.getMouseWorldPosition().x << "," << inputManager.getMouseWorldPosition().y;
				//s << "Cursor World Pos: " << inputManager.getMousePosition().x << "," << inputManager.getMousePosition().y;
				drawText(400, Settings::getWindowHeight() - 90, s.str().c_str());

				if (player != NULL){
					s.str(string());
					s << "Airborne: " << (player->getAir() ? "TRUE" : "False");
					drawText(210, Settings::getWindowHeight() - 30, s.str().c_str());
				}

				s.str(string());
				s << "Pos:";
				drawText(10, Settings::getWindowHeight()-60, s.str().c_str());
				s.str(string());
				s << camera.getPos().x;
				drawText(65, Settings::getWindowHeight()-60, s.str().c_str());
				s.str(string());
				s << camera.getPos().y;
				drawText(65, Settings::getWindowHeight()-85, s.str().c_str());
				s.str(string());
				s << camera.getPos().z;
				drawText(65, Settings::getWindowHeight()-110, s.str().c_str());

				s.str(string());
				s << "Angle:";
				drawText(210, Settings::getWindowHeight()-60, s.str().c_str());
				s.str(string());
				s << camera.getAngle().x;
				drawText(285, Settings::getWindowHeight()-60, s.str().c_str());
				s.str(string());
				s << camera.getAngle().y;
				drawText(285, Settings::getWindowHeight()-85, s.str().c_str());
				s.str(string());
				s << camera.getAngle().z;
				drawText(285, Settings::getWindowHeight()-110, s.str().c_str());
			#endif

		glDepthFunc(GL_LEQUAL);
	endOrtho();
	return 0;
}