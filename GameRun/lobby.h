#ifndef Lobby_H
#define Lobby_H

//#include "common.h"
#include "GameBase.h"
class Lobby  : public GameBase {
public:
	Lobby(const TCHAR * Base ,CallBackSendData _f=NULL , LPVOID lp =NULL,TCHAR * _gName = L"lobby" );
	~Lobby();
	friend void goGame(LPVOID handle,char * data ,int len);
	vector <GameBase*> GameList;
	virtual void CloseWindows();
	virtual bool go();
	virtual void reloadWindows();
	virtual void trainBlockNL();
	virtual void trainBlock(CallBackTask _f);

	friend void reloadServer(LPVOID handle,char * data,int len );
	void ScreenSave();
	
private:	
	
	//SocketBuffer * Client;
	TCHAR *filter_pro;
	TCHAR *_start;
	bool checkStart();
	
	
	void start();
	

};

#endif