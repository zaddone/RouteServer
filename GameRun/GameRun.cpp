// GameRun.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SendBufferBit.h"
#include "lobby.h"


void debug(Lobby *lo,SocketBuffer * Client){
	char *str = new char[100];
	int n = 0;
	while(true){
		n++;
		std::cout << "Please, enter: ";
		std::cin >> str;
		if (strstr(str , "screen")>0){
			while(true){
				Sleep(1000);
				lo->ScreenSave();
			}
		}else if (strstr(str , "train")>0){
			lo->trainBlock(train);
		}else if (strstr(str , "test")>0){
			/**
			int len = 10;
			char *in = new char[10];
			for (int i=0;i<10;i++){
				in[i]=i;
				printf("%d ",in[i]);
			}
			printf("\r\n");
			//char *out = new char[12];
			char *out = Client->GetSendBit(in,len);
			for (int i=0;i<len;i++){
				printf("%d ",out[i]);
			}
			LoadSendData(Client)
			delete [] in;
			delete [] out;
			**/
		}else if (strstr(str , "run")>0){
			break;
		}
	}
	delete [] str;
}

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR * base = _T(".\\base.ini");
	SocketBuffer * Client = new SocketBuffer(base);
	Lobby *lo  =new Lobby(base,LoadSendData,Client);
	
	Client->start();
	if (argc >1){
		//size_t le = wcslen(debug);
		_wcslwr_s(argv[1],wcslen(argv[1])+1);
		if (wcsstr(L"debug",argv[1])){
			debug(lo,Client);
			//return 0;
		}
	}
	Client->SetReloadSystem(reloadServer,lo);
	lo->go();
	return 0;
}

