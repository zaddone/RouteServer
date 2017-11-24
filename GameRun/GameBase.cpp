#include "stdafx.h"
#include "GameBase.h"
#include <fstream>
#include "common.h"

PushData::PushData(int max){
	this->data = new char[max];
	this->len=0;
	this->Max = max;
}
PushData::~PushData(){
	delete [] data;
}
void PushData::add(const char v){
	data[len]=v;
	len++;
}
void PushData::add(const int v){
	size_t l = sizeof(v);
	memcpy(data+len,&v,l);
	len+=l;
}
void PushData::add(const char * v,const int l){
	memcpy(data+len,v,l);
	len+=l;
}
char * PushData::GetData(){
	return this->data;
}
int PushData::GetDataLen(){
	return this->len;
}
void GameBase::trainBlock(CallBackTask _f){
	for (vector<Block*>::iterator it = BlockList.begin(); it != BlockList.end(); it ++) {
		(*it)->ErgodicBlock(_f);
		//if ((*it)->NL == NULL)
	}
}
void GameBase::trainBlockNL(){
	for (vector<Block*>::iterator it = BlockList.begin(); it != BlockList.end(); it ++) {
		(*it)->ErgodicBlock(trainNL);
		//if ((*it)->NL == NULL)
	}
}
void GameBase::SetCallBack(CallBackSendData _f , LPVOID lp){
	this->Func = _f;
	this->lpParamter = lp;
}
void GameBase::SetGNameAndTag(TCHAR * G){
	GName = G;
	Tag = new char[10];
	int len = WideCharToMultiByte(CP_ACP,0,GName,-1,NULL,0,NULL,NULL);  
	WideCharToMultiByte(CP_ACP,0,GName,-1,Tag,len,NULL,NULL );  
	
}
void GameBase::SetBaseIni(const TCHAR * Base){
	TCHAR mapPath[1024];
	char  mapData[1024];
	GetPrivateProfileString(GName,_T("mapPath"),_T("scmj.log"),mapPath ,1024,Base);
	int len = WideCharToMultiByte(CP_ACP,0,mapPath,-1,NULL,0,NULL,NULL);  
	WideCharToMultiByte(CP_ACP,0,mapPath,-1,mapData,len,NULL,NULL );  

	fstream In(mapData, ios::in|ios::binary);	
	if (In.is_open())   
    {  		
		printf("open ok %s\r\n",mapData);
		char tmpStr[1024];
		int UserArrLen = 0;
		char * tag = new char [1024];
		int len = WideCharToMultiByte(CP_ACP,0,GName,-1,NULL,0,NULL,NULL); 
		WideCharToMultiByte(CP_ACP,0,GName,-1,tag,len,NULL,NULL );  
		//CvPoint ps = {0,0};
		while (!In.eof() ){
			In.getline(tmpStr ,1024,'\n');
			if (strlen(tmpStr)==0){
				break;
			}
			//printf("%s %s\r\n",tmpStr,tag);
			Block *bl = new Block(tmpStr,NULL,UserArrLen,tag);
			BlockList.push_back(bl);
			UserArrLen++;
		}
        In.close();
	}
	this->gameName = new TCHAR[1024];
	GetPrivateProfileString(GName,_T("gameName"),_T("tkmc.exe"),this->gameName ,1024,Base);

	heartTime = GetPrivateProfileInt(GName,_T("heartTime"),1000,Base);
}
GameBase::GameBase(){
	ScreenImg = hBitmap2Ipl(Screen());
	GName=NULL;
	gameName=NULL;
	Tag=NULL;
}
GameBase::~GameBase(){
	for (vector<Block*>::iterator it = BlockList.begin(); it != BlockList.end(); it ++) {
		delete (*it);
		*it = NULL;
	}
	this->BlockList.clear();
	cvReleaseImage(&ScreenImg);
	if (GName != NULL)	delete [] GName;
	if (gameName != NULL) delete [] gameName;
	if (Tag != NULL) delete [] Tag;
}
bool GameBase::CheckWindows(){
	
	if(FindEnumProcess(_CheckWindows,this->gameName))return true;
	return false;
}
bool GameBase::ShowWindows(){
	
	if(FindEnumProcess(_ShowWindows,this->gameName))return true;
	return false;
}
void GameBase::InRoom(){
	
	MouseMove(120,120);		
	Sleep(100);
	MouseLeftDown();
	Sleep(100);
	MouseLeftUp();
	Sleep(100);
	
	//this->ShowWindows();
	keybd_event(KeyID,0,0,0);
	Sleep(100);
	keybd_event(KeyID,0,KEYEVENTF_KEYUP,0);
	/**
	char sh[2]={0};
	sh[0] = KeyID;
	printf("KEYEVENTF_KEYUP %s",sh);
	**/
}

