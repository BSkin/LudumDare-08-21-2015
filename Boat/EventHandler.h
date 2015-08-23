#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include "GameObject.h"
#include "PlayerObject.h"

#include <list>
#include <string>
using std::list;
using std::string;

class EventHandler
{
public:
	static void processEvents();

	static void setStatics(list<string> * processed, list<string> * unprocessed);
private:
	static list<string> * processedEventList;
	static list<string> * unprocessedEventList;

	static list<GameObject *> * gameObjectList;
};

#endif