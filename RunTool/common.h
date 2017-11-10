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
    DWORD dwState;      //����״̬   
    DWORD dwLocalAddr;  //���ص�ַ   
    DWORD dwLocalPort;  //���ض˿�   
    DWORD dwRemoteAddr; //Զ�̵�ַ   
    DWORD dwRemotePort; //Զ�̶˿�   
    DWORD dwProcessId;  //���̱�ʶ 
}MIB_TCPEXROW,*PMIB_TCPEXROW;  


typedef struct  
{  
    DWORD dwLocalAddr;  //���ص�ַ   
    DWORD dwLocalPort;  //���ض˿�   
    DWORD dwProcessId;  //���̱�ʶ   

}MIB_UDPEXROW,*PMIB_UDPEXROW;  


typedef struct  
{  
    DWORD dwState;      //����״̬   
    DWORD dwLocalAddr;  //���ص�ַ   
    DWORD dwLocalPort;  //���ض˿�   
    DWORD dwRemoteAddr; //Զ�̵�ַ   
    DWORD dwRemotePort; //Զ�̶˿�   
    DWORD dwProcessId;  //���̱�ʶ   
    DWORD Unknown;      //������ʶ
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
//Descripion: �ú�������ֻ�� Windows XP��Windows Server 2003 ����Ч                    //   
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
//Descripion: �ú�������ֻ�� XP��Windows Server 2003 ����Ч                            //   
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
//Descripion: �ú����� Windows Vista �Լ� Windows 7 ����Ч                             //   
//=====================================================================================//   
typedef DWORD (WINAPI *PFNInternalGetTcpTable2)(  
    PMIB_TCPEXTABLE_VISTA *pTcpTable_Vista,  
    HANDLE heap,  
    DWORD flags  
    );  

//=====================================================================================//   
//Name: DWORD InternalGetUdpTableWithOwnerPid()                                        //   
//Descripion: �ú����� Windows Vista �Լ� Windows 7 ����Ч                             //   
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