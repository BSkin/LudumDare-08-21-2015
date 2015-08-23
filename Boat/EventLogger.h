#ifndef EVENT_LOGGER_H
#define EVENT_LOGGER_H

#include <list>
#include <string>
using std::list;
using std::string;

class EventLogger
{
public:
	static void appendToProcessedEvents(string eventString);
	static void appendToUnprocessedEvents(string eventString);

	static void setStatics(list<string> * processed, list<string> * unprocessed);
private:
	static list<string> * processedEventList;
	static list<string> * unprocessedEventList;
};

#endif