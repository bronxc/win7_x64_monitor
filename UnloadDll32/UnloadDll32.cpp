// UnloadDll32.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include "TLHELP32.H"
#include "fstream"
#include <Psapi.h>
#include <process.h>
#pragma comment(lib,"version.lib")
#pragma comment(lib, "Psapi.lib")
#include "fstream"
#include <time.h>
using namespace std;

char status[MAX_PATH];

void UnInjectDll(const char *szDllName,const DWORD dwPid){
	BOOL flag = FALSE;           
	if ( dwPid == 0 || strlen(szDllName) == 0 )  
	{                   
		return;  
	}           
	//��ȡϵͳ���н��̡��̵߳ȵ��б�  
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);           
	MODULEENTRY32 Me32 = { 0 };  
	Me32.dwSize = sizeof(MODULEENTRY32);           
	//���������������ĵ�һ��ģ�����Ϣ  
	BOOL bRet = Module32First(hSnap, &Me32);           
	while ( bRet )  
	{         
		//������ע���DLL  
		string ws=Me32.szModule;
		string str="NULL";
		str=ws;
		if ( strcmp(str.c_str(), szDllName) == 0 )                   
		{  
			flag = TRUE;                           break;  
		}                   //������һ��ģ����Ϣ  
		bRet = Module32Next(hSnap, &Me32);           
	}  
	if (flag == FALSE)           
	{  
	//	printf("�Ҳ�����Ӧ��ģ��!");     
		return;  
	}     
	CloseHandle(hSnap);     
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);           
	if ( hProcess == NULL )  
	{                   
	//	printf("���̴�ʧ��!");  
		return ;           
	}  
	FARPROC pFunAddr = GetProcAddress(GetModuleHandleA("kernel32.dll"),"FreeLibrary");  
	//cout<<dwPid<<endl;
	HANDLE hThread = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)pFunAddr,Me32.hModule,0,NULL);  
	//cout<<dwPid<<endl;
	if (hThread == NULL)           {  
	//	printf("����Զ���߳�ʧ��!");                  
		return;  
	}         
	//AfxMessageBox("�ɹ�ж��!");           //�ȴ��߳��˳�  
	//WaitForSingleObject(hThread, INFINITE);         
	CloseHandle(hThread);           
	CloseHandle(hProcess);  
}

string dirpath;



string WideToMutilByte(const wstring& _src)
{
	if (&_src==NULL)
	{
		return "NULL";
	}
	int nBufSize = WideCharToMultiByte(GetACP(), 0, _src.c_str(),-1, NULL, 0, NULL, NULL);
	char *szBuf = new char[nBufSize];
	WideCharToMultiByte(GetACP(), 0, _src.c_str(),-1, szBuf, nBufSize, NULL, NULL);
	string strRet(szBuf);
	delete []szBuf;
	szBuf = NULL;
	return strRet;
}

int EnableDebugPriv(const char* name)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//�򿪽������ƻ�
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);
	//��ý��̱���ΨһID
	if(!LookupPrivilegeValueA(NULL,name,&luid))
	{
	//	printf("LookupPrivilegeValueAʧ��");
	}
	tp.PrivilegeCount=1;
	tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid=luid;
	//����Ȩ��
	if(!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
	{
	//	printf("AdjustTokenPrivilegesʧ��");
	}
	return 0;
}

//�жϽ����Ƿ���64λ
int GetProcessIsWOW64(int pid)
{
	int nRet=-1;
	EnableDebugPriv("SeDebugPrivilege");
	HANDLE hProcess;

	//��Զ���߳�
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
	if (hProcess==NULL)
	{
		return -1;
	}

	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
	LPFN_ISWOW64PROCESS fnIsWow64Process; 
	BOOL bIsWow64 = FALSE; 
	BOOL bRet;
	DWORD nError;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle("kernel32"),"IsWow64Process"); 
	if (NULL != fnIsWow64Process) 
	{ 
		bRet=fnIsWow64Process(hProcess,&bIsWow64);
		if (bRet==0)
		{
			nError=GetLastError();
			nRet=-2;
		}
		else
		{
			if (bIsWow64)
			{
				nRet=1;
			}
			else
			{
				nRet=0;
			}
		}
	} 
	CloseHandle(hProcess);
	return nRet;
}


