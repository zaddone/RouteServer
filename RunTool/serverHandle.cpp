#include "stdafx.h"

#include "serverHandle.h"
#include <process.h>

void FindLoclPort(HandleBack FUNC,void *obj ) 
{  
    //DWORD dwPortCount = 0;  
    HMODULE hModule = LoadLibraryW(L"iphlpapi.dll");  
    if (hModule == NULL)  
    {  
        return;  
    }  
 
        // 表明查询的是 UDP 信息   
        PFNAllocateAndGetTcpExTableFromStack pAllocateAndGetTcpExTableFromStack;  
        pAllocateAndGetTcpExTableFromStack = (PFNAllocateAndGetTcpExTableFromStack)GetProcAddress(hModule, "AllocateAndGetTcpExTableFromStack");  
        if (pAllocateAndGetTcpExTableFromStack != NULL)  
        {  
            // 表明为 XP 或者 Server 2003 操作系统   
            PMIB_TCPEXTABLE pTcpExTable = NULL;  
            if (pAllocateAndGetTcpExTableFromStack(&pTcpExTable, TRUE, GetProcessHeap(), 0, AF_INET) != 0)  
            {  
                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                //return dwPortCount;  
				return;
            }  
			int count = 0;
            for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)  
            {  
			 

				if (FUNC(TCPEXROWTO(pTcpExTable->table[i]),obj))break;
				 
					 
					// 过滤掉数据，只获取我们要查询的进程的 Port 信息   
              
            }  

            if (pTcpExTable)  
            {  
                HeapFree(GetProcessHeap(), 0, pTcpExTable);  
            }  

            FreeLibrary(hModule);  
            hModule = NULL;  

            return;  
        }  
        else  
        {  
            // 表明为 Vista 或者 7 操作系统   
            PMIB_TCPEXTABLE_VISTA pTcpExTable = NULL;  
            PFNInternalGetTcpTable2 pInternalGetTcpTable2 = (PFNInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");  
            if (pInternalGetTcpTable2 == NULL)  
            {  
                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return;  
            }  

            if (pInternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1))  
            {  
                if (pTcpExTable)  
                {  
                    HeapFree(GetProcessHeap(), 0, pTcpExTable);  
                }  

                FreeLibrary(hModule);  
                hModule = NULL;  

                return;  
            }  
			int count = 0;
            for (UINT i = 0;i < pTcpExTable->dwNumEntries; i++)  
            {  
				if (FUNC(TCPEXROW_VISTATO(pTcpExTable->table[i]),obj))break;
	 
                // 过滤掉数据，只获取我们要查询的进程的 TCP Port 信息   
               
            }  

            if (pTcpExTable)  
            {  
                HeapFree(GetProcessHeap(), 0, pTcpExTable);  
            }  

            FreeLibrary(hModule);  
            hModule = NULL;  

            return;  
        }  
 
}  

BOOL loadPortWithHandle(MIB_TCP tcpmib, void *obj){


	//printf("%s:%d %s:%d %d\r\n",inet_ntoa(tcpmib.LocalAddr.sin_addr),tcpmib.LocalAddr.sin_port,inet_ntoa(tcpmib.RemoteAddr.sin_addr),tcpmib.RemoteAddr.sin_port,val);
	FindInfo *f = (FindInfo *) obj; 
	if (tcpmib.LocalAddr.sin_port != f->LocalPort){ 
		return FALSE;
	}

	char *addr =new char[100];
	if (f->f->check(tcpmib.dwProcessId)){
	//tcpmib.RemoteAddr.sin_addr.S_un
		sprintf(addr,"%s:%d\0",inet_ntoa(tcpmib.RemoteAddr.sin_addr),tcpmib.RemoteAddr.sin_port);
		//printf("addr %s\r\n",addr);
	}else{
		KillProcess(tcpmib.dwProcessId);
		addr[0] = 2;
		addr[1] = 0;
	}
	try{
		f->so->SendBytes(addr);
		//f->so->Close();
		f->LocalPort = 0;
		//delete f->so;
		//f->so = NULL;
		//delete [] addr;
		
	} catch (...) {		

		delete f->so;
		f->so = NULL;

		//printf("Send Err \r\n");
		//return TRUE;
	}
	delete addr;
	return TRUE;
}

