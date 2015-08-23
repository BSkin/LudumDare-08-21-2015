#include "Client.h"

Client::Client()
{
	clientID = -1;
	clientMutex = CreateMutex(NULL, FALSE, NULL);
	init();
}

Client::~Client()
{
	packetData.cleanup();
	if (clientMutex != NULL) {
		CloseHandle(clientMutex);
		clientMutex = NULL;
	}
}

void Client::init()
{
	connected = false;
	name = "";
	ping = 0;
}

DataBuffer * Client::popSendData()
{
	DataBuffer * temp = new DataBuffer(packetData.getSendBuffer(), packetData.getSendBufferSize());
	packetData.clearSendBuffer();
	return temp;
}

void Client::pushSendData(char * data, int size)
{
	packetData.appendToSendBuffer(data, size);
}

DataBuffer * Client::popReceiveData()
{
	DataBuffer * temp = new DataBuffer(packetData.getReceiveBuffer(), packetData.getReceiveBufferSize());
	packetData.clearReceiveBuffer();
	return temp;
}

void Client::pushReceiveData(char * data, int size)
{
	packetData.appendToReceiveBuffer(data, size);
}

void Client::setConnected(string name)
{
	connected = true;
	this->name = name;
}

void Client::setDisconnected()
{
	init();
}

int Client::aquireMutex()
{
	DWORD waitResult = WaitForSingleObject(clientMutex, INFINITE);
	if (waitResult != WAIT_OBJECT_0) {
		printf("Error aquiring client %d mutex.\n", clientID);
		return -1;
	}
	return 0;
}

int Client::releaseMutex()
{
	if (!ReleaseMutex(clientMutex)) {
		printf("Error releasing client %d mutex.\n", clientID);
		return -1;
	}
	return 0;
}