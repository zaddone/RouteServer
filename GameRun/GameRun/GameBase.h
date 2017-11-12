#ifndef GameBase_H
#define GameBase_H
#include "block.h"


typedef void (*CallBackFun)(LPVOID handle,char * data);
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
	virtual bool  go(int roomID){return true;};
	virtual void reloadWindows(){};
	TCHAR * gameName;
	TCHAR * GName;
	char * Tag;
	char KeyID;
	
	GameBase * Par;
	//GameBase * NextGame;
protected:
	int heartTime;
	IplImage * ScreenImg;
	vector <Block*> BlockList;
	CallBackSendData Func;
	LPVOID lpParamter;
	bool IsGo;
	//bool isRun;
	
	void SetGNameAndTag(TCHAR * G);
	void InRoom();
private:

};


#endif