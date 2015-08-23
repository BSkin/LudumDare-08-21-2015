#include "RigidObject.h"

RigidObject::RigidObject() : PhysicsObject() {}
RigidObject::RigidObject(string t, string s) : PhysicsObject(t, s) {}
RigidObject::RigidObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, float mass, string path, int colGroup, int colMask) : PhysicsObject(t, s, position, rotation, size, mass) 
{
	collisionPath = path;
	collisionGroup = colGroup;
	collisionMask = colMask;
}
RigidObject::~RigidObject() 
{ 
	if (!physInit) return;

	dynamicsWorld->removeCollisionObject( body );
	delete body->getMotionState();
	delete body; 
}

int RigidObject::initBullet(string path, int collisionGroup, int collisionMask)
{
	//Collision Shape already created by child class
	
	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();

	mass = ((path == "model" || path == "heightField") ? 0.0f : mass);
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0,0,0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass,localInertia);
		
	/*startTransform.setOrigin(btVector3(
					btScalar(position.x),
					btScalar(position.y),
					btScalar(0.0f)));*/
	
	worldMatrix = glm::translate(glm::vec3(position.x, position.y, 0.0f)) * glm::rotate(glm::mat4(), rotation, glm::vec3(0, 0, 1));
	startTransform.setFromOpenGLMatrix(glm::value_ptr(worldMatrix));

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	body = new btRigidBody(mass,myMotionState,colShape,localInertia, this);

	body->setLinearFactor(btVector3(1, 1, 0));
	body->setAngularFactor(btVector3(0, 0, 1));

	dynamicsWorld->addRigidBody(body, collisionGroup, collisionMask);
	body->setFriction(0.5f);

	body->setLinearFactor(btVector3(1, 1, 0));
	body->setAngularFactor(btVector3(0, 0, 1));

	//body->setActivationState(ISLAND_SLEEPING);	
	//body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	return PhysicsObject::initBullet();
}

int RigidObject::render(int i, long totalElapsedTime)
{
	if (!physInit) return -1;
	body->getWorldTransform().getOpenGLMatrix(glm::value_ptr(worldMatrix)); // * scale
	worldMatrix = worldMatrix;// *glm::scale(glm::vec3(size.x, size.y, 1.0f));
	return GameObject::render(i, totalElapsedTime);
}

int RigidObject::update(long elapsedTime)
{
	PhysicsObject::update(elapsedTime);
	if (!physInit) initBullet(collisionPath, collisionGroup, collisionMask);
	if (!physInit) return -1;
	
	return 0;
}

void RigidObject::collisionCallback(RigidObject * otherObject)
{
	
}

void RigidObject::setRotation(float rotation) 
{
	btVector3 posBT = body->getCenterOfMassPosition(); 
	glm::mat4 posGLM = glm::translate(glm::vec3(posBT.x(), posBT.y(), posBT.z()));
	glm::mat4 rotGLM = glm::rotate(glm::mat4(), rotation, glm::vec3(0,0,1));
	worldMatrix = posGLM * rotGLM;

	btTransform trans;	
	trans.setFromOpenGLMatrix(glm::value_ptr(worldMatrix));
	body->setWorldTransform(trans);
}

void RigidObject::setMass(float x)
{
	if (!physInit) return;

	//dynamicsWorld->removeRigidBody( body );
	btVector3 inertia;
	body->getCollisionShape()->calculateLocalInertia( x, inertia );
	body->setMassProps(x, inertia);
	//dynamicsWorld->addRigidBody( body );
}