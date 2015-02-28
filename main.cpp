#include <iostream>

#include <string>
#include "buf.h"
#include "MsgHead.h"
#include "Gen.h"
#ifdef WIN32
#include <winsock.h>
#pragma comment (lib, "ws2_32.lib")
#endif

using namespace std;
#define Z_M_DST_SLEE	0x35
#define  Z_FAILED(fun)		((fun)<0)
#define Z_M_DST_OSS	0x40
#define Z_M_DST_COMMAND      0x20
#define ZG_EVT_OSS_COMMAND   0x21
// OSS event
// OSS->SLEE, total 1
#define Z_EVT_OSS_SLEE_BASE				0x0600
#define Z_EVT_OSS_SLEE_SESTIMERSP		Z_EVT_OSS_SLEE_BASE+0x0001
// SLEE->OSS
#define Z_EVT_SLEE_OSS_BASE				0x0380
#define Z_EVT_SLEE_OSS_SESTIMEREQ		Z_EVT_SLEE_OSS_BASE+0x0001
#define Z_EVT_SLEE_OSS_SESREL			Z_EVT_SLEE_OSS_BASE+0x0002

int Rec_Mes()
{	
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	int service=0;
	service=socket(AF_INET,SOCK_STREAM,0);
	if (service==-1)
	{
		cout<<"Create the socket error"<<endl;
	}
	int Port_ADD;
	cout<<"Please input the Port "<<endl;
	cin>>Port_ADD;
	struct sockaddr_in IP_TO;
	IP_TO.sin_family=AF_INET;
	IP_TO.sin_addr.S_un.S_addr=INADDR_ANY;//inet_addr("10.94.20.114");
	IP_TO.sin_port=htons(Port_ADD);
//	if(connect(service,(sockaddr*) &IP_TO,sizeof(IP_TO))==-1)cout<<"connect error"<<endl;
	if(bind(service,(sockaddr*) &IP_TO,sizeof(IP_TO))==-1)
	cout<<"Bind the port error"<<endl;
	cout<<"waiting for connect"<<endl;
	if (listen(service,20)==-1)cout<<"listen error"<<endl;
    struct sockaddr_in there_add;
	int s=sizeof(struct sockaddr_in);
 	int new_ser;
	if((new_ser=accept(service,(sockaddr*)&there_add,&s))==-1)cout<<"accept error"<<endl;
    cout<<inet_ntoa(there_add.sin_addr)<<":"<<ntohs(there_add.sin_port)<<endl;
  while(true)
	{
	char pBuf[1000];
   	int iRet = recv(new_ser, pBuf,1000, 0);
    if (iRet==-1)
    {   
		cout<<"rec error"<<endl;
        cout<<WSAGetLastError();
        continue;
	}
    pBuf[iRet]='\0';
	if (pBuf[0]=='$')
	{   
		char * cmd=new char[100];
		/*strncpy(cmd,pBuf,iRet-1);*/
        cmd=&pBuf[1];
		cmd[iRet-1]='\0';
		cout<<"Run command "<<cmd<<endl;
		system(cmd);
	}
	if (pBuf[iRet-1]=='@')
	{
		exit(1);
	}
	printf("the message=%s\n",pBuf); 
	*pBuf=NULL;
	}
	return 0;
}  



int Send_Mes()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	int service=0;
	service=socket(AF_INET,SOCK_STREAM,0);
	if (service==-1)
	{
		cout<<"Create the socket error"<<endl;
	}
	cout<<"Please input the IP anf port"<<endl;
    char * IP_ADD=new char[100];
	int Port_ADD;
	cin>>IP_ADD;
	cin>>Port_ADD;
	struct sockaddr_in IP_TO;
	IP_TO.sin_family=AF_INET;
	IP_TO.sin_addr.S_un.S_addr=inet_addr(IP_ADD);
	IP_TO.sin_port=htons(Port_ADD);
	if(connect(service,(sockaddr*) &IP_TO,sizeof(IP_TO))==-1)cout<<"connect error"<<endl;
    while (true)
    {	
		cout<<"Please input the message"<<endl;
		char * message=new char[1000];
	    cin.getline(message,1000,'\n');
		if (send(service,message,strlen(message),0)==-1)cout<<"Send to"<<IP_ADD<<":"<<Port_ADD<<"error"<<endl;
    }
    return 0;
}  

