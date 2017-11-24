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
	this->KeyID = 0;
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
		char d[2]={KeyID,0};
		this->Func(this->lpParamter,d,2,goGame,this);
		//Sleep(this->heartTime);
	}
	//reloadWindows();
	return true;
}

void Lobby::ScreenSave(){
	if (!::IsClipboardFormatAvailable(CF_BITMAP)){
		//printf("IsClipboardFormatAvailable\n"); CF_TEXT
		return;
	}
	//HBITMAP H;
	if (::OpenClipboard (NULL)) {
		//H = (HBITMAP)::GetClipboardData(CF_BITMAP);
		//if (this->ScreenImg == NULL) ScreenImg =  hBitmap2Ipl(H);
		//else hBitmap2Ipls(H,ScreenImg );

		hBitmap2Ipls(Screen(),this->ScreenImg );
		char file[1024]={0};
		sprintf(file,"coll\\%d.bmp\0",clock());
		cvSaveImage(file,ScreenImg);
		printf("Screen Save to %s\r\n",file);
		//delete [] file;
		::EmptyClipboard();
		::CloseClipboard ();
	}
	return;

}

void Lobby::trainBlock(CallBackTask _f){
	for (vector<Block*>::iterator it = BlockList.begin(); it != BlockList.end(); it ++) {
		(*it)->ErgodicBlock(_f);
	}
	for (vector<GameBase *>::iterator it = GameList.begin(); it != GameList.end(); it ++) {
		(*it)->trainBlock(_f);
	}

}

void Lobby::trainBlockNL(){

	for (vector<Block*>::iterator it = BlockList.begin(); it != BlockList.end(); it ++) {
		(*it)->ErgodicBlock(trainNL);
	}
	for (vector<GameBase *>::iterator it = GameList.begin(); it != GameList.end(); it ++) {
		(*it)->trainBlockNL();
	}

}
void Lobby::start(){
	ShellExecute(NULL, _T("open") , _start, _T(""),_T(""),SW_SHOW );
	if (checkStart()) return;
	reloadWindows();
}
void Lobby::reloadWindows(){
	CloseWindows();
	start();
}
void reloadServer(LPVOID handle,char * data,int len ){

	Lobby * lo = (Lobby * )handle;
	lo->CloseWindows();

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
	size_t len = sizeof(num);
	PushData * da = new PushData();
	da->add(KeyID);
	da->add(char(1));
	da->add(num);
	this->Func(this->lpParamter,da->GetData(),da->GetDataLen(),NULL,NULL);
	delete da;
	this->IsGo = true;
	return true;
}
void goGame(LPVOID handle,char * data ,int len){

	if (len<2)return;
	Lobby *m = (Lobby*)handle;
	for (vector<GameBase *>::iterator it = m->GameList.begin(); it != m->GameList.end(); it ++) {
		if ((*it)->KeyID == data[0]){
			//printf("%d\r\n",data[0]);
			if (!(*it)->go(int(data[1]))){
				(*it)->Par->CloseWindows();
			}
			break;
		}
	}

}