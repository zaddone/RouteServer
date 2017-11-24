#include "stdafx.h"
//#include <windows.h> 
#include "common.h"
#include "scmj.h"

//#include <fstream>
//using namespace std;

Scmj::Scmj(const TCHAR * Base,CallBackSendData _f , LPVOID lp,TCHAR * _gName,char k ){

	KeyID = k;
	GName = _gName;
	this->SetBaseIni(Base);
	this->SetCallBack(_f,lp); 
	wprintf(L"%s %s %d\r\n",GName,this->gameName,this->KeyID);
	SelfPanLen = 14;
	SelfPan = NULL;// new char [SelfPanLen];
	//SelfNum = 0;
	SelfPanLenNow = SelfPanLen;
}
Scmj::~Scmj(){
	if (SelfPan != NULL) delete [] SelfPan;
}

void Scmj::CloseWindows(){
	if (!this->ShowWindows()) return;
	keybd_event(VK_MENU,0,0,0);
	keybd_event(VK_F4,0,0,0);
	Sleep(100);
	keybd_event(VK_F4,0,KEYEVENTF_KEYUP,0); 
	keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0);
	for (int i=0;i<120;i++){
		Sleep(heartTime*4);
		hBitmap2Ipls(Screen(),ScreenImg);
		if (-1 != this->BlockList[14]->FindOne(this->ScreenImg)){
			this->BlockList[14]->ClickCoordinate(-1,1);
			for (int _i=0;_i<120;_i++){
				Sleep(heartTime*4);
				hBitmap2Ipls(Screen(),ScreenImg);
				if (!FindEnumProcess(_CheckWindows,this->gameName)) return;
				if (-1 != this->BlockList[14]->FindOne(this->ScreenImg)){
					this->BlockList[14]->ClickCoordinate(-1,1);
				}
			}
			break;
		}
		keybd_event(VK_MENU,0,0,0);
		keybd_event(VK_F4,0,0,0);
		Sleep(100);
		keybd_event(VK_F4,0,KEYEVENTF_KEYUP,0); 
		keybd_event(VK_MENU,0,KEYEVENTF_KEYUP,0);
	}
	
	FindEnumProcess(KillWindowsExe,this->gameName);
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
	int i = 0;
	for(;i<60;i++){
		Sleep(this->heartTime*2);
		int n = this->BlockList[12]->FindArr(this->ScreenImg);
		if (n!=4){
			printf("find not Room %d\r\n",n);
			continue;
		}
		Block * bl = this->BlockList[12]->GetChildOne(ScreenImg,roomID);
		if (bl == NULL){
			printf("find not Room %d \r\n",roomID);
			continue;
		}
		bl->Coord.back().MouseClick(1);
		break;
	}
	if (i == 60) return false;
	//printf("click \r\n");
	//return bl->ClickCoordinate(-1,1);
	return true;
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

	if (!this->ShowWindows()){
		if (! inRoom(roomID)) return true;
		if (!inGame()) return false;
	}
	return game();
}

bool Scmj::game(){
	IsGo = true;
	int lastn = -1;
	int lastN = -1;
	int n = -1;
	int sumN = 0 ;
	//char data[1024] = {0};
	//int lastNVal = -1;
	do{
		Sleep(this->heartTime);
		hBitmap2Ipls(Screen(),this->ScreenImg);
		//n = this->BlockList[4]->FindArr(ScreenImg);
		

		n = this->BlockList[4]->FindOne(ScreenImg);	
		//char data[3]={this->KeyID,11,n};
		//this->Func(this->lpParamter,data,3,NULL,NULL);

		if (n == -1){
			sumN++;
			if (sumN<3)continue;
			if (sumN>600){
				IsGo=false;
				break;
			}			
			//continue;
			if (-1 != this->BlockList[8]->Child[6]->FindOne(this->ScreenImg)){
				//lastn = -1;
				lastN = -1;
				this->SelfPanLenNow = this->SelfPanLen;
				this->BlockList[8]->Child[6]->ClickCoordinate(-1,1);
				continue;
			}else if (-1 != this->BlockList[8]->Child[7]->FindOne(this->ScreenImg)){		
				//lastn = -1;
				this->SelfPanLenNow = this->SelfPanLen;
				if (sumN>30){
					this->CloseWindows();
					//IsGo=false;
					break;
				}

				lastN = -1;
				this->BlockList[8]->Child[7]->ClickCoordinate(-1,1);
				continue;
			}
		}else{
			sumN=0;
			if (n == 3){				 
				ScreenSelf(lastN,lastn);
			}else{
				if (n == 4){		
					continue;			 
				}else{
					if (lastN == -1) continue;
					ScreenOther(n,lastn,lastN);
				}
			}
		}
		
	}while(this->ShowWindows());
	return IsGo;
}

