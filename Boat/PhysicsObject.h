#ifndef PHYSICSOBJECT_H
#define PHYSICSOBJECT_H

#include "GameObject.h"
#include "Bullet\src\btBulletDynamicsCommon.h"
#include "Bullet\src\BulletCollision\CollisionShapes\btBvhTriangleMeshShape.h"

#include <unordered_map>
#include <map>
using std::unordered_map;
using std::make_pair;
#define my_map unordered_map<string, btCollisionShape *>

#define BIT(x) (1<<(x))
#define COL_NONE	0
#define COL_STRICT	BIT(0)
#define COL_DEFAULT	BIT(0) | BIT(1)
#define COL_PROJ	BIT(1)

struct CollisionShape {
	string type;
	string pos;
	string size;
	string axis;
	string rot;
	list<btVector3> vertices;
	CollisionShape() : type(""), pos(""), size(""), axis(""), rot("")
	{
		vertices = list<btVector3>();
	}
};

class PhysicsObject : public GameObject
{
public:
	PhysicsObject();
	PhysicsObject(string, string); //ok for now
	PhysicsObject(string, string, glm::vec2 position, float rotation, glm::vec2 size, float mass); //ok for now
	virtual ~PhysicsObject() = 0;

	virtual int render(int i, long elapsedTime);
	virtual int update(long elapsedTime);

	virtual void setVelocity(glm::vec2 vel) {}// { body->setLinearVelocity(btVector3(vel.x, vel.y, vel.z)); }
	virtual void setVelocity(float x, float y) {}// { body->setLinearVelocity(btVector3(x, y, z)); }
	virtual void setGravity(glm::vec2 grav) {}// { body->setGravity(btVector3(grav.x, grav.y, grav.z)); }
	virtual void setGravity(float x, float y) {}// { body->setGravity(btVector3(x, y, z)); }
	virtual void setRotation(float rotation) {}
	virtual btVector3 getPosition() { return btVector3(0,0,0); }
	virtual void wakeUp() {}
	virtual void applyCentralImpulse(float x, float y) {}
	string getCollisionType() { return collisionType; }

	static void setStatics(btAlignedObjectArray<btCollisionShape*> * col, btDiscreteDynamicsWorld * dyn) { collisionShapes = col; dynamicsWorld = dyn; }
	static btCollisionShape * readCollisionData(char * path);
	static void populateCollisionLibrary();
	static void deleteCollisionLibrary();
	static btCollisionShape * getCollisionShape(string key);
protected:
	virtual int initBullet();
	static btCollisionShape * createCollisionShape(CollisionShape * collisionShapeData);
	static btCollisionShape * createConvexHullShape(list<btVector3> * pointList);

	static btAlignedObjectArray<btCollisionShape*> * collisionShapes;
	static btDiscreteDynamicsWorld * dynamicsWorld;
	static my_map * collisionLibrary;
	static list<btCollisionShape*> * collisionSubShapes;

	bool physInit;
	btCollisionShape* colShape;
	string collisionType;
	float mass;
private:
};

#endif