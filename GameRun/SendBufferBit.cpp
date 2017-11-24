#include "stdafx.h"
#include "SendBufferBit.h"


SendDB::SendDB(char * _data,int len, bool * w,CallBackFun _call,LPVOID _lp){
	this->lp = _lp;
	this->sendBuf = _data;
	this->sendLen = len;
	this->call = _call;
	this->Wait = w;
	this->recvLen = 0;
	this->recvBuf = NULL;
}
SendDB::~SendDB(){
	if (this->Wait != NULL){
		this->Wait[0] = false;
	}
	//printf("delete %s\r\n",data);
	if (sendBuf!= NULL) delete [] sendBuf;
	if (recvBuf!= NULL)	delete [] recvBuf;
	
}
void syncWork(LPVOID lpParamter){
	SendDB * s = (SendDB *) lpParamter;
	if (s->call != NULL)	s->call(s->lp,s->recvBuf+1,s->recvLen-1);
	delete s;
}
SocketBuffer::SocketBuffer(const TCHAR * Base){
	TCHAR Tip[16]={0};
	host = new char[16];
	GetPrivateProfileString(_T("Server"),_T("ip"),_T("192.168.1.50"),Tip ,16,Base);
	int len = WideCharToMultiByte(CP_ACP,0,Tip,-1,NULL,0,NULL,NULL);  
	WideCharToMultiByte(CP_ACP,0,Tip,-1,host,len,NULL,NULL ); 

	port = GetPrivateProfileInt(_T("Server"),_T("port"),3333,Base);
	HeartTime = GetPrivateProfileInt(_T("Server"),_T("heartTime"),100,Base);

	keyID = -1;
	this->reloadSystem = NULL;
	this->reloadSystemInfo = NULL;
}
SocketBuffer::SocketBuffer(char * _host,const int _port,const int heart,CallBackFun _reloadSystem,LPVOID _reloadSystemInfo){
	//this->socketClient = new SocketClient(host,port);
	this->HeartTime = heart;
	this->host = _host;
	this->port = _port;
	keyID = -1;
	this->SetReloadSystem(_reloadSystem,_reloadSystemInfo);

	start();

   	
}
void SocketBuffer::start(){
	//this->SetKey();
	this->clock_end = clock();
	this->clock_begin = clock_end;

	HANDLE hThread  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)runThread, this, 0, NULL);
	CloseHandle(hThread);
}
SocketBuffer::~SocketBuffer(){
	//delete host;
	for (vector<SendDB*>::iterator it = Sendlist.begin(); it != Sendlist.end(); it ++) {
		delete (*it);
		*it = NULL;
	}
	this->Sendlist.clear();
	delete host;
}
void SocketBuffer::SetReloadSystem(CallBackFun _reloadSystem ,LPVOID _reloadSystemInfo ){
	this->reloadSystem = _reloadSystem;
	this->reloadSystemInfo = _reloadSystemInfo;
}
void SocketBuffer::ConnServer(){

	//this->socketClient = NULL;
	try{
		this->socketClient = new SocketClient(host,port);
	}catch (...) {
		Sleep(this->HeartTime);
		printf("conn %s %d\r\n",host,port);
		//delete this->socketClient;
		this->ConnServer();
	}
}

void runThread(LPVOID lpParamter){
	SocketBuffer * s = (SocketBuffer *) lpParamter;   
	s->ConnServer();	
	while(true){
		s->syncSend();
	}
}
void SocketBuffer::syncSend(){
	if (Sendlist.empty()){
		long timeOut = TimeOut;
		if (this->clock_end == this->clock_begin){
			timeOut /=2;
		}
		if ((clock()-this->clock_end) > timeOut){
			//printf("reloadSystem____");
			if (reloadSystem != NULL){
				//LoadSendData(this,"-10",reloadSystem,reloadSystemInfo);
				
				char D[1] = {-1};
				int len = 1;
				char *Data = this->GetSendBit(D,len);
				bool Wait = true;
				SendDB * db = new SendDB(Data,len,&Wait,reloadSystem,reloadSystemInfo);
				//SendDB *sdb =new SendDB(_call, _lp,Data,&Wait);	
				this->Sendlist.push_back(db);

			}
			this->clock_end =clock();
		}
		Sleep(this->HeartTime);
		//printf("%d %d\r\n",clock()-this->clock_end,timeOut);
	}else{
		SendDB * db = Sendlist.front();
		sendData(db->sendBuf,db->sendLen);
		recvData(db);

		Sendlist.erase(Sendlist.begin());
		//this->WaitWork.push_back(db);
		//delete db;
	}
}
void SocketBuffer::sendData(const char * data,const int len){
	//printf("send %s\r\n",data);
	 
	int ret =	this->socketClient->SendBytes(data,len);
	if (ret == SOCKET_ERROR)  {
		//Sleep(this->HeartTime);
		printf("send err,conn %s %d\r\n",host,port);
		delete this->socketClient;
		this->ConnServer();
		this->sendData(data,len);
	}
}
void SocketBuffer::recvData(SendDB * db){
	db->recvBuf = new char[1024];	
	db->recvLen = this->socketClient->ReceiveBytes(db->recvBuf,1024);		
	if (db->recvLen == SOCKET_ERROR)  {
		printf("recv err,conn %s %d\r\n",host,port);
		delete this->socketClient;
		this->ConnServer();
		delete db;
		return;
	}
	updateKeyId(db);
	if (db->call != NULL){
		HANDLE hThread  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)syncWork, db, 0, NULL);
		CloseHandle(hThread);
	}else{
		delete db;
	}
	/**
	WaitWait.push_back()
	if (db->call!= NULL){
		db->call(db->lp,recvBuf,db->Wait);
	}
	**/

}

void LoadSendData(LPVOID lpParamter,const char * data,int len,CallBackFun _call,LPVOID _lp){
	if (lpParamter == NULL) return;
	SocketBuffer * c = (SocketBuffer *) lpParamter; 
	char *Data = c->GetSendBit(data,len);
	bool Wait = true;	
	SendDB *sdb =new SendDB(Data,len,&Wait,_call, _lp);	
	if (_call!=NULL){
		c->clock_end = clock();
	}
	c->Sendlist.push_back(sdb);
	//if (_call!=NULL){
	while(Wait){
		Sleep(c->HeartTime);
	}
	//}
}
void SocketBuffer::updateKeyId(SendDB * db){
	if (db->recvLen>0){
		if (db->recvBuf[0] != this->keyID) this->keyID = db->recvBuf[0];
		//db->recvLen--;
		//db->recvBuf++;
	}
}
char * SocketBuffer::GetSendBit(const char * data,int & len){
	int nlen = len+1;
	char * out = new char[nlen];
	//out[0]=0;
	out[0]=this->keyID;
	memcpy(out+1,data,len);
	len = nlen;
	return out;
}
