#include "PhysicsObject.h"

btAlignedObjectArray<btCollisionShape*> *	PhysicsObject::collisionShapes =	NULL;
btDiscreteDynamicsWorld *					PhysicsObject::dynamicsWorld =		NULL;
list<btCollisionShape*> *					PhysicsObject::collisionSubShapes = NULL;
unordered_map<string, btCollisionShape *> * PhysicsObject::collisionLibrary = NULL;

PhysicsObject::PhysicsObject() : GameObject(), physInit(FALSE) { collisionType = "default"; }
PhysicsObject::PhysicsObject(string t, string s) : GameObject(t, s), physInit(FALSE) { mass = 1.0f; collisionType = "default"; }
PhysicsObject::PhysicsObject(string t, string s, glm::vec2 position, float rotation, glm::vec2 size, float mass) : GameObject(t, s, position, rotation, size), physInit(FALSE) { this->mass = mass; collisionType = "default"; }

PhysicsObject::~PhysicsObject()
{

}

int PhysicsObject::initBullet()
{
	physInit = true;
	return 0;
}

int PhysicsObject::render(int i, long totalElapsedTime)
{
	/*
	body->getWorldTransform().getOpenGLMatrix(glm::value_ptr(worldMatrix)); // * scale
	normalMatrix = glm::transpose(glm::inverse(worldMatrix));
	*/return GameObject::render(i, totalElapsedTime);
}

int PhysicsObject::update(long elapsedTime)
{
	GameObject::update(elapsedTime);

	return 0;
}

btCollisionShape * PhysicsObject::createCollisionShape(CollisionShape * input)
{
	btCollisionShape * colShape = NULL;
	if (input->type == "box")
		colShape = new btBoxShape(stringToBTVector3(input->size)/2);
	else if (input->type == "sphere")
		colShape = new btSphereShape(stringToBTVector3(input->size).x()/2);
	else if (input->type == "capsule")
		colShape = new btCapsuleShape(stringToBTVector3(input->size).x()/2, stringToBTVector3(input->size).y());
	else if (input->type == "cylinder")
		colShape = new btCylinderShape(stringToBTVector3(input->size)/2);
	else if (input->type == "cylinderX")
		colShape = new btCylinderShapeX(stringToBTVector3(input->size)/2);
	else if (input->type == "cylinderZ")
		colShape = new btCylinderShapeZ(stringToBTVector3(input->size)/2);
	else if (input->type == "hull")
		colShape = createConvexHullShape(&(input->vertices));
	else
		colShape = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));
	
	return colShape;
}

btCollisionShape * PhysicsObject::createConvexHullShape(list<btVector3> * pointList)
{
	btCollisionShape * temp = new btConvexHullShape();
	while (pointList->size() > 0) {
		btVector3 tempPoint = btVector3(pointList->front());
		((btConvexHullShape*)temp)->addPoint(tempPoint);
		pointList->pop_front();
	}
	((btConvexHullShape*)temp)->recalcLocalAabb();
	return temp;
}

