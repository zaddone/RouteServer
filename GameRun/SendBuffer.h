#ifndef SendBuffer_H_
#define SendBuffer_H_
#include "Socket.h"
#include <time.h>
#include<vector>
using namespace std;

#define random(x) (rand()%x)

const char rander[63] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
const long TimeOut = 1000*60*5;  
typedef void (*CallBackFun)(LPVOID handle,char * data,bool * wait);
class SendDB  
{  
public:
	SendDB(CallBackFun _call,LPVOID _lp,char * _data,bool * w );
	~SendDB();
	CallBackFun call;
	LPVOID lp;
	char * data;
	bool * Wait;
}; 

class SocketBuffer
{
public:
	SocketBuffer(char *_host,const int _port,const int heart,CallBackFun _reloadSystem = NULL,LPVOID _reloadSystemInfo=NULL);
	~SocketBuffer();
	friend void LoadSendData(LPVOID lpParamter,const char * data ,CallBackFun _call = NULL,LPVOID _lp = NULL);
	//friend void LoadSendDataSync(LPVOID lpParamter,const char * data ,CallBackFun _call = NULL,LPVOID _lp = NULL);
	char Key[6];
	void SetReloadSystem(CallBackFun _reloadSystem ,LPVOID _reloadSystemInfo );
private:

	CallBackFun reloadSystem;
	LPVOID reloadSystemInfo;

	clock_t clock_begin;
	clock_t clock_end;
	SocketClient *socketClient;
	char * host;
	int port;
	int HeartTime;
	vector <SendDB*> Sendlist;
	void ConnServer();
	void SetKey();
	void sendData(const char * data);
	void recvData(SendDB * db);
	friend void runThread(LPVOID lpParamter);
	void syncSend();
	void GetSendDB(const char * data,char * outData);
};


#endif