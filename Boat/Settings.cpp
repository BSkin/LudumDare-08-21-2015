#include "Settings.h"

int Settings::windowWidth = 1;
int Settings::windowHeight = 1;
int Settings::windowState = WINDOWED;
int Settings::maxFPS = 1000;
string Settings::username = "Peasant";
int Settings::clientID = -1;
HANDLE Settings::clientIDMutex = CreateMutex(NULL, FALSE, NULL);
HANDLE Settings::usernameMutex = CreateMutex(NULL, FALSE, NULL);

void Settings::cleanup()
{
	if (clientIDMutex != NULL) 
		CloseHandle(clientIDMutex);
	if (usernameMutex != NULL) 
		CloseHandle(usernameMutex);
}

void Settings::setClientID(int x)
{
	aquireClientIDMutex();
		clientID = x;
	releaseClientIDMutex();
}

int Settings::getClientID()
{
	int temp;
	aquireClientIDMutex();
		temp = clientID;
	releaseClientIDMutex();
	return temp;
}

void Settings::setUsername(string x)
{
	aquireUsernameMutex();
		username = x;
	releaseUsernameMutex();
}

string Settings::getUsername()
{
	string temp;
	aquireUsernameMutex();
		temp = username;
	releaseUsernameMutex();
	return temp;
}

int Settings::aquireClientIDMutex()
{
	DWORD waitResult = WaitForSingleObject(clientIDMutex, INFINITE);
	if (waitResult != WAIT_OBJECT_0) {
		printf("Error aquiring clientID mutex.\n");
		return -1;
	}
	return 0;
}

int Settings::releaseClientIDMutex()
{
	if (!ReleaseMutex(clientIDMutex)) {
		printf("Error releasing clientID mutex.\n");
		return -1;
	}
	return 0;
}

int Settings::aquireUsernameMutex()
{
	DWORD waitResult = WaitForSingleObject(usernameMutex, INFINITE);
	if (waitResult != WAIT_OBJECT_0) {
		printf("Error aquiring clientID mutex.\n");
		return -1;
	}
	return 0;
}

int Settings::releaseUsernameMutex()
{
	if (!ReleaseMutex(usernameMutex)) {
		printf("Error releasing clientID mutex.\n");
		return -1;
	}
	return 0;
}