btCollisionShape * PhysicsObject::readCollisionData(char * path)
{
	SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, false};
	CreateDirectory("Collision Shapes", &sa);

	FILE * file;
	char * configName = path;
	file = fopen(configName, "r");
	if (file == NULL) { fclose(file); return NULL; } //no file found, Failed to Generate Collision Shape
	fclose(file);

	list<CollisionShape> * shapes = new list<CollisionShape>();

	ifstream fileStream(configName);
	string s;
	string var, val;
	while (fileStream.peek() != -1)
	{
		s = var = val = "";
		getline(fileStream, s);
		bool eq = false;
		int i = 1;

		if (s == "}" || s == "{") continue;
		else if (s == "box") { shapes->push_back(CollisionShape()); shapes->back().type = "box"; continue; }
		else if (s == "sphere") { shapes->push_back(CollisionShape()); shapes->back().type = "sphere"; continue; }
		else if (s == "capsule") { shapes->push_back(CollisionShape()); shapes->back().type = "capsule"; continue; }
		else if (s == "cylinderY" || s ==  "cylinder") { shapes->push_back(CollisionShape()); shapes->back().type = "cylinder"; continue; }
		else if (s == "cylinderX") { shapes->push_back(CollisionShape()); shapes->back().type = "cylinderX"; continue; }
		else if (s == "cylinderZ") { shapes->push_back(CollisionShape()); shapes->back().type = "cylinderZ"; continue; }
		else if (s == "hull") { shapes->push_back(CollisionShape()); shapes->back().type = "hull"; shapes->back().vertices = list<btVector3>(); continue; }

		else
		{
			while (i < s.size())
			{
				if (s[i] == '=')	eq = true;
				else if (eq)		val += s[i];
				else				var += s[i];
				i++;
			}

			if (var == "pos")		shapes->back().pos = val;
			else if (var == "size")	shapes->back().size = val;
			else if (var == "axis")	shapes->back().axis = val;
			else if (var == "rot")	shapes->back().rot = val;
			else if (var == "vert")	shapes->back().vertices.push_back(stringToBTVector3(val));
		}
	}
	fileStream.close();

	if (shapes->size() == 0) return NULL;
	else if (shapes->size() == 1)
	{
		btCollisionShape * colShape = createCollisionShape(&(shapes->front()));
		shapes->pop_front();
		delete shapes;
		return colShape;
	}
	else
	{
		btCompoundShape * compoundShape = new btCompoundShape();
		int isd = COMPOUND_SHAPE_PROXYTYPE;
		while (shapes->size() > 0)
		{
			btCollisionShape * colShape = createCollisionShape(&shapes->front());

			btTransform trans; trans.setIdentity();
			trans.setOrigin(stringToBTVector3(shapes->front().pos));
			trans.setRotation(btQuaternion(stringToBTVector3(shapes->front().axis), atof(shapes->front().rot.c_str())));

			collisionSubShapes->push_back(colShape);
			compoundShape->addChildShape(trans, colShape);
			shapes->pop_front();
		}

		delete shapes;
		return compoundShape;
	}
}

void PhysicsObject::populateCollisionLibrary()
{
	if (collisionLibrary != NULL || collisionSubShapes != NULL) 
		deleteCollisionLibrary();

	collisionLibrary = new my_map();
	collisionSubShapes = new list<btCollisionShape*>();

	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile("Collision Shapes\\*.prop", &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			//printf("%s\n", data.cFileName);
			char temp[256]; temp[0] = '\0'; strcpy(temp, "Collision Shapes\\"); strcat(temp, data.cFileName);
			string name = string(data.cFileName);
			while (name.back() != '.') name.pop_back();
			name.pop_back();
			collisionLibrary->insert(std::pair<string, btCollisionShape*>(name, PhysicsObject::readCollisionData(temp)));
		}
		while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

void PhysicsObject::deleteCollisionLibrary()
{
	while (collisionSubShapes->size() > 0) {
		delete collisionSubShapes->back();
		collisionSubShapes->pop_back();
	}
	delete collisionSubShapes;

	for (my_map::iterator it = collisionLibrary->begin(); it != collisionLibrary->end(); it++) {
		/*if (it->second->getShapeType() == COMPOUND_SHAPE_PROXYTYPE) {
			while (((btCompoundShape *)it->second)->getNumChildShapes() > 0) {
				btCollisionShape * childShape = ((btCompoundShape *)it->second)->getChildShape(0);
				((btCompoundShape *)it->second)->removeChildShapeByIndex(0);
				delete childShape;
			}
		}*/
		delete it->second;
		it->second = 0;
	}
	collisionLibrary->clear();
	delete collisionLibrary;
}

btCollisionShape * PhysicsObject::getCollisionShape(string key)
{
	my_map::iterator it = collisionLibrary->find(key);
	return (it == collisionLibrary->end()) ? NULL : it->second;
}