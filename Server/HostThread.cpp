#include "HostThread.h"

bool HostThread::running = false;
int * HostThread::maxClients = NULL;
Client * HostThread::clients = NULL;

void HostThread::start(SOCKET socket) 
{
	running = true;
	_beginthreadex(NULL, 0, startHostThread, (LPVOID)socket, NULL, NULL); 
}

void HostThread::stopAll()
{
	running = false;
}

unsigned int WINAPI HostThread::startHostThread(LPVOID arg)
{
	SOCKET hostSocket = (SOCKET)arg;
	DataBuffer * receiveBuffer = new DataBuffer(MAX_PACKET_SIZE);
	DataBuffer * sendBuffer = new DataBuffer(MAX_PACKET_SIZE);
	DataBuffer * tempSendBuffer;
	int returnValue;
	string username;
	int clientID;

	#pragma region Initial Handshake
	//Receive Username
	receiveBuffer->clear();
	returnValue = recv(hostSocket, receiveBuffer->getData(), MAX_PACKET_SIZE, 0);
	if (returnValue == 0) return 1;
	else if (returnValue == SOCKET_ERROR) {
		printf("recv() failed: %d\n", WSAGetLastError());
		return 1;
	}
	receiveBuffer->setSize(returnValue);
	receiveBuffer->set(returnValue, '\0');
	username = string(receiveBuffer->getData());

	//Aquire first available client ID
	clientID = getFirstAvailableClientID(username);
	if (clientID == -1) {
		printf("Server Full. Closing thread.\n");
		return 0;
	}

	printf("%s connected (Client %d)\n", username.c_str(), clientID);

	//Send Client ID
	string idString = std::to_string(clientID);
	returnValue = send(hostSocket, idString.c_str(), idString.size()+1, 0);
	if (returnValue == 0) {
		freeClient(clientID);
		return 1;
	}
	else if (returnValue == SOCKET_ERROR) {
		printf("send() failed: %d\n", WSAGetLastError());
		freeClient(clientID);
		return 1;
	}
	printf("Sent Client ID: %d\n", clientID);
	#pragma endregion

	while(running) {
		returnValue = recv(hostSocket, receiveBuffer->getData(), MAX_PACKET_SIZE, 0);
		if (returnValue == 0) break;
		else if (returnValue == SOCKET_ERROR) {
            printf("recv() failed: %d\n", WSAGetLastError());
            break;
        }
		receiveBuffer->setSize(returnValue);
		receiveBuffer->set(returnValue, '\0');
		printf("Host Rec[%d]: ", clientID);
		printfDataPacket(receiveBuffer->getData(), receiveBuffer->getSize());
		printf("\n");

		aquireClientsMutex(clientID);
			clients[clientID].pushReceiveData(receiveBuffer->getData(), receiveBuffer->getSize());
		//releaseClientsMutex(clientID);
		//aquireClientsMutex(clientID);
			tempSendBuffer = clients[clientID].popSendData();
		releaseClientsMutex(clientID);

		int packetSize = tempSendBuffer->getSize();
		sendBuffer->clear();
		sendBuffer->copy(0, &packetSize, sizeof(int));
		sendBuffer->copy(4, tempSendBuffer->getData(), tempSendBuffer->getSize());
		delete tempSendBuffer;

		returnValue = send(hostSocket, sendBuffer->getData(), sendBuffer->getSize(), 0);
		if (returnValue == 0) break;
		else if (returnValue == SOCKET_ERROR) {
            printf("send() failed: %d\n", WSAGetLastError());
            break;
        }
		printf("Host Snd[%d]: ", clientID);
		printfDataPacket(sendBuffer->getData(), sendBuffer->getSize());
		printf("\n");

		Sleep(1000);
		//Sleep(1000.0f/NETWORK_UPDATE_RATE); //Change to factor in duration of the function
	}

	delete receiveBuffer;
	delete sendBuffer;

	freeClient(clientID);
	printf("Connection to client %d lost.\n", clientID);
	return 0;
}

void HostThread::setStatics(int * maxClients, Client * clients)
{
	HostThread::maxClients = maxClients;
	HostThread::clients = clients;
}

int HostThread::aquireClientsMutex(int index)
{
	if (index > *maxClients) {
		printf("Client Mutex Index Out of bounds.\n");
		return -1;
	}
	return clients[index].aquireMutex();
}

int HostThread::releaseClientsMutex(int index)
{
	if (index > *maxClients) {
		printf("Client Mutex Index Out of bounds.\n");
		return -1;
	}
	return clients[index].releaseMutex();
}

int HostThread::getFirstAvailableClientID(string username)
{
	for (int i = 0; i < *maxClients; i++) {
		aquireClientsMutex(i);
		if (!clients[i].isConnected()) {
			clients[i].setConnected(username);
			releaseClientsMutex(i);
			return i;
		}
		releaseClientsMutex(i);
	}
	return -1; //Server is Full
}

void HostThread::freeClient(int index)
{
	if (index >= *maxClients) return;

	aquireClientsMutex(index);
		clients[index].setDisconnected();
	releaseClientsMutex(index);
}