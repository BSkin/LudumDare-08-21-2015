#ifndef HOST_LISTENER_H
#define HOST_LISTENER_H

#include <WinSock2.h>
#include <Windows.h>
#include <process.h>
#include <iostream>
#include "..\Boat\Settings.h"
#include "..\Boat\Packet.h"
#include "HostThread.h"
using std::cout;
using std::cin;

class HostListener
{
public:
	static HANDLE start();
	static void stop();
private:
	static unsigned int WINAPI startHostListenerThread(LPVOID);
	static bool running;
};

#endif