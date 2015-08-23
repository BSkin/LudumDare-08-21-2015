#include "MeshObject.h"

MeshObject::MeshObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, int colGroup, int colMask) : RigidObject(t, s, position, direction, rotation, size, "model", colGroup, colMask)
{
	initBullet(colGroup, colMask);
}

MeshObject::~MeshObject()
{
	delete mTriMesh;
	delete colShape;
}

int MeshObject::initBullet(int colGroup, int colMask)
{
	if (model == NULL) return -1;

	int totalVertices = model->getNumVertices();
	float * vertices = new float[totalVertices * 3];
	glBindBuffer(GL_ARRAY_BUFFER, model->getVertexBuffer());
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*totalVertices * 3, vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	mTriMesh = new btTriangleMesh();

	for (int i = 0; i < totalVertices - 1; i += 3) {

		btVector3 v0 = btVector3(
			vertices[3 * (i + 0) + 0],
			vertices[3 * (i + 0) + 1],
			vertices[3 * (i + 0) + 2]);
		btVector3 v1 = btVector3(
			vertices[3 * (i + 1) + 0],
			vertices[3 * (i + 1) + 1],
			vertices[3 * (i + 1) + 2]);
		btVector3 v2 = btVector3(
			vertices[3 * (i + 2) + 0],
			vertices[3 * (i + 2) + 1],
			vertices[3 * (i + 2) + 2]);
		mTriMesh->addTriangle(v0, v1, v2);
	}

	colShape = new btBvhTriangleMeshShape(mTriMesh, true);
	//delete mTriMesh;
	delete[] vertices;

	return RigidObject::initBullet("model", colGroup, colMask);
}