unsigned __stdcall SocketHandle(void* a) {
	FindInfo *f = (FindInfo *) a;
  //Socket* s = (Socket*) a;
	
	int len;
	char buf[1024] = {0};
	while(true){
		//printf("full1\r\n");
		//buf={0};
		//char *buf =new char[10];
		try{
			len = f->so->ReceiveBytes(buf,1024);

		}catch (...) {
			//printf("ReceiveBytes SOCKET_ERROR\r\n");
			break;
		}
		//printf("full2\r\n");
		//SOCKET_ERROR
		if (len>0) {
			f->LocalPort = atoi(buf);
			FindLoclPort(loadPortWithHandle,a);
		}else{
			//printf("len SOCKET_ERROR %d %s\r\n",len ,buf);
			break;
		}
		//printf("full3\r\n");
		if (f->LocalPort != 0){
			//printf("find not %d\r\n",f->LocalPort);
			char buf[2] = {1,0};
			//if (f->LocalPort < 0) buf[1] = 2;
			try{
				f->so->SendBytes(buf);
			}catch (...) {			
				//printf("LocalPort Not Fount\r\n");				
				break;			
			}
		}
		//printf("full4\r\n");
	} 
	delete f->so;
	delete f;
	//printf("conn stop\r\n");
	return 0;
}
unsigned __stdcall RunServer(void* a){
	Filters *F = (Filters *) a;
	
	SocketServer in(F->listenPort,5);
  //printf("begin\r\n");
	while (true) {
		//printf("for\r\n");
		//F->fi
		FindInfo f;
		f.f = F;
		f.so=in.Accept();
		unsigned ret;
		_beginthreadex(0,0,SocketHandle,&f,0,&ret);
	}
	//delete f;
	return 0;
}

 unsigned SyncRunServer(Filters *F ){
	 unsigned ret;
	_beginthreadex(0,0,RunServer,F,0,&ret);
	return ret;
}
 /**
 		try{
			SocketClient s("192.168.1.50", 3330);
			s.SendBytes(key);
		    while (1) {
			  string l = s.ReceiveLine();
			  if (l.empty()) break;
			  printf("%s\r\n",l.c_str());			
			}
		}  catch (const char* s) {
			cerr << s << endl;
		} 
		catch (std::string s) {
			cerr << s << endl;
		} 
		catch (...) {
			cerr << "unhandled exception\n";
		}

 **/
Filters::Filters(const TCHAR *FilterName,const int _listenPort){
	size_t le = wcslen(FilterName);
	this->ProcessNames =  new TCHAR[le];	
	lstrcpy(this->ProcessNames,FilterName);
	_wcslwr_s(this->ProcessNames,le+1);
	this->ProcessIDs = new DWORD[10];
	this->count = 0;
	this->listenPort = _listenPort;
}
Filters::~Filters(){
	delete [] this->ProcessNames;
	delete [] this->ProcessIDs;
}
BOOL Filters::check(DWORD h){
	for (int i=0;i<this->count;i++){
		if (h == this->ProcessIDs[i])return TRUE;
	}
	this->tmpProcessID = h;
	if (wcslen(this->ProcessNames)>0){
		return FindEnumProcess(CheckFilterProcessID,this);
	}
	return FALSE;
}
BOOL CheckFilterProcessID(LPVOID lpParamter,TCHAR * ProName, DWORD h){
	Filters * f = (Filters *) lpParamter;
	if (f->tmpProcessID != h) return FALSE;
	size_t le = wcslen(ProName);
	_wcslwr_s(ProName,le+1);
	TCHAR *N = NULL, *N1=NULL;
	N = wcsstr(f->ProcessNames,ProName);
	//wprintf(L"%s %s\r\n",f->ProcessNames,ProName);
	if (N!= NULL){
		N1 = N+le;
		wmemcpy(N,N1,wcslen(N1)+1);		
		f->ProcessIDs[f->count] = f->tmpProcessID;
		f->count++;
		wprintf(L"%s %s %d %d\r\n",f->ProcessNames,ProName,f->tmpProcessID,f->count);
		return TRUE;
	}
	return FALSE;
}
BOOL Test(MIB_TCP tcpmib, void * lpParamter){
	Filters * f = (Filters *) lpParamter;
	return f->check(tcpmib.dwProcessId);
}
