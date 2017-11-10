// GameRun.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "monitor.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Monitor *mo  =new Monitor( _T(".\\base.ini"));
	mo->run();
	return 0;
}

