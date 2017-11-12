#include "stdafx.h"
#include "SendBuffer.h"


SendDB::SendDB(CallBackFun _call,LPVOID _lp,char * _data,bool * w){
	this->lp = _lp;
	this->data = _data;
	this->call = _call;
	this->Wait = w;
	this->recvBuf = NULL;
}
SendDB::~SendDB(){
	if (this->Wait != NULL){
		this->Wait[0] = false;
	}
	//printf("delete %s\r\n",data);
	if (data!= NULL) delete [] this->data;
	if (recvBuf!= NULL)	delete [] recvBuf;
	
}
void syncWork(LPVOID lpParamter){
	SendDB * s = (SendDB *) lpParamter;
	if (s->call != NULL)	s->call(s->lp,s->recvBuf);
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

}
SocketBuffer::SocketBuffer(char * _host,const int _port,const int heart,CallBackFun _reloadSystem,LPVOID _reloadSystemInfo){
	//this->socketClient = new SocketClient(host,port);
	this->HeartTime = heart;
	this->host = _host;
	this->port = _port;
	
	this->SetReloadSystem(_reloadSystem,_reloadSystemInfo);

	start();

   	
}
void SocketBuffer::start(){
	this->SetKey();
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
				
				char *Data = new char[1024];
				this->GetSendDB("-10",Data);
				bool Wait = true;
				SendDB * db = new SendDB(reloadSystem,reloadSystemInfo,Data,&Wait);
				//SendDB *sdb =new SendDB(_call, _lp,Data,&Wait);	
				this->Sendlist.push_back(db);

			}
			this->clock_end =clock();
		}
		Sleep(this->HeartTime);
		//printf("%d %d\r\n",clock()-this->clock_end,timeOut);
	}else{

		SendDB * db = Sendlist.front();
		sendData(db->data);
		recvData(db);

		Sendlist.erase(Sendlist.begin());
		//this->WaitWork.push_back(db);
		//delete db;
	}
}
void SocketBuffer::sendData(const char * data){
	printf("send %s\r\n",data);
	try{
		this->socketClient->SendBytes(data);
	}catch (...) {
		//Sleep(this->HeartTime);
		printf("send err,conn %s %d\r\n",host,port);
		delete this->socketClient;
		this->ConnServer();
		this->sendData(data);
	}
}
void SocketBuffer::recvData(SendDB * db){
	db->recvBuf = new char[1024];  
	try{
		this->socketClient->ReceiveBytes(db->recvBuf,1024);
	}catch (...) {
		printf("recv err,conn %s %d\r\n",host,port);
		delete this->socketClient;
		this->ConnServer();
		return;
	}

	HANDLE hThread  = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)syncWork, db, 0, NULL);
	CloseHandle(hThread);
	/**
	WaitWait.push_back()
	if (db->call!= NULL){
		db->call(db->lp,recvBuf,db->Wait);
	}
	**/

}
void LoadSendData(LPVOID lpParamter,const char * data,CallBackFun _call,LPVOID _lp){
	if (lpParamter == NULL) return;
	SocketBuffer * c = (SocketBuffer *) lpParamter; 
	char *Data = new char[1024];
	c->GetSendDB(data,Data);
	bool Wait = true;
	
	SendDB *sdb =new SendDB(_call, _lp,Data,&Wait);	
	if (_call!=NULL){
		c->clock_end = clock();
		printf("change clock end %d\r\n",c->clock_end);
		
	}
	c->Sendlist.push_back(sdb);
	while(Wait){
		//printf("wait %s\r\n",Data);
		Sleep(c->HeartTime);
	}
}
void SocketBuffer::GetSendDB(const char * data,char * outData){
	sprintf(outData,"%s %s\0",this->Key,data);
}
void SocketBuffer::SetKey(){
	srand((unsigned)time(NULL));
	for (int i=0;i<5;i++){
		Key[i] = rander[random(63)];
	}
	Key[5]='\0';
}