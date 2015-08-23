#ifndef SERVER_H
#define SERVER_H

#include "..\Boat\Client.h"

#include <iostream>
#include <fcntl.h>
#include <io.h>
#include "HostListener.h"
#include <string>
#include <list>
using std::string;
using std::list;
using std::cout;
using std::cin;

#define DEBUG 0

#if DEBUG
	#include <vld.h>	
#endif

class Server
{
public:
	Server(HWND hWnd);
	~Server();
	int run(const char * ini);
private:
	void readServerData(string iniPath);

	int aquireClientsMutex(int index);
	int releaseClientsMutex(int index);

	HWND consoleWindow;
	bool running;
	int maxPlayers;
	
	int gameType;
	string mapName;
	list<string> mapRotation;
	float updateRate;

	Client * clients;
};

#endif