int U_Send_Mes()
{	
WSADATA wsa;
WSAStartup(MAKEWORD(2,2),&wsa);
int service=0;
service=socket(AF_INET,SOCK_DGRAM,0);
if (service==-1)
{
	cout<<"Create the socket error"<<endl;
}
cout<<"Please input the IP anf port"<<endl;
char * IP_ADD=new char[100];
int Port_ADD;
cin>>IP_ADD;
cin>>Port_ADD;
struct sockaddr_in IP_TO;
IP_TO.sin_family=AF_INET;
IP_TO.sin_addr.S_un.S_addr=inet_addr(IP_ADD);
IP_TO.sin_port=htons(Port_ADD);
while (true)
{	
	cout<<"Please input the message"<<endl;
	char * message=new char[1000];
	cin.getline(message,1000,'\n');
	if (sendto(service,message,strlen(message),0,(sockaddr*) &IP_TO,sizeof(IP_TO))==-1)
		cout<<"Send to"<<IP_ADD<<":"<<Port_ADD<<"error"<<endl;
}
return 0;
}  

int U_Rec_Mes()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	int service=0;
	service=socket(AF_INET,SOCK_DGRAM,0);
	if (service==-1)
	{
		cout<<"Create the socket error"<<endl;
	}
	int Port_ADD;
	cout<<"Please input the Port "<<endl;
	cin>>Port_ADD;
	struct sockaddr_in IP_TO;
	IP_TO.sin_family=AF_INET;
	IP_TO.sin_addr.S_un.S_addr=INADDR_ANY;//inet_addr("10.94.20.114");
	IP_TO.sin_port=htons(Port_ADD);
	//	if(connect(service,(sockaddr*) &IP_TO,sizeof(IP_TO))==-1)cout<<"connect error"<<endl;
	if(bind(service,(sockaddr*) &IP_TO,sizeof(IP_TO))==-1)
		cout<<"Bind the port error"<<endl;
	while(true)
	{
		sockaddr_in stAddr = {0};
        int iAddrLen = sizeof(stAddr);
		char pBuf[1000];
		int iRet = recvfrom(service, pBuf,1000, 0,(sockaddr*) &stAddr, &iAddrLen);
		if (iRet==-1)
		{   
			cout<<"rec error"<<endl;
			cout<<WSAGetLastError();
			continue;
		}
		cout<<inet_ntoa(stAddr.sin_addr)<<':'<<ntohs(stAddr.sin_port)<<endl;
		pBuf[iRet]='\0';
		if (pBuf[iRet-1]=='$')
		{   
			char * cmd=new char[100];
			strncpy(cmd,pBuf,iRet-1);
			cmd[iRet-1]='\0';
			cout<<"Run command "<<cmd<<endl;
			system(cmd);
		}
		if (pBuf[iRet-1]=='@')
		{
			exit(1);
		}
		printf("the message=%s\n",pBuf); 
		*pBuf=NULL;
	}
	return 0;
}

