#ifndef SCMJ_H
#define SCMJ_H
#include "GameBase.h"


//const TCHAR * GName = L"scmj";

class Scmj  : public GameBase {
public:
	Scmj(const TCHAR * Base,CallBackSendData _f=NULL , LPVOID lp =NULL ,TCHAR * _gName = L"scmj",char k = 'O');
	~Scmj();
	//using GameBase::go;
    //virtual print(float a);
	virtual bool go(int roomID);
	virtual void CloseWindows();
	//GameBase * Par;
private:	
	bool game();
	bool checkRoom();
	bool inRoom(int roomID);
	bool inGame();
	void ScreenOther(int n,int &lastn,int &lastN);
	void ScreenSelf( int &lastN,int &lastn);
	bool Send_1();
	bool Send_2();
	bool Send_4(int la);
	bool Send_4_1();
	void Send_5();
	void Send_3();
	
	bool FindSee();
	void GetSelfPan(int & OutLen);
	int GetSelfNum();
	bool ClickSelfBotton(int k);
	bool ClickSelfPan(int k);
	//void refresh();

	//int SelfNum;
	int SelfPanLenNow;
	int SelfPanLen;
	char * SelfPan;
	friend void _ClickBack(LPVOID handle,char * data ,int len);

	friend void _ClickSend_1(LPVOID handle,char * data ,int len);
	friend void _ClickSend_2(LPVOID handle,char * data ,int len);
	friend void _ClickSend_4(LPVOID handle,char * data ,int len);
	friend void _ClickSend_5(LPVOID handle,char * data ,int len);
};

#endif