bool hBitmap2Ipls(HBITMAP hBmp,IplImage* dst ){

	BITMAP bmp;	
    GetObject(hBmp,sizeof(BITMAP),&bmp); 
    int    nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel/8;
	//int    nChannels = bmp.bmBitsPixel == 1 ? 1 : 4;
    int    depth     = 8;//bmp.bmBitsPixel;//   == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	//printf("%d %d %d %d %d \r\n",bmp.bmBitsPixel,nChannels,depth,bmp.bmWidth, bmp.bmHeight );
    IplImage *img=cvCreateImage(cvSize(bmp.bmWidth, bmp.bmHeight), IPL_DEPTH_8U,nChannels);
    BYTE *pBuffer = new BYTE[bmp.bmHeight*bmp.bmWidth*nChannels];
	GetBitmapBits(hBmp,bmp.bmHeight*bmp.bmWidth*nChannels,pBuffer);
	memcpy(img->imageData,pBuffer,bmp.bmHeight*bmp.bmWidth*nChannels);
    delete pBuffer; 


	if (img->width != dst->width || img->height != dst->height){
		cvReleaseImage(&img);  
		DeleteObject(hBmp);
		return false;
	}
	
	if (nChannels == 2)
		cvCvtColor(img,dst,CV_BGR5652GRAY);  
	else if (nChannels == 3)
		cvCvtColor(img,dst,CV_BGR2GRAY);
	else if (nChannels == 4)
		cvCvtColor(img,dst,CV_BGRA2GRAY);

	cvReleaseImage(&img);  
	DeleteObject(hBmp);
	return true;
}
IplImage* hBitmap2Ipl(HBITMAP hBmp )
{
	//bitmap.GetBitmap(&bm); 
    BITMAP bmp;
    GetObject(hBmp,sizeof(BITMAP),&bmp);
    int    nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel/8;
    int    depth     = 8;//bmp.bmBitsPixel;// == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	//return NULL;
    IplImage *img=cvCreateImage(cvSize(bmp.bmWidth, bmp.bmHeight), IPL_DEPTH_8U,nChannels);
	
    BYTE *pBuffer = new BYTE[bmp.bmHeight*bmp.bmWidth*nChannels];
	GetBitmapBits(hBmp,bmp.bmHeight*bmp.bmWidth*nChannels,pBuffer);
	memcpy(img->imageData,pBuffer,bmp.bmHeight*bmp.bmWidth*nChannels);
    delete pBuffer; 
	IplImage *dst = cvCreateImage(cvGetSize(img),depth,1);

	if (nChannels == 2)
		cvCvtColor(img,dst,CV_BGR5652GRAY);  
	else if (nChannels == 3)
		cvCvtColor(img,dst,CV_BGR2GRAY);
	else if (nChannels == 4)
		cvCvtColor(img,dst,CV_BGRA2GRAY);	

	//cvSplit(img,dst,0,0,0);
	cvReleaseImage(&img);  
	DeleteObject(hBmp);
	return dst;  

}

HBITMAP Screen()
{
	//printf("test 1\r\n");
	HDC desk=GetDC(GetDesktopWindow());
	int screenWidth = GetDeviceCaps(desk,HORZRES);     //фад╩©М
	int screenHeight = GetDeviceCaps(desk,VERTRES);     //фад╩╦ъ
	HBITMAP memBitmap=CreateCompatibleBitmap(desk,screenWidth,screenHeight);
	HDC memDC=CreateCompatibleDC(desk);

	SelectObject(memDC,memBitmap);
	BitBlt(memDC,0,0,screenWidth,screenHeight,desk,0,0,SRCCOPY|CAPTUREBLT);
	BITMAP bmpInfo;
	GetObject(memBitmap,sizeof(bmpInfo),&bmpInfo);
	DeleteDC(memDC);
	ReleaseDC(GetDesktopWindow(),desk);
	return  memBitmap;
}
