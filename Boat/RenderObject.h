#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"	

#include "AssetManager.h"
#include "SoundManager.h"
#include "Utilities.h"
#include "Settings.h"
#include "Camera.h"
using namespace Utilities;

class RenderObject
{
public:
	RenderObject();
	RenderObject(string, string); //ok for now
	RenderObject(string, string, glm::vec2 position, float rotation, glm::vec2 size); //ok for now
	virtual ~RenderObject();

	virtual int update(long elapsedTime);
	virtual int render(int index, long totalElapsedTime);
	virtual int shaderlessRender(int index, long totalElapsedTime, bool useOwnTexture = false);
	virtual int updateMatrices();

	virtual void setPosition(float x, float y) { position = glm::vec2(x, y); }

	static void setStatics(glm::mat4 * view, glm::mat4 * proj, Model * q, AssetManager * ass, SoundManager * sM) {
		viewMatrix = view; projMatrix = proj; quad = q; assetManager = ass; soundManager = sM;
	}

protected:
	string texturePath;
	string shaderPath;

	Texture * getTexture(string path) { return assetManager->getTexture(path); }
	Shader * getShader(string path) { return assetManager->getShader(path); }
	Sound * getSound(string path) { return assetManager->getSound(path); }
	void addSoundSource(SoundSource * s) { soundManager->addSound(s); }
	
	static Model * quad;
	Texture * texture;
	Shader * shader;

	bool lighting;

	glm::mat4 worldMatrix;
	glm::vec2 position;
	glm::vec2 size;
	float rotation;
	bool flipSprite;

	#define activeCamera Camera::getActiveCamera()
	#define activeShader Shader::getActiveShader()

	static glm::mat4 * viewMatrix;
	static glm::mat4 * projMatrix;
private:
	void init();

	static AssetManager * assetManager;
	static SoundManager * soundManager;
};

#endif