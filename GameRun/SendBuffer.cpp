#include "stdafx.h"
#include "SendBuffer.h"


SendDB::SendDB(CallBackFun _call,LPVOID _lp,char * _data,bool * w){
	this->lp = _lp;
	this->data = _data;
	this->call = _call;
	this->Wait = w;
}
SendDB::~SendDB(){
	if (this->call  == NULL){
		this->Wait = false;
	}
	delete [] this->data;
}

SocketBuffer::SocketBuffer(char * _host,const int _port,const int heart,CallBackFun _reloadSystem,LPVOID _reloadSystemInfo){
	//this->socketClient = new SocketClient(host,port);
	this->HeartTime = heart;
	this->host = _host;
	this->port = _port;
	this->SetKey();
	this->reloadSystem = _reloadSystem;
	this->reloadSystemInfo = _reloadSystemInfo;
	//this->socketClient = NULL;

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
	s->syncSend();
}
void SocketBuffer::syncSend(){
	if (Sendlist.empty()){
		long timeOut = TimeOut;
		if (this->clock_end == this->clock_begin){
			timeOut /=2;
		}
		if ((clock()-this->clock_end) > timeOut){
			LoadSendData(this,"-10",reloadSystem,reloadSystemInfo);
		}
		Sleep(this->HeartTime);
	}

	SendDB * db = Sendlist.front();
	sendData(db->data);
	recvData(db);

	Sendlist.erase(Sendlist.begin());
	delete db;
}
void SocketBuffer::sendData(const char * data){
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
	char recvBuf[1024] = {0};  
	try{
		this->socketClient->ReceiveBytes(recvBuf,1024);
	}catch (...) {
		printf("recv err,conn %s %d\r\n",host,port);
		delete this->socketClient;
		this->ConnServer();
		return;
	}
	if (db->call!= NULL){			
		db->call(db->lp,recvBuf,db->Wait);
	}

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
	}
	c->Sendlist.push_back(sdb);
	while(Wait){
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