// GameRun.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "monitor.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Monitor *mo  =new Monitor( _T(".\\base.ini"));
	mo->run();
	return 0;
}

