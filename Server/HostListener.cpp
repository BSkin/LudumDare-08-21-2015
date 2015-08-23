#include "HostListener.h"

bool HostListener::running = false;

HANDLE HostListener::start() {
	if (running) return 0; //only 1 listener at a time
	running = true;
	return (HANDLE)_beginthreadex(NULL, 0, startHostListenerThread,  NULL, NULL, NULL); 
}

void HostListener::stop()
{
	running = false;
}

unsigned int HostListener::startHostListenerThread(LPVOID)
{
	WSADATA				wsaData;
    SOCKET				listenSocket, clientSocket;
	struct sockaddr_in	localAddress, clientAddress;
	int					iAddressSize;

	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        printf("Failed to load Winsock!\n");
        return 1;
    }

	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (listenSocket == SOCKET_ERROR) {
		printf("socket() failed: %d\n", WSAGetLastError());
		return 1;
	}

	localAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddress.sin_family = AF_INET;
    localAddress.sin_port = htons(DEFAULT_PORT);

	if (bind(listenSocket, (struct sockaddr *)&localAddress, sizeof(localAddress)) == SOCKET_ERROR) {
        printf("bind() failed: %d\n", WSAGetLastError());
        return 1;
    }

    listen(listenSocket, 8);
	cout << "Server Initialized. Waiting for clients.\n";

	TIMEVAL tv = { 0 };
	fd_set readSet;
	
	int returnValue;

	while (running) {
		tv.tv_usec = 2000000; // microseconds
        FD_ZERO(&readSet);
		FD_SET(listenSocket, &readSet);
		returnValue = select( 0, &readSet, NULL, NULL, &tv );

		if (returnValue == SOCKET_ERROR) {
			cout << "Error listening to clients\n";
			break;
		}
		else if (returnValue) {
			iAddressSize = sizeof(clientAddress);
			clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &iAddressSize);
			if (clientSocket == INVALID_SOCKET) {
				printf("accept() failed: %d\n", WSAGetLastError());
				break;
			}

			HostThread::start(clientSocket);
			printf("Accepted client: %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
		}
		else {
			//cout << "timeOut\n";
		}
	}

	return 0;
}