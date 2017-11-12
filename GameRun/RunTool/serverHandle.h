#ifndef SERVER_H
#define SERVER_H

#include "Socket.h"
#include "common.h"
#include "time.h"

const int listenPort = 2000;
class Filters {
public:
	Filters(const TCHAR *FilterName,const int _listenPort);
	~Filters();
	BOOL check(DWORD h);
	friend BOOL Test(MIB_TCP tcpmib, void * lpParamter);
	int listenPort;
	//FindInfo &fi;
private:
	TCHAR * ProcessNames;
	DWORD *ProcessIDs;
	int count;
	friend BOOL CheckFilterProcessID(LPVOID lpParamter,TCHAR * ProName, DWORD h);	
	DWORD tmpProcessID;
};

typedef struct {
	
	//int RemoteMonitorPort;
	Socket* so;
	Filters* f;
	int LocalPort;
	clock_t start;
}FindInfo;

typedef BOOL (HandleBack)( MIB_TCP tcpmib, void *obj  );  

BOOL loadPortWithHandle(MIB_TCP tcpmib, void *obj );
void FindLoclPort(HandleBack FUNC,void *obj ) ;
unsigned __stdcall SocketHandle(void* a);

unsigned __stdcall RunServer(void* a);
unsigned SyncRunServer(Filters *f);

#endif