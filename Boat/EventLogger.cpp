#include "EventLogger.h"

list<string> * EventLogger::processedEventList = NULL;
list<string> * EventLogger::unprocessedEventList = NULL;

void EventLogger::setStatics(list<string> * processedEventList, list<string> * unprocessedEventList)
{
	EventLogger::processedEventList = processedEventList;
	EventLogger::unprocessedEventList = unprocessedEventList;
}

void EventLogger::appendToProcessedEvents(string eventString)
{
	processedEventList->push_back(eventString);
}

void EventLogger::appendToUnprocessedEvents(string eventString)
{
	unprocessedEventList->push_back(eventString);
}