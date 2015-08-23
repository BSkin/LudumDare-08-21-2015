#include "EventHandler.h"

list<string> * EventHandler::processedEventList = NULL;
list<string> * EventHandler::unprocessedEventList = NULL;

list<GameObject *> * EventHandler::gameObjectList = NULL;

void EventHandler::setStatics(list<string> * processedEventList, list<string> * unprocessedEventList)
{
	EventHandler::processedEventList = processedEventList;
	EventHandler::unprocessedEventList = unprocessedEventList;
}

void EventHandler::processEvents()
{
	while (unprocessedEventList->size() > 0) {
		//pop from unprocessed event list

		//process event

		//push to processed event list
	}
}