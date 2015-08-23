#ifndef UTILITIES_H
#define UTILITIES_H

#include "SDL2-2.0.3\include\SDL.h"
#include "SDL2-2.0.3\include\SDL_opengl.h"
#include "Bullet\src\btBulletDynamicsCommon.h"

#include "GaussianDistributionHandler.h"

#include <random>
#include <iostream>
#include <string>
#include <list>
using std::cout;
using std::string;
using std::list;

namespace Utilities 
{
	static float frand()
	{
		long l = ((long)(rand()) * 1103515245 + 12345) & 0x7FFFFFFF;
		long r = l >> (31 - 24);
		return r / (float)(1 << 24);
	}
	static void grandSeed(unsigned long seed) { GaussianDistributionHandler::seed(seed); }
	static float grand()
	{
		return GaussianDistributionHandler::getRandomGaussianFloat();
	}
	static bool dirExists(string dirName_in)
	{
		DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return true;   // this is a directory!

		return false;    // this is not a directory!
	}
	static btVector3 stringToBTVector3(string input)
	{
		if (input == "") return btVector3(0,0,1);
		float x,y,z;
		x = y = z = 0.0f;
		short commaCount = 0;
		string temp;
		for (int i = 0; i < (int)input.length(); i++)
		{
			if (input[i] == ',')
			{
				if (commaCount == 0)		x = (float)atof(temp.c_str());
				else if (commaCount == 1)	y = (float)atof(temp.c_str());
				commaCount++;
				temp = "";
			}

			else temp += input[i];
		}
	
		z = (float)atof(temp.c_str());
		return btVector3(btScalar(x), btScalar(y), btScalar(z));
	}

	template<typename T>
	static bool listContainsElement(list<T> * l, T element)
	{
		for (list<string>::iterator iter = l->begin(); iter != l->end(); iter++)
			if (*iter == element) return true;
		return false;
	}
	static btVector3 glmToBT3(glm::vec3 val) { return btVector3(val.x, val.y, val.z); }
	static glm::vec3 btToGLM3(const btVector3 * val) { return glm::vec3(val->x(), val->y(), val->z()); }
	static float magSqr(glm::vec3 val) { return val.x*val.x + val.y*val.y + val.z*val.z; }
	static float magSqr(glm::vec2 val) { return val.x*val.x + val.y*val.y; }
	static float magSqr(const btVector3 * val) { return val->x()*val->x() + val->y()*val->y() + val->z()*val->z(); }
	static float magnitude(glm::vec3 val) { return sqrt(magSqr(val)); }
	static float magnitude(glm::vec2 val) { return sqrt(magSqr(val)); }
	static float magnitude(const btVector3 * val) { return sqrt(magSqr(val)); }
	static int mod(int a, int b) { return (a%b + b) % b; }
	static void printfDataPacket(const char * buffer, int size) 
	{
		for (int i = 0; i < size; i++) {
			if (i == 0) printf("(%d) ", *(int*)buffer);
			else if (i > 3) printf("%c", buffer[i]);
		}
	}
	static void printfDataPacket(char * buffer, int size) { printfDataPacket((const char*)buffer, size); }
};


#endif