// UsMon.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>  
#include <direct.h>  
#include <stdlib.h>  
#include <memory>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include "process.h"
#include "Psapi.h"
#include <wincrypt.h>
#include "Objbase.h"
#include "Shlwapi.h"
#include "winbase.h"
#include "wininet.h"
#include "winuser.h"
//#include "FileAPI.h" 
#include <sstream>
#include <windows.h>
#include <stdio.h>
#include "time.h"
#include "string.h"
#include "iostream"
#include "fstream"
#include "tinyxml.h"

using namespace std;
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Psapi.lib")
string dirpath;
char start32[1000];
char stop32[1000];
char start64[1000];
char stop64[1000];
char dat[100];
int filesize=50;//��־�ļ��и�Ĵ�С
char g_filepath[1000]=""; //ԭ��־·��
BOOL g_fileWritingFlag = FALSE;
BOOL g_fileMoveFlag = FALSE;
char logdir[1000]=""; //�з���־��Ŀ¼
char hostname[128]="";
char *spynum="000";
char * GetDate(){
	time_t t=time(NULL);
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	sprintf_s(dat,"%4d%02d%02d",sys.wYear,sys.wMonth,sys.wDay);
	return dat;
}

//������־����
char str[100];
char * LogName()
{
	time_t t=time(NULL);
	SYSTEMTIME sys;
	GetLocalTime( &sys );
	DWORD pid=(DWORD)_getpid();
	sprintf_s(str,"%4d%02d%02d%02d%02d%02d%d",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	return str;
}
char ch[200];
//char ch1[100]="C:\\Log\\usermode\\";
char fold[1000];
int MoveLog()
{
	struct _stat info;
	//char* filepath="C:\\Log\\usermode\\Log.txt";
	_stat(g_filepath,&info);
	int size = info.st_size;
	double KBsize=size/1024;
	if (KBsize>filesize)
	{
		//sprintf_s(fold,"%s%s",dirpath,GetDate());
		sprintf_s(fold,"%s",logdir);
		char exe1[300];
		sprintf_s(exe1,"md %s\0",fold);
		cout<<exe1<<endl;
		//system(exe1);
		//_mkdir(fold);
		char exe2[300]="";
		char srcpath[1000]="";
		char distpath[1000]="";
		sprintf_s(srcpath,"%s",g_filepath);
		sprintf_s(distpath,"%s\\%s_000_%s",fold,hostname,LogName());
		//rename("C:\\Log\\usermode\\Log.txt","C:\\Log\\usermode\\Log1.txt");
		//sprintf_s(exe2,"move \"%s\" %s\\%s-000-%s",filepath,fold,hostname,LogName());
		cout<<exe2<<endl;
		cout<<LogName()<<endl;
		//system(exe2);
		//Ҫ���ȵ�һ��
		//��ط���while���׳�������
		//while(!g_fileWritingFlag);
		g_fileMoveFlag = TRUE;
		MoveFileA(srcpath,distpath);
		g_fileMoveFlag = FALSE;
	}
	return 0;
}

void Prepare(){
	char exepath[MAX_PATH];
	GetModuleFileNameA(NULL,exepath,MAX_PATH);
	string str(exepath);
	int end=strlen(exepath)-9;
	dirpath=str.substr(0,end);
	sprintf_s(start32,"%sMonInject32.exe",dirpath.c_str());
	sprintf_s(stop32,"%sUnloadDll32.exe",dirpath.c_str());
	sprintf_s(start64,"%sMonInject64.exe",dirpath.c_str());
	sprintf_s(stop64,"%sUnloadDll64.exe",dirpath.c_str());
	//����XML��ԭ��־·��filepath��Ŀ����־Ŀ¼logdir����־��Сfilesize
	char configpath[1000]={0};//start.xml��·��
    sprintf_s(configpath,"%sstart.xml",dirpath.c_str());
	sprintf_s(g_filepath,"%sLog.txt",dirpath.c_str());//32λ��־
	TiXmlDocument doc;
	if(!doc.LoadFile(configpath)) 
	{
		//MessageBoxA(NULL,configpath,"��ʾ",MB_OK);
		MessageBoxW(NULL,L"�Ҳ��������ļ��������˳�",L"��ʾ",MB_OK);
		ExitProcess(1);
		//ftest<<doc.ErrorDesc()<<endl;
	}
	//TiXmlDocument doc;
	TiXmlElement* root = doc.FirstChildElement();
	TiXmlElement* elem = root->FirstChildElement();
	TiXmlElement* e1=elem->FirstChildElement("KBsize");
	filesize=atoi(e1->FirstChild()->ToText()->Value());//��ȡ�зִ�С
	//printf("%d\n",filesize);
	TiXmlElement* elem1 = elem->FirstChildElement("TempLogPath");
	strcpy_s(logdir,elem1->FirstChild()->ToText()->Value());//��ȡ���ŵ���־Ŀ¼·��
	char mkdir[100];
	sprintf_s(mkdir,"md %s",logdir);
	//cout<<mkdir<<endl;
	system(mkdir);
	WSAData wsaData;
	WSAStartup(MAKEWORD(1,1), &wsaData); //!!!<-����������� 
	gethostname(hostname,128);
	//cout<<"ddddd"<<endl;
	//printf("%s\n",logdir);
}

DWORD WINAPI WindowThread(
	_In_ LPVOID lpParameter
	)
{
#define  MAX_BUF_LEN  10240
	char MaxBuf[MAX_BUF_LEN]={0};
	UINT bufLen = 0;
	LPTSTR strInjectMailSlot = TEXT("\\\\.\\mailslot\\inject_server_mailslot");
	HANDLE hSlot= CreateMailslot(  
		strInjectMailSlot,       // mailslot ��  
		0,                          // ��������Ϣ��С   
		MAILSLOT_WAIT_FOREVER,         // �޳�ʱ   
		NULL);  
	if (hSlot == INVALID_HANDLE_VALUE)
		return 0;

	BOOL bResult; 
	DWORD cbMessage, cMessage, cbRead,cbWrite,cAllMessages;
	while(1)  
	{  
		// ��ȡmailslot��Ϣ  
		bResult = GetMailslotInfo(hSlot, // mailslot ���   
			(LPDWORD) NULL,               // �������Ϣ����  
			&cbMessage,                   // ��һ����Ϣ�Ĵ�С  
			&cMessage,                    // ��Ϣ������  
			(LPDWORD) NULL);           // ��ʱ��  
		if (!bResult)   
		{   
			MessageBoxA(NULL,"���������ʧ��","��ʾ",MB_OK);
			CloseHandle(hSlot);
			return 0;   
		}else{
			//MessageBoxA(NULL,"��������۳ɹ�","��ʾ",MB_OK);
		}    
		printf("MAILSLOT_NO_MESSAGE \n");  
		if (cbMessage == MAILSLOT_NO_MESSAGE)   
		{   
			// û����Ϣ����һ��ʱ����ȥ��  
			Sleep(1000);  
			//Sleep(3000);
			continue;  
		}
		cAllMessages = cMessage;   
		while (cMessage != 0)  // ��ȡȫ����Ϣ���п��ܲ�ֻһ��  
		{   
			char lpszBuffer[4096]={0};
			// ��ȡ��Ϣ   
			if(!ReadFile(hSlot,lpszBuffer,cbMessage,&cbRead,NULL))  
			{   
				printf("ReadFile failed with %d.\n", GetLastError());    
				break;   
			}
			else
			{
				//�������ֵǰд��
				if(bufLen+cbRead >= MAX_BUF_LEN)
				{
					if (g_fileMoveFlag)
					{
						//���������Ҫ����д�������ط�֮��
						Sleep(300);  
					}
					g_fileWritingFlag = TRUE;
					ofstream f(g_filepath,ios::app);
					f<<MaxBuf;
					f.close();
					g_fileWritingFlag = FALSE;
					MaxBuf[0]=0;
					bufLen=0;
				}
				//���ﰴ��˵Ӧ����memcpy
				sprintf_s(MaxBuf,"%s%s",MaxBuf,lpszBuffer);
				bufLen+=cbRead;
			}

			bResult = GetMailslotInfo(hSlot,NULL,&cbMessage,&cMessage,NULL); // ����ʣ�����Ϣ��,��cMessage=0�����˳���ѭ��  
			if (!bResult)   
			{   
				printf("GetMailslotInfo failed (%d)\n", GetLastError());  
				break;
			}   
		}   
	}   
}



//int _tmain(int argc, CHAR* argv[])
int CALLBACK WinMain(
_In_ HINSTANCE hInstance,
_In_ HINSTANCE hPrevInstance,
_In_ LPSTR     lpCmdLine,
_In_ int       nCmdShow
)
{
	Prepare();
	if (__argc==1)
	{
		//������һ����������
		HANDLE hMutex = CreateMutexW(NULL,false,L"UsMon_Siglon"); 
		if (hMutex!= INVALID_HANDLE_VALUE && GetLastError() == ERROR_ALREADY_EXISTS) 
		{
			 CloseHandle(hMutex);
			 ExitProcess(0);
		}

		OutputDebugStringA("this is flag 1");
		HANDLE handle =	CreateThread(NULL,0,WindowThread,NULL,0,NULL);
		//˵����һ�δ���
		if(WAIT_TIMEOUT==WaitForSingleObject(handle, 1000))
		{
			
		}
		WinExec(start32,SW_HIDE);
		WinExec(start64,SW_HIDE);
		while (true)
		{

			Sleep(1000);
			MoveLog();
		}

	}
	else if (__argc==2)
	{
		HANDLE handle =	CreateThread(NULL,0,WindowThread,NULL,0,NULL);
		//˵����һ�δ���
		if(handle == NULL )
		{
			OutputDebugStringA("handle failed");
		}
		if(WAIT_TIMEOUT==WaitForSingleObject(handle, 1000))
		{
			
		}
		if (strcmp("start",__argv[1])==0)
		{
			WinExec(start32,SW_HIDE);
			WinExec(start64,SW_HIDE);

		}
		if (strcmp("stop",__argv[1])==0)
		{
			WinExec("taskkill /im MonInject32.exe /f",SW_HIDE);
			WinExec("taskkill /im MonInject64.exe /f",SW_HIDE);
			WinExec(stop32,SW_HIDE);
			WinExec(stop64,SW_HIDE);
		}
		if (strcmp("restart",__argv[1])==0)
		{
			WinExec("taskkill /im MonInject32.exe /f",SW_HIDE);
			WinExec("taskkill /im MonInject64.exe /f",SW_HIDE);
			char configpath1[1000];
			char configpath2[1000];
			sprintf_s(configpath1,"%sconfig.xml",dirpath.c_str());
			sprintf_s(configpath2,"%supdate.xml",dirpath.c_str());
			DeleteFileA(configpath1);
			MoveFileA(configpath2,configpath1);
			WinExec(stop32,SW_HIDE);
			WinExec(stop64,SW_HIDE);
			WinExec(start32,SW_HIDE);
			WinExec(start64,SW_HIDE);

		}
		while (true)
		{
			Sleep(1000);
			MoveLog();
		}
	}
	return 0;
}

