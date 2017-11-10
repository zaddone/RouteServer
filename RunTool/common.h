#ifndef COMMON_H
#define COMMON_H

//#include "Socket.h"
#include <Windows.h>   

#include <Psapi.h>   
#include <Iprtrmib.h>   
//#include <Winsock2.h>

#pragma  comment(lib,"Psapi.lib")
//#pragma  comment(lib,"Iphlpapi.Lib")
//#pragma  comment(lib,"WS2_32.lib")

typedef struct {
	TCHAR * ProName;
	DWORD ProcessID;
}Filter;
 
typedef struct  
{
	sockaddr_in LocalAddr;
	sockaddr_in RemoteAddr;
	DWORD dwProcessId;
	//char * Addr;

}MIB_TCP;  


typedef struct  
{  
    DWORD dwState;      //连接状态   
    DWORD dwLocalAddr;  //本地地址   
    DWORD dwLocalPort;  //本地端口   
    DWORD dwRemoteAddr; //远程地址   
    DWORD dwRemotePort; //远程端口   
    DWORD dwProcessId;  //进程标识 
}MIB_TCPEXROW,*PMIB_TCPEXROW;  


typedef struct  
{  
    DWORD dwLocalAddr;  //本地地址   
    DWORD dwLocalPort;  //本地端口   
    DWORD dwProcessId;  //进程标识   

}MIB_UDPEXROW,*PMIB_UDPEXROW;  


typedef struct  
{  
    DWORD dwState;      //连接状态   
    DWORD dwLocalAddr;  //本地地址   
    DWORD dwLocalPort;  //本地端口   
    DWORD dwRemoteAddr; //远程地址   
    DWORD dwRemotePort; //远程端口   
    DWORD dwProcessId;  //进程标识   
    DWORD Unknown;      //待定标识
}MIB_TCPEXROW_VISTA,*PMIB_TCPEXROW_VISTA;  


typedef struct  
{  
    DWORD dwNumEntries;  
    MIB_TCPEXROW table[ANY_SIZE];
}MIB_TCPEXTABLE,*PMIB_TCPEXTABLE;  


typedef struct  
{  
    DWORD dwNumEntries;  
    MIB_TCPEXROW_VISTA table[ANY_SIZE];
}MIB_TCPEXTABLE_VISTA,*PMIB_TCPEXTABLE_VISTA;  


typedef struct  
{  
    DWORD dwNumEntries;  
    MIB_UDPEXROW table[ANY_SIZE];
}MIB_UDPEXTABLE,*PMIB_UDPEXTABLE;  


//=====================================================================================//   
//Name: DWORD AllocateAndGetTcpExTableFromStack()                                      //                                                                //   
//Descripion: 该函数仅仅只在 Windows XP，Windows Server 2003 下有效                    //   
//=====================================================================================//   
typedef DWORD (WINAPI *PFNAllocateAndGetTcpExTableFromStack)(  
    PMIB_TCPEXTABLE *pTcpTabel,  
    bool bOrder,  
    HANDLE heap,  
    DWORD zero,  
    DWORD flags  
    );  

//=====================================================================================//   
//Name: DWORD AllocateAndGetUdpExTableFromStack()                                      //   
//Descripion: 该函数仅仅只在 XP，Windows Server 2003 下有效                            //   
//=====================================================================================//   
typedef DWORD (WINAPI *PFNAllocateAndGetUdpExTableFromStack)(  
    PMIB_UDPEXTABLE *pUdpTable,  
    bool bOrder,  
    HANDLE heap,  
    DWORD zero,  
    DWORD flags  
    );  

//=====================================================================================//   
//Name: DWORD InternalGetTcpTable2()                                                   //   
//Descripion: 该函数在 Windows Vista 以及 Windows 7 下面效                             //   
//=====================================================================================//   
typedef DWORD (WINAPI *PFNInternalGetTcpTable2)(  
    PMIB_TCPEXTABLE_VISTA *pTcpTable_Vista,  
    HANDLE heap,  
    DWORD flags  
    );  

//=====================================================================================//   
//Name: DWORD InternalGetUdpTableWithOwnerPid()                                        //   
//Descripion: 该函数在 Windows Vista 以及 Windows 7 下面效                             //   
//=====================================================================================//   
typedef DWORD (WINAPI *PFNInternalGetUdpTableWithOwnerPid)(  
    PMIB_UDPEXTABLE *pUdpTable,  
    HANDLE heap,  
    DWORD flags  
    );  

MIB_TCP TCPEXROWTO(MIB_TCPEXROW);
MIB_TCP TCPEXROW_VISTATO(MIB_TCPEXROW_VISTA);
BOOL OnFshutdown();
typedef BOOL (HandleFUNC)(LPVOID lpParamter,TCHAR * ProName, DWORD h ); 
BOOL FindEnumProcess(HandleFUNC F,LPVOID lpParamter );
void KillProcess( DWORD h);
#endif