void UnInjectAll()
{
	EnableDebugPriv("SeDebugPrivilege");
	PROCESSENTRY32 pe32;//������ſ��ս�����Ϣ��һ���ṹ�塣�����һ��������Ϣ�͵��ó�Ա���������Ϣ��
	//���� Process32Firstָ���һ��������Ϣ��
	//����������Ϣ��ȡ��PROCESSENTRY32�С���Process32Nextָ����һ��������Ϣ��
	pe32.dwSize = sizeof(pe32);   //�����������Ҫ��Ȼ��ȡ����pe32 
	// ��ϵͳ�ڵ����н�����һ������
	HANDLE hprocesssnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS , 0);
	if (hprocesssnap == INVALID_HANDLE_VALUE) 
	{
	//	printf("INVALID_HANDLE_VALUE\n");
		return ;
	}
	BOOL bmore = Process32First(hprocesssnap , &pe32);
	HANDLE hfilename = NULL ;
	MODULEENTRY32 pes; 
	//����ṹ������һ����Ŀ��һ���б��о���һ��ָ���Ľ�����ʹ�õ�ģ�顣
	pes.dwSize = sizeof(MODULEENTRY32);
	// �������̿��գ�������ʾÿ�����̵���Ϣ 
	BOOL flag=TRUE;
	char ch[300];
	char ch1[1000];

	//cout<<mondll32path<<endl;
	//cout<<mondll64path<<endl;
	//cout<<easyhookdll64path<<endl;
	//ofstream f("C:/dll.txt",ios::app);
	while (bmore)
	{
		int r=0;
		r=GetProcessIsWOW64(pe32.th32ProcessID);
		/*
		if (r!=1)//�������64λ��ֱ��continue
		{
			bmore=Process32Next(hprocesssnap,&pe32);
			continue;
		}
		*/
		//f<<pe32.szExeFile<<endl;

		//printf("%s\n",pe32.szExeFile);
		//printf("%d\n" , pe32.th32ProcessID ); 
		int check=1;//��ǰ���̻�û��ע��dll
		hfilename = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE , pe32.th32ProcessID );
		BOOL i = Module32First(hfilename , &pes);
		//char dllpath[MAX_PATH]="D:/EasyHook32.dll";//Ҫж�ص�DLL��·��
		i=Module32Next(hfilename,&pes);
		while (i)
		{
			//f<<pes.szExePath<<" "<<pes.szModule<<endl;
			//cout<<pes.szModule<<endl;
			//int t1=strcmp(pes.szModule,"MonDll32.dll");
			int t2=strcmp(pes.szModule,"MonDll32.dll");
			int t3=strcmp(pes.szModule,"EasyHook32.dll");

			if (t2==0||t3==0)
			{
				check=0;
			//	cout<<pes.szExePath<<endl;
				break;
			}
			i=Module32Next(hfilename,&pes);
		}
		int t1=strcmp(pe32.szExeFile,"UnloadDll32.exe");
		if(t1==0){
			check=1;
		}
		if (check==0)
		{

		//	cout<<pe32.szExeFile<<endl;
			UnInjectDll("MonDll32.dll",pe32.th32ProcessID);
			//Sleep(500);
			//UnInjectDll("EasyHook32.dll",pe32.th32ProcessID);	
		}
		bmore = Process32Next(hprocesssnap , &pe32);
	}
	//f.close();
	CloseHandle(hfilename);
	CloseHandle(hprocesssnap);
	return;
}

void After(){
	EnableDebugPriv("SeDebugPrivilege");
	char exepath[MAX_PATH];
	GetModuleFileNameA(NULL,exepath,MAX_PATH);
	string str(exepath);
	int end=strlen(exepath)-15;
	dirpath=str.substr(0,end);
	sprintf_s(status,"%sUsMon.exe.status",dirpath.c_str());
	//cout<<exepath<<endl;
	//cout<<dirpath.c_str()<<endl;

}

int _tmain(int argc, _TCHAR* argv[])
/*int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
	)
	*/
{
	/*
	time_t rawtime; 
	struct tm * timeinfo; 
	time ( &rawtime ); 
	timeinfo = localtime ( &rawtime ); 
	printf("��ǰϵͳʱ��: %s", asctime(timeinfo));
	WinExec("taskkill /im MonInject32.exe /f",SW_HIDE);
	UnInjectAll();
	time(&rawtime);
	timeinfo = localtime ( &rawtime ); 
	printf("��ǰϵͳʱ��: %s", asctime(timeinfo));
	return 0;
	ofstream file1(status);
	file1<<"2";
	file1.close();
	while (true)
	{
		Sleep(1000);
		ifstream fin(status);
		char temp[100];
		fin.get(temp,100,'\0');
		fin.close();
		if (strcmp(temp,"1")==0)
		{
			break;
		}
	}
	*/
	After();
	UnInjectAll();
	ofstream file(status);
	file<<"0";
	file.close();
	return 0;
}