void Scmj::ScreenOther(int n,int & lastn,int & lastN){
	
	bool isOut = false;
	int Last = lastn;
	if (lastn <4){
		if (lastn != n ){
			isOut = true;
			lastn = n;
		}
	}else{
			
		if (lastn>>8 == n){
			return;
		}else{
			lastn = n;
		}
		isOut = true;
		
	}
	int valN = this->BlockList[n]->FindOne(this->ScreenImg);

	if (valN != -1){
		lastn = n <<8 | valN;
	}else{
		if (!isOut)return;
	}
	//lastn = n;

	//int lastVal = this->BlockList[Last]->FindOne(this->ScreenImg);
	lastN = this->BlockList[9]->Child[1]->FindNL(ScreenImg);					
	int num = this->BlockList[n+5]->Child[0]->FindNL(ScreenImg);
	PushData * da = new PushData();
	da->add(KeyID);
	da->add(char(10));
	da->add(char(n));
	da->add(char(lastN));
	da->add(char(valN));
	//da->add(char(lastVal));	
	da->add(num);
	this->Func(this->lpParamter,da->GetData(),da->GetDataLen(),_ClickBack,this);	
	delete da;
	return; 
}
void Scmj::ScreenSelf(int & lastN,int & lastn){
	int N = BlockList[9]->Child[1]->FindNL(ScreenImg);
	//int Num=0;//BlockList[8]->Child[0]->FindNL(ScreenImg);
	if (N == 55 ){
		if (!this->Send_1()){
			if (!this->Send_2()){					
				if (!Send_4_1()){
					this->Send_5();
				}
			}
		}
	}else{
		if (!this->Send_4_1()){
			this->Send_5();	
		}
	}		
	lastN = N;
	lastn = 3;
}
void Scmj::GetSelfPan(int & OutLen){	
	/**
	//int len = SelfPanLen-1;
	Block * bls = this->BlockList[8]->Child[12];
	if (bls->Child.empty()){
		
		CvRect r=  bls->rect;
		int sep =r.width/13;
		for (int i=0;i<13;i++){
			Block *bl = new Block(cvRect(r.x+sep*i,r.y,sep,r.height),bls);
			bls->Child.push_back(bl);
		}
		bls->Child.push_back(this->BlockList[8]->Child[13]);
	}

	if (SelfPan == NULL) SelfPan = new char[SelfPanLen];
	int i=0;
	bls->Coord.clear();
	for (vector<Block *>::iterator it = bls->Child.begin(); it != bls->Child.end(); it ++) {
		this->SelfPan[i]=(*it)->FindOne(ScreenImg);
		if (this->SelfPan[i] != -1 )bls->Coord.push_back((*it)->Coord.back());
		i++;
	}
	OutLen= SelfPanLen;	
	**/
	if (SelfPan == NULL) {
		SelfPan = new char[SelfPanLen];
		SelfPanLenNow = SelfPanLen;
	}
	OutLen = SelfPanLenNow -1;
	char * val =  this->BlockList[8]->Child[12]->FindArr(this->ScreenImg,OutLen);
	memcpy(SelfPan,val,OutLen);
	SelfPan[OutLen] = this->BlockList[8]->Child[13]->FindOne(ScreenImg);
	if (SelfPan[OutLen] != -1) {
		this->BlockList[8]->Child[12]->Coord.push_back( this->BlockList[8]->Child[13]->Coord.back());
		OutLen++;
	}
	
	for (int i = OutLen;i<SelfPanLen;i++){
		SelfPan[i] = -1;
	}

	delete [] val;
	
}
int Scmj::GetSelfNum(){
	Block *bl = BlockList[8]->Child[0];
	NumberList * _NL = bl->GetNumblerListClass();


	
	if (bl->Nothing == NULL ){
		bl->Nothing = new TempleImg(ScreenImg, bl->rect,0 );
		IplImage *src = cvCreateImage(cvSize(bl->rect.width,bl->rect.height), ScreenImg->depth,ScreenImg->nChannels);		
		cvThreshold( bl->Nothing->img,src,_NL->Sep , 255, CV_THRESH_BINARY  );

		vector <CvRect> templeRect;
		CvSeq* contours = NULL;
		CvMemStorage* storage = cvCreateMemStorage(0);		
		int count = cvFindContours(src, storage, &contours,sizeof(CvContour),CV_RETR_EXTERNAL); 
		for (CvSeq* c = contours; c != NULL; c = c->h_next) {  
			templeRect.push_back(cvBoundingRect(c,0));
		}
		sort(templeRect.begin(),templeRect.end(),compRectNum);
		int len_x = -1;

		for (vector<CvRect>::reverse_iterator it = templeRect.rbegin(); it != templeRect.rend(); ++ it ){	
			//(*it).
			if (len_x == -1)len_x = (*it).x;
			else{
				if ( (len_x - (*it).x+(*it).width) <10)len_x = (*it).x;
				else{
					//int w = bl->rect.width + bl->rect.x;
					bl->rect.width = bl->rect.width + bl->rect.x - len_x;
					bl->rect.x = len_x;
					break;
				}
			}
		}
		templeRect.clear();
		cvReleaseMemStorage(&storage);
		
		cvReleaseImage(&src);
		delete bl->Nothing;
		bl->Nothing=NULL;
		/**
		bl->Nothing = new TempleImg(ScreenImg, bl->rect,0 );
		bl->Nothing->ShowImg();
		delete bl->Nothing;
		**/
	}
	return bl->FindNL(ScreenImg);
	//return SelfNum;
}
bool Scmj::Send_1(){

	if (-1 == this->BlockList[8]->Child[1]->FindOne(this->ScreenImg)) return false;
	//int num = BlockList[8]->Child[0]->FindNL(ScreenImg);
	int count;
	this->SelfPanLenNow = this->SelfPanLen;
	//char * keyVal = NULL;
	GetSelfPan(count);
	if (count < 13){
		//delete keyVal;
		/**
		int i=0;
		for(;i<10;i++) {
			printf("find self pan %d\r\n",i);
			Sleep(this->heartTime*2);
			hBitmap2Ipls(Screen(),this->ScreenImg);
			GetSelfPan(count);
			if (count < 12 )break;
		}
		**/
		this->BlockList[8]->Child[14]->ClickCoordinate(-1,2);
		return true;
	}

	PushData * da = new PushData();
	da->add(this->KeyID);
	da->add(char(1));
	da->add(SelfPan,SelfPanLen);
	da->add(GetSelfNum());
	this->Func(this->lpParamter,da->GetData(),da->GetDataLen(),_ClickSend_1,this);	
	delete da;
	//delete [] keyVal;
	return true;

}
void _ClickBack(LPVOID handle,char * data ,int len){
	//Scmj * sc = (Scmj *)handle;
	//sc->
}
void _ClickSend_1(LPVOID handle,char * data ,int len){
	Scmj * sc = (Scmj *)handle;
	if (len>0){
		sc->BlockList[8]->Child[12]->ClickCoordinate(int(data[0]),1,true);		
		for(int i=1;i<len;i++){
			Sleep(sc->heartTime);
			hBitmap2Ipls(Screen(),sc->ScreenImg);
			sc->BlockList[8]->Child[12]->ClickCoordinateCheck(int(data[i]),1,sc->ScreenImg);
		}
		Sleep(sc->heartTime);
		sc->BlockList[8]->Child[1]->ClickCoordinate(-1,1);
	}else{
		sc->BlockList[8]->Child[14]->ClickCoordinate(-1,2);
	}
}
bool Scmj::Send_2(){

	if (-1 == this->BlockList[8]->Child[2]->FindOne(this->ScreenImg)){
		return false;
	}
	this->SelfPanLenNow = this->SelfPanLen;
	int count = SelfPanLen;
	//char * keyVal = NULL;
	GetSelfPan(count);
	if (count < 13){
		//delete keyVal;
		return true;		 
	}

	PushData * da = new PushData();
	da->add(this->KeyID);
	da->add(char(2));
	da->add(SelfPan,SelfPanLen);
	da->add(char(this->BlockList[9]->Child[0]->FindNL(ScreenImg)));
	this->Func(this->lpParamter,da->GetData(),da->GetDataLen(),_ClickSend_2,this);	
	delete da;
	//delete [] keyVal;
	return true;
}
void _ClickSend_2(LPVOID handle,char * data ,int len){
	Scmj * sc = (Scmj *)handle;
	if (len>0){
		sc->BlockList[8]->Child[data[0]+2]->ClickCoordinate(-1,1);
		if (len>1){
			//int num = sc->BlockList[8]->Child[0]->FindNL(sc->ScreenImg);
			PushData * da = new PushData();
			da->add(sc->KeyID);
			da->add(char(9));
			da->add(char(3));
			da->add(sc->GetSelfNum());
			sc->Func(sc->lpParamter,da->GetData(),da->GetDataLen(),NULL,NULL);	
			delete da;
		}
	}
}

