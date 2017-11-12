#include "stdafx.h"
#include "lobby.h"
#include "common.h"
#include "scmj.h"

Lobby::Lobby(const TCHAR * Base,CallBackSendData _f , LPVOID lp,TCHAR * _gName):GameBase(){

	GName = _gName;
	this->SetBaseIni(Base);
	filter_pro = new TCHAR[50];
	GetPrivateProfileString(_T("Process"),_T("filter"),_T("tkcltnet.exetkmc.exetklobby.exe"),filter_pro,50,Base);
	
	_start = new TCHAR[1024];
	GetPrivateProfileString(_T("Process"),_T("start"),_T("C:\\Program Files\\JJGame\\TKLobby.exe"),_start,1024,Base);
	

	this->SetCallBack(_f,lp);
	Scmj *Game1 = new Scmj(Base,_f,lp);
	Game1->Par = this;
	GameList.push_back(Game1);
	IsGo=true;

}
Lobby::~Lobby() {
	//GameBase:~GameBase();
	this->CloseWindows();
	for (vector<GameBase *>::iterator it = GameList.begin(); it != GameList.end(); it ++) {
		delete (*it);
	}
	delete [] filter_pro;
	delete [] _start;
	//delete Client;
}
bool Lobby::go(){

	while(IsGo){
		start();
		
		this->Func(this->lpParamter,"0",goGame,this);
		//Sleep(this->heartTime);
	}
	//reloadWindows();
	return true;
}
void Lobby::start(){
	ShellExecute(NULL, _T("open") , _start, _T(""),_T(""),SW_SHOW );
	if (checkStart()) return;
	reloadWindows();
}
void Lobby::reloadWindows(){
	CloseWindows();
	//printf("begin %d\r\n",clock());
	
	//printf("end %d\r\n",clock());
	start();
}
void reloadServer(LPVOID handle,char * data ){
	printf("reloadServer\r\n");
	Lobby * lo = (Lobby * )handle;
	//lo->reloadWindows();
	lo->CloseWindows();
	//if (wait != NULL)wait[0] = false;

}
void Lobby::CloseWindows(){
	for (vector<GameBase *>::iterator it = GameList.begin(); it != GameList.end(); it ++) {
		(*it)->CloseWindows();
	}
	if (this->ShowWindows()){
		keybd_event(VK_MENU,0,0,0);
		keybd_event(VK_F4,0,0,0);
		Sleep(100);
		keybd_event(VK_F4,0,KEYEVENTF_KEYUP,0); 
		keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0); 
		int i;
		for (i = 0;i<60;i++){
			Sleep(this->heartTime);
			if (!this->ShowWindows())break;
			keybd_event(VK_MENU,0,0,0);
			keybd_event(VK_F4,0,0,0);
			Sleep(100);
			keybd_event(VK_F4,0,KEYEVENTF_KEYUP,0); 
			keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0); 
		}
	}
	FindEnumProcessAll(KillWindowsExe,this->filter_pro);
	Sleep(this->heartTime*5);
}
bool Lobby::checkStart(){
	int n = -1;
	int i;
	for(i = 0;i<120;i++){
		Sleep(heartTime);
		hBitmap2Ipls(Screen(),this->ScreenImg);
		n = this->BlockList[1]->FindOne(this->ScreenImg);
		if (n != -1) break;
		//printf("%d \r\n",clock());
	}
	if (i == 120){
		return false;
	}

	Coordinate coor = this->BlockList[1]->Coord[0];
	this->BlockList[0]->rect.x = coor.rect.x+ coor.rect.width;
	this->BlockList[0]->rect.y = coor.rect.y;
	int num = this->BlockList[0]->FindNL(ScreenImg);

	printf("NL num %d\r\n",num);
	char data[10] = {0};
	sprintf(data,"-3 %d",num);

	this->Func(this->lpParamter,data,NULL,NULL);
	this->IsGo = true;
	return true;
}
void goGame(LPVOID handle,char * data ){
	//data[0]
	//printf("%s\r\n",data);
	//Sleep(5000);
	//return;
	Lobby *m = (Lobby*)handle;
	//bool isOut;
	for (vector<GameBase *>::iterator it = m->GameList.begin(); it != m->GameList.end(); it ++) {
		if ((*it)->KeyID == data[0]){
			//isOut=false;
			//while(!isOut){
			//m->IsGo = (*it)->go(int(data[1]));
			if (!(*it)->go(int(data[1]))){
				(*it)->Par->CloseWindows();
			}
			//  Sleep(m->heartTime);
			//}
			break;
		}
	}
//	if (wait != NULL)wait[0] = false;

}