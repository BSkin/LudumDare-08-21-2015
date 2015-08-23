#ifndef PACKET_H
#define PACKET_H

#include "GameObject.h"
#include "EventSerializer.h"
#include "DataBuffer.h"
#include "Settings.h"

#include <sstream>
#include <iostream>
#include <string>
#include <list>
#include <array>
using std::istringstream;
using std::getline;
using std::string;
using std::list;
using std::array;

class Packet
{
public:
	Packet();
	~Packet();

	const char * getSendBuffer();
	int getSendBufferSize();
	
	const char * getReceiveBuffer();
	int getReceiveBufferSize();
	
	void appendToSendBuffer(char * x, int size);
	void appendToSendBuffer(GameObject * o);
	void clearSendBuffer();

	void appendToReceiveBuffer(char * x, int size);
	void appendToReceiveBuffer(GameObject * o);
	void clearReceiveBuffer();

	void cleanup();
private:
	void fillSendBuffer();
	void fillReceiveBuffer();

	char sendBuffer[MAX_PACKET_SIZE-2];
	int sendBufferSize;
	list<DataBuffer *> sendBackBuffer;

	char receiveBuffer[MAX_PACKET_SIZE-2];
	int receiveBufferSize;
	list<DataBuffer *> receiveBackBuffer;
};

#endif