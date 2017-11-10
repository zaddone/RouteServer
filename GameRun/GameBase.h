#ifndef GameBase_H
#define GameBase_H
#include "block.h"


typedef void (*CallBackFun)(LPVOID handle,char * data,bool * wait);
typedef void (*CallBackSendData)(LPVOID lpParamter,const char * data ,CallBackFun _call ,LPVOID _lp );
bool hBitmap2Ipls(HBITMAP hBmp,IplImage* dst );
IplImage* hBitmap2Ipl(HBITMAP hBmp );
HBITMAP Screen();

class GameBase{
public:
	GameBase();
	~GameBase();
	void SetBaseIni(const TCHAR * Base);
	void SetCallBack(CallBackSendData _f , LPVOID lp);
	bool ShowWindows();
	virtual void CloseWindows(){};
	virtual int  go(int roomID){return 0;};
	TCHAR * gameName;
	TCHAR * GName;
	char * Tag;
	char KeyID;
	bool Stop;
	GameBase * Par;
	//GameBase * NextGame;
protected:
	int heartTime;
	IplImage * ScreenImg;
	vector <Block*> BlockList;
	CallBackSendData Func;
	LPVOID lpParamter;

	
	void SetGNameAndTag(TCHAR * G);
	void InRoom();
private:

};


#endif