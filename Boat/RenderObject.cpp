#include "RenderObject.h"

Model * RenderObject::quad = NULL;
AssetManager * RenderObject::assetManager = NULL;
SoundManager * RenderObject::soundManager = NULL;

RenderObject::RenderObject()
{
	init();
}

RenderObject::RenderObject(string tPath, string sPath)
{
	init();

	texturePath = tPath;
	shaderPath = sPath;
	texture = assetManager->getTexture(texturePath);
	shader = assetManager->getShader(shaderPath);

	//UNTIL DYNAMIC LOADING IS FIXED
	//----------------------------------------------------
	/*if (model == NULL) {
		assetManager->forceLoadModel(modelPath);
		model = assetManager->getModel(modelPath);
	}
	if (texture == NULL) {
		assetManager->forceLoadModel(texturePath);
		texture = assetManager->getTexture(texturePath);
	}
	if (shader == NULL) {
		assetManager->forceLoadShader(shaderPath);
		shader = assetManager->getShader(shaderPath);
	}*/
	//----------------------------------------------------

	position = glm::vec2(0,0);
	size = glm::vec2(100.0f, 100.0f);
	rotation = 0.0f;
}

RenderObject::RenderObject(string tPath, string sPath, glm::vec2 position, float rotation, glm::vec2 size)
{
	init();

	texturePath = tPath;
	shaderPath = sPath;
	texture = assetManager->getTexture(texturePath);
	shader = assetManager->getShader(shaderPath);

	//UNTIL DYNAMIC LOADING IS FIXED
	//----------------------------------------------------
	if (texture == NULL) {
		assetManager->forceLoadModel(texturePath);
		texture = assetManager->getTexture(texturePath);
	}
	if (shader == NULL) {
		assetManager->forceLoadShader(shaderPath);
		shader = assetManager->getShader(shaderPath);
	}
	//----------------------------------------------------

	this->position = position;
	this->rotation = rotation;
	this->size = size;

	updateMatrices();
}

RenderObject::~RenderObject()
{
	
}

void RenderObject::init()
{
	texture = NULL;
	shader = NULL;
	lighting = true;
	position = glm::vec2(0,0);
	rotation = 0.0f;
	size = glm::vec2(100.0f, 100.0f);
}

int RenderObject::update(long elapsedTime)
{
	if (texture == NULL) 
		texture = assetManager->getTexture(texturePath);
	if (shader == NULL) 
		shader = assetManager->getShader(shaderPath);
	return 0;
}

int RenderObject::updateMatrices() 
{
	worldMatrix = glm::translate(glm::vec3(position.x, position.y, 0.0f)) * glm::rotate(glm::mat4(), rotation, glm::vec3(0, 0, 1));
	//worldMatrix = glm::translate(glm::vec3(position.x, position.y, 0.0f)) * glm::rotate(glm::mat4(), rotation, glm::vec3(0, 0, 1)) * glm::rotate(glm::mat4(), 90.0f, glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(size.x, size.y, 1.0f));
	return 0;
}

int RenderObject::render(int index, long totalElapsedTime)
{
	if (shader == NULL || texture == NULL || projMatrix == NULL || viewMatrix == NULL) return -1;;

	Shader * temp = activeShader;

	shader->activate();

	activeShader->setUniformTexture("textureSampler", texture->getTextureID());
	glActiveTexture(GL_TEXTURE0);
	texture->bindTexture();
	glUniform1i(glGetUniformLocation(activeShader->getShaderHandle(), "textureSampler"), 0);

	activeShader->setUniformMatrixf4("projViewMatrix", *projMatrix * *viewMatrix);
	activeShader->setUniformMatrixf4("worldMatrix", worldMatrix * glm::translate(glm::vec3(0, 0, -90 + index*1.0f)) * glm::scale(glm::vec3(size.x, size.y, 0)));
		
	quad->render();

	if (temp != NULL) temp->activate(); //restore old shader

	return 0;
}

int RenderObject::shaderlessRender(int index, long totalElapsedTime, bool useOwnTexture)
{
	if (texture == NULL || projMatrix == NULL || viewMatrix == NULL) return -1;;

	if (useOwnTexture) {
		activeShader->setUniformTexture("textureSampler", texture->getTextureID());
		glActiveTexture(GL_TEXTURE0);
		texture->bindTexture();
		glUniform1i(glGetUniformLocation(activeShader->getShaderHandle(), "textureSampler"), 0);
	}

	activeShader->setUniformMatrixf4("projViewMatrix", *projMatrix * *viewMatrix);
	activeShader->setUniformMatrixf4("worldMatrix", worldMatrix * glm::translate(glm::vec3(0, 0, -10 + index*0.1f)) * glm::scale(glm::vec3(size.x, size.y, 1)));

	quad->render();

	return 0;
}