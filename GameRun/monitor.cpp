#include "stdafx.h"
#include "monitor.h"
#include "scmj.h"

Monitor::Monitor(const TCHAR * Base){

	TCHAR Tip[16]={0};
	char * ip = new char[16];
	GetPrivateProfileString(_T("Server"),_T("ip"),_T("192.168.1.50"),Tip ,16,Base);
	int len = WideCharToMultiByte(CP_ACP,0,Tip,-1,NULL,0,NULL,NULL);  
	WideCharToMultiByte(CP_ACP,0,Tip,-1,ip,len,NULL,NULL ); 

	//delete Tip;
	//printf("%s\r\n",ip);

	
	int port = GetPrivateProfileInt(_T("Server"),_T("port"),3333,Base);
	int heartTime = GetPrivateProfileInt(_T("Server"),_T("heartTime"),100,Base);

	//TCHAR filter_pro[50]={0};
	//char Process[50];
	//GetPrivateProfileString(_T("Process"),_T("filter"),_T("tkcltnet.exetkmc.exetklobby.exe"),filter_pro,50,Base);

	this->Client = new SocketBuffer(ip,port,heartTime);

	//start = new TCHAR[1024];
	//GetPrivateProfileString(_T("Process"),_T("start"),_T("C:\\Program Files\\JJGame\\TKLobby.exe"),start,1024,Base);
	//debug = GetPrivateProfileInt(_T("Process"),_T("debug"),5,Base);

	startLobby = new Lobby(Base,LoadSendData,this->Client);
	Client->SetReloadSystem(reloadServer,startLobby);
	GameList = new GameBase *[1];
	GameList[0] = new Scmj(Base,LoadSendData,this->Client);
	GameList[0]->Par = startLobby;
	GameListCount=1;
	//this->scmjGame = new Scmj(Base,LoadSendData,this->Client);
	
}
Monitor::~Monitor(){
	
	isRun=false;
	for (int i = 0;i<GameListCount;i++){
		delete GameList[i];
	}
	//delete scmjGame;
	delete Client;
	//delete [] start;
	delete startLobby;

}


void Monitor::run(){
	//char db[1]={}

	//ShellExecute(NULL, _T("open") , start, _T(""),_T(""),SW_SHOW );
	startLobby->start();
	//startLobby->checkStart();
	
	
	isRun=true;
	int n;
	while(isRun){
		n = GameList[0]->go(0);
		if (n == -1) {
			startLobby->reloadWindows();
			continue;
		}
		//startLobby->ShowWindows();
		//Sleep(1000);
		//GameList[0]->go(0);
		//LoadSendData(this->Client,"0",goGame,this);
	}
	
}
void goGame(LPVOID handle,char * data,bool * wait){
	//data[0]
	Monitor *m = (Monitor*)handle;

	for (int i = 0;i<m->GameListCount;i++){
		if (m->GameList[i]->KeyID == data[0]){
			m->GameList[i]->go(int(data[1]));
			break;
		}
	}
	

	if (wait!=NULL){
		wait[0] = false;
	}

}