bool Scmj::Send_4(int la){
	int n = this->BlockList[8]->Child[13]->FindOne(this->ScreenImg);	
	if (-1 == n){
		return false;
	}
	//return Send_4_1();
	if (SelfPan == NULL || la == -1) return Send_4_1();

	for (int i=0;i<this->SelfPanLen;i++){
		if (this->SelfPan[i]==-1 ){
			this->SelfPan[i] = n;
			break;
		}
	}
	this->BlockList[8]->Child[12]->Coord.push_back(this->BlockList[8]->Child[13]->Coord.back());
	PushData * da = new PushData();
	da->add(KeyID);
	da->add(char(4));
	da->add(SelfPan,SelfPanLen);
	//da->add(char(n));
	da->add(GetSelfNum());
	this->Func(this->lpParamter,da->GetData(),da->GetDataLen(),_ClickSend_4,this);
	delete da;
	return true;
}
bool Scmj::Send_4_1(){	
	int n = this->BlockList[8]->Child[13]->FindOne(this->ScreenImg);	
	if (-1 == n){
		return false;
	}
	int count;
	//char *keyVal = NULL;
	this->GetSelfPan(count);
	//if (count%2 == 0) return false;

	PushData * da = new PushData();
	da->add(KeyID);
	da->add(char(6));
	da->add(SelfPan,SelfPanLen);
	da->add(GetSelfNum());
	this->Func(this->lpParamter,da->GetData(),da->GetDataLen(),_ClickSend_4,this);
	delete da;
	//delete [] keyVal;
	return true;
}
void _ClickSend_4(LPVOID handle,char * data ,int len){
	Scmj * sc = (Scmj *)handle;
	if (len==0){
		return;
	}else{
		if (data[0]<27){
			if (!sc->ClickSelfPan(data[0])){
				//sc->Send_4_1();
				delete [] sc->SelfPan;
				sc->SelfPan = NULL;
			}
		}else{
			int k = data[0]-30;
			if (k>=0){
				if (!sc->ClickSelfBotton(k)){
					//if (k != 3)	sc->Send_4_1();
					delete [] sc->SelfPan;
					sc->SelfPan = NULL;
				}
			}
		}

	}
}
void Scmj::Send_5(){

	Block * bls = this->BlockList[8]->Child[16];
	if (bls->Child.empty()){
		bls->Child.push_back(this->BlockList[8]->Child[8]);
		bls->Child.push_back(this->BlockList[8]->Child[9]);
		bls->Child.push_back(this->BlockList[8]->Child[10]);
		bls->Child.push_back(this->BlockList[8]->Child[11]);
	}
	char * db = new char [4];
	int i= 0,OutLen = 0;
	for (vector<Block *>::iterator it = bls->Child.begin(); it != bls->Child.end(); it ++) {
		db[i] = (*it)->FindOne(ScreenImg);
		if (db[i]!= -1)OutLen++;
		i++;
	}
	if (OutLen == 0)return;

	PushData * da = new PushData();
	da->add(KeyID);
	da->add(char(5));
	da->add(db,i);
	da->add(GetSelfNum());
	this->Func(this->lpParamter,da->GetData(),da->GetDataLen(),_ClickSend_5,this);
	delete da;
	delete [] db;

}
void _ClickSend_5(LPVOID handle,char * data ,int len){
	
	Scmj * sc = (Scmj *)handle;
	if (len == 0) {
		Sleep(sc->heartTime*2);
		hBitmap2Ipls(Screen(),sc->ScreenImg);
		sc->Send_5();
		return;
	}

	sc->ClickSelfBotton(data[0]);
}
void Scmj::Send_3(){

	char *bak = new char [SelfPanLen];
	//char sendDB[13]={-1};
	memcpy(bak,this->SelfPan,this->SelfPanLen);
	int count,_i;

	this->GetSelfPan(count);

	for (int i=0;i<SelfPanLen;i++){
		if (bak[i]>26)continue;
		for (_i=0;_i<SelfPanLen;_i++){ 
			if (SelfPan[_i]==bak[i]){
				bak[i] = -1;
				break;
			}
		}
	}
	this->SelfPanLenNow = count;
	PushData * da = new PushData();
	da->add(KeyID);
	da->add(char(3));
	da->add(bak,SelfPanLen);
	da->add(SelfPan,SelfPanLen);
	da->add(GetSelfNum());
	//da->add(k);
	this->Func(this->lpParamter,da->GetData(),da->GetDataLen(),_ClickSend_4,this);
	delete da;
	delete [] bak;
	
}
bool Scmj::ClickSelfPan(int k){

	hBitmap2Ipls(Screen(),this->ScreenImg);
	if ( this->BlockList[8]->Child[12]->ClickFindOne(this->ScreenImg,k)){
		int n;
		do{
			Sleep(this->heartTime*2);
			hBitmap2Ipls(Screen(),this->ScreenImg);

			n = this->BlockList[4]->FindOne(ScreenImg);
			if (n != 4 && n != 3)break;
			if ( this->BlockList[8]->Child[13]->FindOne(ScreenImg) ==  -1)break;

			if ( !this->BlockList[8]->Child[12]->ClickFindOne(this->ScreenImg,k))break;

		}while(true);

		for (int i = 0;i<this->SelfPanLen;i++){
			if(this->SelfPan[i]==k){
				this->SelfPan[i] = -1;
				break;
			}
		}
		
		return true;
	}else{
		return false;
	}

}
bool Scmj::ClickSelfBotton(int k){
	hBitmap2Ipls(Screen(),this->ScreenImg);
	if (this->BlockList[8]->Child[16]->ClickFindOne(this->ScreenImg,k)){
		/**
		do{
			Sleep(this->heartTime*2);
			hBitmap2Ipls(Screen(),this->ScreenImg);
		}while(this->BlockList[8]->Child[16]->ClickFindOne(this->ScreenImg,k));
		**/

		if (k != 3){
			int n;
			while (true){
				Sleep(this->heartTime*2);
				hBitmap2Ipls(Screen(),this->ScreenImg);	

				if (this->BlockList[8]->Child[13]->FindOne(this->ScreenImg) != -1){
					Send_3();
					break;
				}else{
					n = this->BlockList[4]->FindOne(ScreenImg);
					if (n != 4 && n != 3)break;
				}

				this->BlockList[8]->Child[16]->ClickFindOne(this->ScreenImg,k);
			}
			
			
		}
		return true;
	}else{
		return false;
	}
}