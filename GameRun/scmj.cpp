#include "stdafx.h"
//#include <windows.h> 
#include "scmj.h"
//#include <fstream>
//using namespace std;

Scmj::Scmj(const TCHAR * Base,CallBackSendData _f , LPVOID lp,TCHAR * _gName,char k ){

	KeyID = k;
	GName = _gName;
	this->SetBaseIni(Base);
	this->SetCallBack(_f,lp); 
	wprintf(L"%s %s %d\r\n",GName,this->gameName,this->KeyID);
}

int Scmj::go(int roomID){
	//printf("inRoom %d \r\n",roomID);
	//this->Par->ShowWindows();
	hBitmap2Ipls(Screen(),this->ScreenImg);
	int n =this->BlockList[10]->FindOne(this->ScreenImg);
	int i;
	for(i=0;i<120;i++){
		if (!this->Par->ShowWindows()) return -1;
		this->InRoom();
		Sleep(this->heartTime*2);
		hBitmap2Ipls(Screen(),this->ScreenImg);
		n = this->BlockList[10]->FindOne(this->ScreenImg);
		if (n != -1)break;
	}
	if (i ==  120) return -1;

	n = this->BlockList[12]->FindArr(this->ScreenImg);
	if (n!=4){
		printf("find not Room \r\n");
		return -1;
	}
	Block * bl = this->BlockList[12]->GetChildOne(ScreenImg,roomID);
	if (bl == NULL){
		printf("find not Room %d \r\n",roomID);
		return -1;
	}
	bl->ClickCoordinate(-1,1);
	Sleep(this->heartTime*2);
	hBitmap2Ipls(Screen(),this->ScreenImg);
	n = this->BlockList[13]->FindOne(ScreenImg);
	bool isBreak;
	int j;
	for(i=0;i<60;i++){
		Sleep(this->heartTime*2);
		hBitmap2Ipls(Screen(),this->ScreenImg);
		n = this->BlockList[13]->FindOne(ScreenImg);
		if (n != -1){
			this->BlockList[13]->ClickCoordinate(-1,1);
			isBreak = false;
			for(j=0;j<20;j++){
				Sleep(this->heartTime*2);
				if (this->ShowWindows()){
					isBreak = true;
					break;
				}
			}
			if (isBreak) break;			
		}
	}
	if (i ==  60) return -1;


	return 0;
}