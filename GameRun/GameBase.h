#ifndef GameBase_H
#define GameBase_H
#include "block.h"


typedef void (*CallBackFun)(LPVOID handle,char * data,int len);
typedef void (*CallBackSendData)(LPVOID lpParamter,const char * data ,int len,CallBackFun _call ,LPVOID _lp );
bool hBitmap2Ipls(HBITMAP hBmp,IplImage* dst );
IplImage* hBitmap2Ipl(HBITMAP hBmp );
HBITMAP Screen();

class PushData{
public:
	PushData(int max=1024);
	~PushData();
	void add(const char v);
	void add(const int v);
	void add(const char * v, const int len);
	char * GetData();
	int GetDataLen();
protected:
	char *data;
	int Max ;
	int len ;	
};

class GameBase{
public:
	GameBase();
	~GameBase();
	void SetBaseIni(const TCHAR * Base);
	void SetCallBack(CallBackSendData _f , LPVOID lp);
	bool ShowWindows();
	bool CheckWindows();
	virtual void CloseWindows(){};
	virtual bool  go(int roomID){return true;};
	virtual void reloadWindows(){};
	virtual void trainBlockNL();
	virtual void trainBlock(CallBackTask _f);
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