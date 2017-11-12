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
bool Scmj::checkRoom(){
	for (int i = 0;i<120;i++){
		
		if (!this->Par->ShowWindows())return false;
		hBitmap2Ipls(Screen(),this->ScreenImg);
		int n =this->BlockList[10]->FindOne(this->ScreenImg);
		if (n!= -1)return true;
		this->InRoom();
		Sleep(this->heartTime*2);
	}
	return false;
}
bool Scmj::inRoom(int roomID){
	if (!checkRoom()) return false;

	int n = this->BlockList[12]->FindArr(this->ScreenImg);
	if (n!=4){
		printf("find not Room %d\r\n",n);
		return false;
	}
	Block * bl = this->BlockList[12]->GetChildOne(ScreenImg,roomID);
	if (bl == NULL){
		printf("find not Room %d \r\n",roomID);
		return false;
	}
	return bl->ClickCoordinate(-1,1);
}
bool Scmj::inGame(){

	//bool isBreak;
	int j,i,n;
	for(i=0;i<60;i++){
		Sleep(this->heartTime*2);
		hBitmap2Ipls(Screen(),this->ScreenImg);
		n = this->BlockList[13]->FindOne(ScreenImg);
		if (n != -1){
			this->BlockList[13]->ClickCoordinate(-1,1);
			//isBreak = false;
			for(j=0;j<20;j++){
				Sleep(this->heartTime*2);
				if (this->ShowWindows()){
					return true;
				}
			}
			//if (isBreak) break;			
		}else if (!this->Par->CheckWindows())return false;
	}
	return false;
}
bool Scmj::go(int roomID){
	
	//if (!checkRoom())return false;
	if (!this->ShowWindows()){
	if (! inRoom(roomID)) return false;
	if (!inGame()) return false;
	}
	/**
	//in game
	int n;
	do{
		hBitmap2Ipls(Screen(),this->ScreenImg);
		n = this->BlockList[4]->FindOne(ScreenImg);
		if (n != -1){
			char data[10]={0};
			sprintf(data,"%d\0",n);
			this->Func(this->lpParamter,data,NULL,NULL);
		}
		printf("n-> %d\r\n",n);
		Sleep(this->heartTime);
	}while(true);
	**/
	return game();
}
bool Scmj::game(){
	IsGo = true;
	int n;
	do{
		hBitmap2Ipls(Screen(),this->ScreenImg);
		//n = this->BlockList[4]->FindArr(ScreenImg);
		n = this->BlockList[4]->FindOne(ScreenImg);
		//for (vector<Block *>::iterator it = this->BlockList[4]->Child.begin(); it != this->BlockList[4]->Child.end(); it ++) {
		//}
		//for (this->BlockList[4]->Child.back())
		if (n != -1){
			char data[10]={0};
			sprintf(data,"%d\0",n);
			this->Func(this->lpParamter,data,NULL,NULL);
			//LoadSendData(this->Client,"0",goGame,this);
		}
		printf("n-> %d\r\n",n);
		//if (n==0)

		Sleep(this->heartTime);
	}while(ShowWindows());
	return IsGo;
}