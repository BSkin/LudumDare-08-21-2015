#ifndef HOST_THREAD_H
#define HOST_THREAD_H

#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#include <iostream>
#include "..\Boat\Settings.h"
#include "..\Boat\Client.h"
using std::cout;
using std::cin;

class HostThread
{
public:
	static void start(SOCKET hostSocket);
	static void stopAll();

	static int aquireClientsMutex(int index);
	static int releaseClientsMutex(int index);
	static int getFirstAvailableClientID(string clientName);
	static void freeClient(int index);

	static void setStatics(int * maxClients, Client * clients);
private:
	static unsigned int WINAPI startHostThread(LPVOID);
	static bool running;

	static int * maxClients;
	static Client * clients;
};

#endif