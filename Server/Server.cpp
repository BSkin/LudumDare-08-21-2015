#include "Server.h"

Server::Server(HWND hWnd)
{
	consoleWindow = hWnd;
	gameType = 0;
	mapName = "test";
	mapRotation = list<string>();
	mapRotation.push_back("test");
	maxPlayers = 10;
	updateRate = 64.0f;
}

Server::~Server()
{

}


int Server::run(const char * ini)
{
	readServerData(ini);

	clients = new Client[maxPlayers];
	for (int i = 0; i < maxPlayers; i++) clients[i].setID(i);

	HostThread::setStatics(&maxPlayers, clients);

	running = true;
	HANDLE listenThreadHandle = HostListener::start();

	while (running) {
		if (GetAsyncKeyState(VK_ESCAPE) && (GetForegroundWindow() == consoleWindow) &&
			MessageBox(0, L"Shutdown the Server?", L"Boat Server Shutdown", MB_YESNO) == IDYES) break;

		//Aquire Packet Receive Mutex
		//Deserialize Packet Receive Buffer Data
		//Shit like player info, player actions
		//Release Packet ReceiveMutex
		DataBuffer * temp;
		
		for (int i = 0; i < maxPlayers; i++) {
			aquireClientsMutex(i);
				temp = clients[i].popReceiveData();
			releaseClientsMutex(i);

			//Packet::deserialize(temp);
		}

		//update physics
		//update game logic

		//Aquire Packet Send Mutex
		//Serialize Packet Data, add to Packet Send Buffer
		//Updated player info, new actions (weapons firing, weapons changing, things dieing)
		//Release Packet Send Mutex
	}

	cout << "Shutting Down\n";
	HostListener::stop();
	WaitForSingleObject(listenThreadHandle, INFINITE);
	delete [] clients;
	
	return 0;
}

void Server::readServerData(string iniPath)
{
	//read values from iniPath
	// .
	// .
	// .
}

int Server::aquireClientsMutex(int index)
{
	if (index > maxPlayers) {
		printf("Client Mutex Index Out of bounds.\n");
		return -1;
	}
	return clients[index].aquireMutex();
}

int Server::releaseClientsMutex(int index)
{
	if (index > maxPlayers) {
		printf("Client Mutex Index Out of bounds.\n");
		return -1;
	}
	return clients[index].releaseMutex();
}