#ifndef CLIENT_H
#define CLIENT_H

#include "Packet.h"
#include "DataBuffer.h"

#include <string>
using std::string;

class Client
{
public:
	Client();
	~Client();

	DataBuffer * popSendData();
	void pushSendData(char *, int size);
	DataBuffer * popReceiveData();
	void pushReceiveData(char *, int size);

	void setConnected(string name = "Peasant");
	void setDisconnected();
	bool isConnected() { return connected; }
	void setName(string x) { name = x; }
	string getName() { return name; }
	void setPing(int x) { ping = x; }
	void setID(int x) { clientID = x; }
	 
	int aquireMutex();
	int releaseMutex();
private:
	void init();

	Packet packetData;
	bool connected;
	string name;
	int ping;
	int clientID;

	HANDLE clientMutex;
};

#endif CLIENT_H