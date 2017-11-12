// GameRun.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SendBuffer.h"
#include "lobby.h"


int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR * base = _T(".\\base.ini");
	SocketBuffer * Client = new SocketBuffer(base);
	Lobby *lo  =new Lobby(base,LoadSendData,Client);
	Client->SetReloadSystem(reloadServer,lo);
	Client->start();
	lo->go();
	return 0;
}

