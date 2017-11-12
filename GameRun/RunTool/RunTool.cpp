// RunTool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "serverHandle.h"


//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"wmainCRTStartup\"" )
BOOL CheckProcessName(LPVOID lpParamter,TCHAR * ProName, DWORD h){
	TCHAR * pro = (TCHAR *)lpParamter;
	size_t le = wcslen(ProName);
	_wcslwr_s(ProName,le+1);
	TCHAR *N = NULL ;
	N = wcsstr(pro,ProName);
	if (N != NULL){
		return TRUE;
	}
	return FALSE;
}
int _tmain(int argc, _TCHAR* argv[]){
	
	__in_opt LPCWSTR IniFile = _T(".\\base.ini");
	int listenPort= GetPrivateProfileInt(_T("listen"),_T("Listenport"),2000,IniFile);

	TCHAR watch_pro[15];
	GetPrivateProfileString(_T("Process"),_T("watch"),_T("run.exe"),watch_pro ,15,IniFile);

	TCHAR filter_pro[50];
	//char watch[50];
	GetPrivateProfileString(_T("Process"),_T("filter"),_T("TKCltNet.exeTkMC.exeTKLobby.exe"),filter_pro,50,IniFile);
	wprintf(L"%s %d\r\n",filter_pro,wcslen(filter_pro));
	Filters * F = new Filters(filter_pro,listenPort);
	//FindInfo f;
	//f.f = F;// new Filters(filter_pro);
	unsigned ret = SyncRunServer(F);

	
	//FindLoclPort(Test,F);
	//f.start = clock(); 
	
	printf("ret: %d\r\n",ret);
	//Sleep(10000);
	if (!FindEnumProcess(CheckProcessName,watch_pro)){
		printf("open run.exe\r\n");
		//ShellExecute(NULL,L"open",watch_pro,NULL,NULL, SW_HIDE ); 
		ShellExecute(NULL,L"open",L"run.exe",NULL,NULL, SW_SHOW ); 
	}
	long heart = 1000*60;
	while(true){
		//clock_t finish = clock(); s		
		Sleep(heart);
		if (!FindEnumProcess(CheckProcessName,watch_pro)){
			printf("open run.exe");
			ShellExecute(NULL,L"open",L"run.exe",NULL,NULL, SW_SHOW ); 
			//ShellExecute(NULL,L"open",watch_pro,NULL,NULL, SW_HIDE );
		}
		/**
		if ((clock() - f.start)>heart ){
			printf("is stop socket\r\n");
			OnFshutdown();

			break;			
		}
		**/
	}
	delete F;
	return 0;
}