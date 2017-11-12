#include "stdafx.h"
#include "common.h"



BOOL FindEnumProcess(HandleFUNC F,LPVOID lpParamter )
{
	DWORD arrProIds[1024], ByteCnt, ProCnt;

/*������еĽ��̵�ID*/
	if (!EnumProcesses(arrProIds, sizeof(arrProIds), &ByteCnt))
	{
		_tprintf( TEXT("��ȡ���н��̵�IDʧ�ܣ�\n"));
		return FALSE;
	}

/*�����õĽ���ID������*/
	ProCnt = ByteCnt / sizeof(DWORD);

/*������н��̵�����*/
	BOOL isFind = FALSE;
	for (unsigned int i = 0; i < ProCnt; i++)
	{
		if( arrProIds[i] != 0 )
		{
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

			HANDLE hProcess = OpenProcess( 
			PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ|
			PROCESS_TERMINATE, 
			FALSE, arrProIds[i]);

/*�õ����̵�����*/
			if (NULL != hProcess )
			{
				HMODULE hMod;
				DWORD ByteCnt;

				if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &ByteCnt) )
				{
					GetModuleBaseName( hProcess, hMod, szProcessName, 
					sizeof(szProcessName)/sizeof(TCHAR) );
				}
			
				isFind = F(lpParamter,hProcess,szProcessName,arrProIds[i]);
			
				CloseHandle( hProcess );
			}
			if (isFind) break;
		}
	}

	return isFind;
}

void FindEnumProcessAll(HandleFUNC F,LPVOID lpParamter )
{
	DWORD arrProIds[1024], ByteCnt, ProCnt;

/*������еĽ��̵�ID*/
	if (!EnumProcesses(arrProIds, sizeof(arrProIds), &ByteCnt))
	{
		_tprintf( TEXT("��ȡ���н��̵�IDʧ�ܣ�\n"));
		return;
	}

/*�����õĽ���ID������*/
	ProCnt = ByteCnt / sizeof(DWORD);

/*������н��̵�����*/
	BOOL isFind = FALSE;
	for (unsigned int i = 0; i < ProCnt; i++)
	{
		if( arrProIds[i] != 0 )
		{
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

			HANDLE hProcess = OpenProcess( 
			PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ|
			PROCESS_TERMINATE, 
			FALSE, arrProIds[i]);

/*�õ����̵�����*/
			if (NULL != hProcess )
			{
				HMODULE hMod;
				DWORD ByteCnt;

				if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &ByteCnt) )
				{
					GetModuleBaseName( hProcess, hMod, szProcessName, 
					sizeof(szProcessName)/sizeof(TCHAR) );
				}			
				F(lpParamter,hProcess,szProcessName,arrProIds[i]);			
				CloseHandle( hProcess );
			}
			//if (isFind) break;
		}
	}
	return;
}

int CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam){
	DWORD toFindPID = (DWORD)lParam;//��ȡҪ���ҵ�Ŀ�����ID
	DWORD processID;
	::GetWindowThreadProcessId(hwnd, &processID);//��ȡ��ǰ���ڵĽ���ID
	//DWORD curThreadID = ::GetCurrentThreadId();//��ǰ�߳�ID
	//DWORD toFindThreadID = ::GetWindowThreadProcessId(hwnd, &processID);//Ŀ���߳�ID
	if (processID == toFindPID)//������ҳɹ������л���������ǰ��
	{ 
		//printf("find ID\r\n");
		//::AttachThreadInput(toFindThreadID, curThreadID, TRUE);
		//::ShowWindow(hwnd, SW_NORMAL);
		if (IsWindowVisible(hwnd))
			::SetForegroundWindow(hwnd);
		/**
		int cxScreen,cyScreen;
		cxScreen=GetSystemMetrics(SM_CXSCREEN);
		cyScreen=GetSystemMetrics(SM_CYSCREEN);
		::SetWindowPos(hwnd,HWND_TOPMOST,0,0,cxScreen,cyScreen,SWP_SHOWWINDOW);
		**/
		//::AttachThreadInput(toFindThreadID, curThreadID, FALSE);
		//return 0;
	}
	//else
	//{
		return 1;
	//}
}
BOOL _CheckWindows(LPVOID lpParamter,HANDLE hProcess,TCHAR * ProName, DWORD h){
	TCHAR * pro = (TCHAR *)lpParamter;
	size_t le = wcslen(ProName);
	_wcslwr_s(ProName,le+1);
	TCHAR *N = NULL ;
	N = wcsstr(pro,ProName);
	if (N != NULL){		
		//EnumWindows(lpEnumFunc,h);
		return TRUE;
	}
	return FALSE;
}
BOOL _ShowWindows(LPVOID lpParamter,HANDLE hProcess,TCHAR * ProName, DWORD h){
	TCHAR * pro = (TCHAR *)lpParamter;
	size_t le = wcslen(ProName);
	_wcslwr_s(ProName,le+1);
	TCHAR *N = NULL ;
	N = wcsstr(pro,ProName);
	if (N != NULL){		
		EnumWindows(lpEnumFunc,h);
		return TRUE;
	}
	return FALSE;
}
BOOL KillWindowsExe(LPVOID lpParamter,HANDLE hProcess,TCHAR * ProName, DWORD h){

	TCHAR * pro = (TCHAR *)lpParamter;
	size_t le = wcslen(ProName);
	_wcslwr_s(ProName,le+1);
	TCHAR *N = NULL ;
	N = wcsstr(pro,ProName);
	if (N != NULL){
		return TerminateProcess(hProcess,0);
	}
	return FALSE;
}
void reloadSystem(LPVOID handle,char * data,bool * wait){
	//char * Process = (char *)handle;
	FindEnumProcessAll(KillWindowsExe,handle);

	if (wait!=NULL){
		wait[0] = false;
	}
}