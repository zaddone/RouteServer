#include "stdafx.h"
#include "common.h"

MIB_TCP TCPEXROWTO(MIB_TCPEXROW v){
	MIB_TCP t;
	t.RemoteAddr.sin_family = AF_INET;
	t.RemoteAddr.sin_addr.s_addr = v.dwRemoteAddr;
	t.RemoteAddr.sin_port =ntohs(0x0000FFFF & v.dwRemotePort);

	t.LocalAddr.sin_family = AF_INET;
	t.LocalAddr.sin_addr.s_addr = v.dwLocalAddr;
	t.LocalAddr.sin_port = ntohs(0x0000FFFF &v.dwLocalPort);


	t.dwProcessId = v.dwProcessId;
	//t.Addr = inet_ntoa(t.RemoteAddr.sin_addr);
	return t;
}
MIB_TCP TCPEXROW_VISTATO(MIB_TCPEXROW_VISTA v){
	MIB_TCP t;
	t.RemoteAddr.sin_family = AF_INET;
	t.RemoteAddr.sin_addr.s_addr = v.dwRemoteAddr;
	t.RemoteAddr.sin_port = ntohs(0x0000FFFF & v.dwRemotePort);

	t.LocalAddr.sin_family = AF_INET;
	t.LocalAddr.sin_addr.s_addr = v.dwLocalAddr;
	t.LocalAddr.sin_port = ntohs(0x0000FFFF &v.dwLocalPort);

	t.dwProcessId = v.dwProcessId;
	return t;
}
BOOL OnFshutdown() 
{
    // TODO: Add your control notification handler code here
    HANDLE       hToken;     
    TOKEN_PRIVILEGES tkp;     
    
    // Get a  token  for  this  process.     
    // OpenProcessToken() ��������������Ǵ�һ�����̵ķ�������
    // GetCurrentProcess() �����������ǵõ������̵ľ��
    
    
    if (!OpenProcessToken(GetCurrentProcess(),     
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken))     
        return( FALSE );     
    
    // Get the LUID for the shutdown privilege.     
    // LookupPrivilegeValue() ���������޸Ľ��̵�Ȩ��
    LookupPrivilegeValue(NULL,SE_SHUTDOWN_NAME,     
        &tkp.Privileges[0].Luid);     
    
    tkp.PrivilegeCount = 1;        //one privilege to set  ������������Ȩ
    
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;     
    
    // Get the shutdown privilege for  this process.     
    // AdjustTokenPrivileges������������֪ͨWindows NT�޸ı����̵�Ȩ��
    
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,     
        (PTOKEN_PRIVILEGES)NULL, 0);     
    
    if (GetLastError() != ERROR_SUCCESS)     
        return     FALSE;     
    
    // Shut  down the   system  and  force all applications to close.     
    
    if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))      //�������������á�    ǿ���˳�WINDOWS��
        return     FALSE;     
    
    return     TRUE; 
}


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
			}
			//_tprintf( TEXT("%s  (PID: %u)\n"), szProcessName, arrProIds[i] );
			//auto p = _tcsstr(szProcessName,ProName);
			//strstr(szProcessName,ProName)
			//if (_tcsstr(szProcessName,ProName)){
			
			isFind = F(lpParamter,szProcessName,arrProIds[i]);
			
			CloseHandle( hProcess );
			if (isFind) break;
		}
	}

	return isFind;
}

void KillProcess( DWORD h){

	HANDLE hProcess = OpenProcess( 
			PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ|
			PROCESS_TERMINATE,
			FALSE, h);
	//HANDLE   hProcess   =   OpenProcess(PROCESS_ALL_ACCESS,FALSE,h);
	TerminateProcess(hProcess,   0);
	CloseHandle(hProcess);
}