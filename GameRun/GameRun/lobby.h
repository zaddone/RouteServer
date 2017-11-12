#ifndef Lobby_H
#define Lobby_H

//#include "common.h"
#include "GameBase.h"
class Lobby  : public GameBase {
public:
	Lobby(const TCHAR * Base ,CallBackSendData _f=NULL , LPVOID lp =NULL,TCHAR * _gName = L"lobby" );
	~Lobby();
	friend void goGame(LPVOID handle,char * data );
	vector <GameBase*> GameList;
	virtual void CloseWindows();
	virtual bool go();
	virtual void reloadWindows();
	friend void reloadServer(LPVOID handle,char * data );
private:	
	
	//SocketBuffer * Client;
	TCHAR *filter_pro;
	TCHAR *_start;
	bool checkStart();
	
	
	void start();
	

};

#endif