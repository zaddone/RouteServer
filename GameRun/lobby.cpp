#include "stdafx.h"
#include "lobby.h"
#include "common.h"
Lobby::Lobby(const TCHAR * Base,CallBackSendData _f , LPVOID lp,TCHAR * _gName,char k ):GameBase(){

	KeyID = k;
	GName = _gName;
	this->SetBaseIni(Base);
	this->SetCallBack(_f,lp); 
	//wprintf(L"%s %s %d\r\n",GName,this->gameName,this->KeyID);
	filter_pro = new TCHAR[50];
	//char Process[50];
	GetPrivateProfileString(_T("Process"),_T("filter"),_T("tkcltnet.exetkmc.exetklobby.exe"),filter_pro,50,Base);
	
	_start = new TCHAR[1024];
	GetPrivateProfileString(_T("Process"),_T("start"),_T("C:\\Program Files\\JJGame\\TKLobby.exe"),_start,1024,Base);


}
Lobby::~Lobby() {
	//GameBase:~GameBase();
	delete [] filter_pro;
	delete [] _start;
}
void Lobby::start(){
	ShellExecute(NULL, _T("open") , _start, _T(""),_T(""),SW_SHOW );
	if (checkStart()) return;
	reloadWindows();
}
void Lobby::reloadWindows(){
	CloseWindows();
	//if (change)	this->Func(this->lpParamter,"-1",reloadServer,this);
	//else start();
	start();
}
void reloadServer(LPVOID handle,char * data,bool * wait){
	Lobby * lo = (Lobby * )handle;
	lo->reloadWindows();
	//lo->start();
	wait=false;
}
void Lobby::CloseWindows(){
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
	//if (this->ShowWindows()){
	FindEnumProcessAll(KillWindowsExe,this->filter_pro);

		//this->Func(this->lpParamter,"-1"
		//CallBackSendData
	//}
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
	//this->BlockList[1]->Coord[0]->rect.width;
	Coordinate coor = this->BlockList[1]->Coord[0];
	//vector <TempleImg*> Templist = this->BlockList[1]->GetTempleList();	
	/**
	cvSetImageROI(ScreenImg, coor.rect);
	IplImage * dst=cvCreateImage(cvSize(coor.rect.width,coor.rect.height),ScreenImg->depth,ScreenImg->nChannels);
	cvCopy(ScreenImg,dst);
	cvResetImageROI(ScreenImg);

	cvNamedWindow("contour1");
	cvShowImage("contour1", dst);  
	cvWaitKey(0);
	cvDestroyWindow("contour1");
	**/
	this->BlockList[0]->rect.x = coor.rect.x+ coor.rect.width;
	this->BlockList[0]->rect.y = coor.rect.y;
	int num = this->BlockList[0]->FindNL(ScreenImg);

	printf("NL num %d\r\n",num);
	

	return true;
}