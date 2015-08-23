#ifndef SETTINGS_H
#define SETTINGS_H

#include <WinSock2.h>
#include <Windows.h>
#include <string>
using std::string;

#define FULLSCREEN	0
#define WINDOWED	1
#define BORDERLESS	2

#define DEFAULT_PORT	5150
#define MAX_PACKET_SIZE 4096 //Bytes
#define NETWORK_UPDATE_RATE 60

class Settings
{
public:
	static void cleanup();
	static void setWindowWidth(int x) { windowWidth = x; }
	static void setWindowHeight(int x) { windowHeight = x; }
	static void setWindowState(int x) { windowState = x; }
	static void setMaxFPS(int x) { maxFPS = x; }
	static void setMaxParticles(int x) { maxParticles = x; }
	static void setUsername(string x);
	static void setClientID(int x);

	static int getWindowWidth() { return windowWidth; }
	static int getWindowHeight() { return windowHeight; }
	static float getAspectRatio() { return (float)windowHeight / (float)windowWidth; }
	static int getWindowState() { return windowState; }
	static int getMaxFPS() { return maxFPS; }
	static int getMaxParticles() { return maxParticles; }
	static string getUsername();
	static int getClientID();
private:
	static int aquireClientIDMutex();
	static int releaseClientIDMutex();
	static int aquireUsernameMutex();
	static int releaseUsernameMutex();

	static int windowWidth;
	static int windowHeight;
	static int windowState;
	static int maxFPS;
	static int maxParticles;
	static string username;
	static HANDLE usernameMutex;
	static int clientID;
	static HANDLE clientIDMutex;
};

#endif