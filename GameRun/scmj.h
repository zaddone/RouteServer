#ifndef SCMJ_H
#define SCMJ_H
#include "GameBase.h"


//const TCHAR * GName = L"scmj";

class Scmj  : public GameBase {
public:
	Scmj(const TCHAR * Base,CallBackSendData _f=NULL , LPVOID lp =NULL ,TCHAR * _gName = L"scmj",char k = 'O');

	//using GameBase::go;
    //virtual print(float a);
	virtual int go(int roomID);
	//GameBase * Par;
private:	
	
};

#endif