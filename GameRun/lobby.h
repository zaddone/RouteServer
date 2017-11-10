#ifndef Lobby_H
#define Lobby_H
#include "GameBase.h"
class Lobby  : public GameBase {
public:
	Lobby(const TCHAR * Base,CallBackSendData _f=NULL , LPVOID lp =NULL ,TCHAR * _gName = L"lobby",char k = 0);
	~Lobby();
	bool checkStart();
	virtual void CloseWindows();
	void reloadWindows();
	TCHAR *filter_pro;
	TCHAR *_start;
	void start();
	friend void reloadServer(LPVOID handle,char * data,bool * wait);

private:	

};

#endif