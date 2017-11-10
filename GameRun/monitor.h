#ifndef Monitor_H_
#define Monitor_H_

#include "SendBuffer.h"
#include "common.h"
#include "GameBase.h"
#include "lobby.h"

void reloadSystem(LPVOID handle,char * data ,bool * wait);

class Monitor
{
public:
	Monitor(const TCHAR * Base);
	~Monitor();
	void run();

private:
	
	SocketBuffer * Client;
	GameBase **  GameList;
	int GameListCount;
	bool isRun;
	friend void goGame(LPVOID handle,char * data,bool * wait);
	//TCHAR * start;
	//int debug;
	Lobby * startLobby;
	//GameBase * NowGame;
};
#endif