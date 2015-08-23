#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "AssetLoader.h"

class AssetManager
{
public:
	AssetManager();
	~AssetManager();

	void startAssetLoader();
	void setStatics(HDC * window, HGLRC * loading, HGLRC * primary) { AssetLoader::displayWindow = window; AssetLoader::loadingContext = loading; AssetLoader::primaryContext = primary; }
	void shutdown() { assetLoader.running = false; }

	void loadModel(string path);
	void loadTexture(string path);
	void loadShader(string path);
	void loadSound(string path);
	void forceLoadModel(string path);
	void forceLoadTexture(string path);
	void forceLoadShader(string path);
	void forceLoadSound(string path);

	Model * getModel(string path);
	Texture * getTexture(string path);
	Shader * getShader(string path);
	Sound * getSound(string path);
private:
	void pushToObjectsList(string path);
	
	AssetLoader assetLoader;

	list<string> objectsToLoad;
	modelMap	models;
	textureMap	textures;
	shaderMap	shaders;
	soundMap	sounds;

	HANDLE objectsMutex;
	HANDLE modelsMutex;
	HANDLE texturesMutex;
	HANDLE shadersMutex;
	HANDLE soundsMutex;
};

#endif