int Send_Oss()
{	
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	int service=0;
	service=socket(AF_INET,SOCK_DGRAM,0);
	if (service==-1)
	{
		cout<<"Create the socket error"<<endl;
	}
	cout<<"Please input the IP anf port"<<endl;
	char * IP_ADD=new char[100];
	int Port_ADD;
	cin>>IP_ADD;
	cin>>Port_ADD;
	struct sockaddr_in IP_TO;
	IP_TO.sin_family=AF_INET;
	IP_TO.sin_addr.S_un.S_addr=inet_addr(IP_ADD);
	IP_TO.sin_port=htons(Port_ADD);
	while (true)
	{   	
		ZMsgHead* mBuf=new ZMsgHead;
	    string callerCountry,callerArea,caller,callerSrvCode, calleeCountry,calleeArea,callee,calleeSrvCode, startTime,endTime;
		guint32 calleeType;
		UINT SuId,SpId;
		
		cout<<"Please input the message"<<endl;
        cout<<"SuId:"<<endl;
		cin>>SuId;
		cout<<"Spid:"<<endl;
        cin>>SpId;
		cout<<"callerCountry"<<endl;
		cin>>callerCountry;
		cout<<"callerArea"<<endl;
	    cin>>callerArea;
		cout<<"caller"<<endl;
		cin>>caller;
		cout<<"callerSrvCode"<<endl;
		cin>>callerSrvCode;
		cout<<"calleeType"<<endl;
		cin>>calleeType;
		cout<<"calleeCountry"<<endl;
		cin>>calleeCountry;
		cout<<"calleeArea"<<endl;
		cin>>calleeArea;
		cout<<"callee"<<endl;
		cin>>callee;
		
		Z_CHECK_FUN(mBuf->PkPost(Z_M_DST_SLEE, Z_M_DST_OSS, Z_EVT_SLEE_OSS_SESTIMEREQ), -1);
        if (
	        Z_FAILED(mBuf->PkData(SuId)) ||
			Z_FAILED(mBuf->PkData(SpId)) ||
			Z_FAILED(mBuf->PkData(callerCountry)) ||
			Z_FAILED(mBuf->PkData(callerArea)) ||
			Z_FAILED(mBuf->PkData(caller)) ||
			Z_FAILED(mBuf->PkData(callerSrvCode)) ||
			Z_FAILED(mBuf->PkData(calleeType)) ||
			Z_FAILED(mBuf->PkData(calleeCountry)) ||
			Z_FAILED(mBuf->PkData(calleeArea)) ||
			Z_FAILED(mBuf->PkData(callee)))
			{
				cout<<"PkDATA error"<<endl;
			}
		  char * message=NULL;
         message= (CHAR *)(mBuf->Get_Mp()+mBuf->Get_Ms());
//         if (mBuf->Get_Ml()!=(mBuf->Get_Ms()-mBuf->Get_Me()))
//         {
// 			cout<<"len error"<<endl;
//         }
		if (sendto(service,message,(mBuf->Get_Me()-mBuf->Get_Ms()),0,(sockaddr*) &IP_TO,sizeof(IP_TO))==-1)
			cout<<"Send to"<<IP_ADD<<":"<<Port_ADD<<"error"<<endl;
	}
	return 0;
}  
int Command_Oss()
{	
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	int service=0;
	service=socket(AF_INET,SOCK_DGRAM,0);
	if (service==-1)
	{
		cout<<"Create the socket error"<<endl;
	}
	cout<<"Please input the IP anf port"<<endl;
	char * IP_ADD=new char[100];
	int Port_ADD;
	cin>>IP_ADD;
	cin>>Port_ADD;
	struct sockaddr_in IP_TO;
	IP_TO.sin_family=AF_INET;
	IP_TO.sin_addr.S_un.S_addr=inet_addr(IP_ADD);
	IP_TO.sin_port=htons(Port_ADD);
	while (true)
	{   	
		ZMsgHead* mBuf=new ZMsgHead;
		char * command=new char[1000];
		cout<<"Please input the command"<<endl;
		cin.getline(command,1000,'\n');


		Z_CHECK_FUN(mBuf->PkPost(Z_M_DST_COMMAND, Z_M_DST_OSS, ZG_EVT_OSS_COMMAND), -1);
		if (Z_FAILED(mBuf->PkData(command)))

		{
			cout<<"PkDATA error"<<endl;
		}
		char * message=NULL;
		message= (CHAR *)(mBuf->Get_Mp()+mBuf->Get_Ms());
		//         if (mBuf->Get_Ml()!=(mBuf->Get_Ms()-mBuf->Get_Me()))
		//         {
		// 			cout<<"len error"<<endl;
		//         }
		if (sendto(service,message,(mBuf->Get_Me()-mBuf->Get_Ms()),0,(sockaddr*) &IP_TO,sizeof(IP_TO))==-1)
			cout<<"Send to"<<IP_ADD<<":"<<Port_ADD<<"error"<<endl;
	}
	return 0;
}  
int main()
{
	int sel;
	cout<<"   ^_^  send and receive message by zw  ^_^"<<endl;
	cout<<"    1. TCP_send    message"<<endl;
	cout<<"    2. UDP_send    message"<<endl;
    cout<<"    3. TCP_receive message"<<endl;
    cout<<"    4. UDP_receive message"<<endl;
	cout<<"    5. Oss_Sent    message"<<endl;
	cout<<"    6. Oss_Command message"<<endl;
	cout<<"Please input your selection:";
	cin>>sel;
	if (sel==1)
	{
	 system("cls");
	 Send_Mes();
	}
     else if (sel==2)
     {
		 system("cls");
         U_Send_Mes();
     }
	 else if(sel==3)
	 {
	   system("cls");
	   Rec_Mes();
	 }
	 else if(sel==4)
	 {
       system("cls");
	   U_Rec_Mes();
	 }
	 else if(sel==5)
	 {
		 system("cls");
	     Send_Oss();
	 }
	 else if(sel==6)
	 {
		 system("cls");
		 Command_Oss();
	 }
return 0;
}
