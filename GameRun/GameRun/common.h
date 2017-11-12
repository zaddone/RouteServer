#ifndef Common_H_
#define Common_H_

#include <Windows.h>
#include <psapi.h>

//#include <vector>
//using namespace std;



typedef BOOL (HandleFUNC)(LPVOID lpParamter,HANDLE hProcess,TCHAR * ProName, DWORD h ); 
BOOL FindEnumProcess(HandleFUNC F,LPVOID lpParamter );
void FindEnumProcessAll(HandleFUNC F,LPVOID lpParamter );
BOOL KillWindowsExe(LPVOID lpParamter,HANDLE hProcess,TCHAR * ProName, DWORD h);
BOOL _ShowWindows(LPVOID lpParamter,HANDLE hProcess,TCHAR * ProName, DWORD h);
void reloadSystem(LPVOID handle,char * data,bool * wait);

#endif