#include "ClientThread.h"

bool ClientThread::running = false;
Client * ClientThread::client = NULL;
HANDLE ClientThread::runningMutex = CreateMutex(NULL, FALSE, NULL);

void ClientThread::cleanup()
{
	if (runningMutex != NULL) {
		CloseHandle(runningMutex);
		runningMutex = NULL;
	}
}

HANDLE ClientThread::connectToServer(char * ipAddress)
{
	aquireRunningMutex();
		if (running) {
			releaseRunningMutex();	
			return NULL; //can only be connected to 1 host at a time
		}
		running = true;
	releaseRunningMutex();
	return (HANDLE)_beginthreadex(NULL, 0, startClientThread, (LPVOID)ipAddress, NULL, NULL); 
}

void ClientThread::disconnectFromServer()
{
	aquireRunningMutex();
		running = false;
	releaseRunningMutex();
}

unsigned int ClientThread::startClientThread(LPVOID arg)
{
	#pragma region Initialize
	char * ip = (char*)arg;
	char hostName[128];

	WSADATA wsaData;
	SOCKET clientSocket;
	DataBuffer * receiveBuffer = new DataBuffer(MAX_PACKET_SIZE);
	DataBuffer * sendBuffer = new DataBuffer(MAX_PACKET_SIZE);
	DataBuffer * tempSendBuffer;
	int returnValue;

	sockaddr_in server;
	hostent * host = NULL;

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("Failed to load Winsock library!\n");
        return 1;
    }

	printf("Attempting to connect to %s:%d%\n", ip, DEFAULT_PORT);
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf("socket() failed: %d\n", WSAGetLastError());
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);
    server.sin_addr.s_addr = inet_addr(ip);

	if (server.sin_addr.s_addr == INADDR_NONE) {
        host = gethostbyname(hostName);
        if (host == NULL) {
            printf("Unable to resolve server: %s\n", hostName);
            return 1;
        }
        CopyMemory(&server.sin_addr, host->h_addr_list[0], host->h_length);
    }
	if (connect(clientSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("connect() failed: %d\n", WSAGetLastError());
        return 1;
    }
	char * serverIp = inet_ntoa(server.sin_addr);
	int serverPort = server.sin_port;
	printf("Connected to %s:%d\n", serverIp, serverPort);
	#pragma endregion

	#pragma region Initial Handshake
	//Send Username
	client->aquireMutex();
		client->setName(Settings::getUsername());
		string username = client->getName();
	client->releaseMutex();

	returnValue = send(clientSocket, username.c_str(), username.size()+1, 0);
	if (returnValue == 0) return 1;
	else if (returnValue == SOCKET_ERROR) {
		printf("send() failed: %d\n", WSAGetLastError());
		return 1;
	}
	printf("Sent Username: %s\n", username.c_str());

	//Receive Client ID
	receiveBuffer->clear();
	returnValue = recv(clientSocket, receiveBuffer->getData(), MAX_PACKET_SIZE, 0);
	if (returnValue == 0) return 1;
	else if (returnValue == SOCKET_ERROR) {
		printf("recv() failed: %d\n", WSAGetLastError());
		return 1;
	}
	receiveBuffer->setSize(returnValue);
	receiveBuffer->set(returnValue, '\0');
	Settings::setClientID(atoi(receiveBuffer->getData()));
	printf("Received Client ID: %.*s\n", receiveBuffer->getSize(), receiveBuffer->getData());
	#pragma endregion
	
	while (running) {
		#pragma region Send
		client->aquireMutex();
			tempSendBuffer = client->popSendData();
		client->releaseMutex();

		int packetSize = tempSendBuffer->getSize();
		sendBuffer->clear();
		sendBuffer->copy(0, &packetSize, sizeof(int));
		sendBuffer->copy(4, tempSendBuffer->getData(), tempSendBuffer->getSize());
		delete tempSendBuffer;

		returnValue = send(clientSocket, sendBuffer->getData(), sendBuffer->getSize(), 0);
		if (returnValue == 0) break;
        else if (returnValue == SOCKET_ERROR) {
            printf("send() failed: %d\n", WSAGetLastError());
            break;
        }
		printf("Client Snd[%dB]: ", sendBuffer->getSize());
		printfDataPacket(sendBuffer->getData(), sendBuffer->getSize());
		printf("\n");
		#pragma endregion

		#pragma region Receive
		receiveBuffer->clear();
		returnValue = recv(clientSocket, receiveBuffer->getData(), MAX_PACKET_SIZE, 0);
		if (returnValue == 0) break;
        else if (returnValue == SOCKET_ERROR) {
            printf("recv() failed: %d\n", WSAGetLastError());
            break;
        }
		receiveBuffer->setSize(returnValue);
        //receiveBuffer.set(returnValue, '\0');
		printf("Client Rec[%dB]: ", receiveBuffer->getSize());
		printfDataPacket(receiveBuffer->getData(), receiveBuffer->getSize());
		printf("\n");

		client->aquireMutex();
			client->pushReceiveData(receiveBuffer->getData(), receiveBuffer->getSize());
		client->releaseMutex();
		#pragma endregion

		Sleep(1000);
		//Sleep(1000.0f/NETWORK_UPDATE_RATE); //Change to factor in duration of the function
	}

	delete sendBuffer;
	delete receiveBuffer;

	closesocket(clientSocket);
	printf("Closing Socket\n");
    WSACleanup();

	return 0;
}

bool ClientThread::isRunning()
{
	bool temp;
	aquireRunningMutex();
		temp = running;
	releaseRunningMutex();
	return temp;
}

void ClientThread::setStatics(Client * client)
{
	ClientThread::client = client;
}

int ClientThread::aquireRunningMutex()
{
	DWORD waitResult = WaitForSingleObject(runningMutex, INFINITE);
	if (waitResult != WAIT_OBJECT_0) {
		printf("Error aquiring running mutex.\n");
		return -1;
	}
	return 0;
}

int ClientThread::releaseRunningMutex()
{
	if (!ReleaseMutex(runningMutex)) {
		printf("Error releasing running mutex.\n");
		return -1;
	}
	return 0;
}