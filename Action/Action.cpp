// Action.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "stdio.h"  
#include "tchar.h"  
#include <windows.h>  
#include<iostream>  
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif
using namespace std;

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
		printf("LookupPrivilegeValueAʧ��");
	}
	tp.PrivilegeCount=1;
	tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid=luid;
	//����Ȩ��
	if(!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
	{
		printf("AdjustTokenPrivilegesʧ��");
	}
	CloseHandle(hToken);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	BYTE    *lpData;
	int mmm=0;
	cin>>mmm;
	//�������ͷ��ļ�
	HANDLE hOpenFileA=CreateFileA("C:\\trojan.exe",GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	char *data="http://www.baidu.com/"; 
	DWORD a = 25; 
	unsigned long b; 
	OVERLAPPED   c; 
	WriteFile(hOpenFileA, data, a, &b, NULL);
	STARTUPINFOA si = { sizeof(si) };   
	PROCESS_INFORMATION pi;   
	si.dwFlags = STARTF_USESHOWWINDOW;   
	si.wShowWindow = TRUE; //TRUE��ʾ��ʾ�����Ľ��̵Ĵ���  
	//TCHAR cmdline[] =TEXT("c://program files//internet explorer//iexplore.exe http://community.csdn.net/"); 
	char cmd[]="c://trojan.exe";
	BOOL bRet = ::CreateProcessA (   
		NULL,  
		cmd, //��Unicode�汾�д˲�������Ϊ�����ַ�������Ϊ�˲����ᱻ�޸�    
		NULL,   
		NULL,   
		FALSE,   
		CREATE_NEW_CONSOLE,   
		NULL,   
		NULL,   
		&si,   
		&pi); 

	//ע�����̬��
	//http://blog.csdn.net/chenyujing1234/article/details/8023816
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);  
	TCHAR szServiceName[] = _T("ServiceTest"); 
	TCHAR szFilePath[MAX_PATH];  
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);
	SC_HANDLE hService = ::CreateService(  
		hSCM, szServiceName, szServiceName,  
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,  
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,  
		szFilePath, NULL, NULL, _T(""), NULL, NULL);

	//���ϵͳ����ǽ�Ź��б�
	/*
	*����ֱ��ע��regedit�ռ�����Ϊ
	*/
	/*
	HKEY hk;
	RegCreateKeyA(HKEY_LOCAL_MACHINE, 
		"SYSTEM\\CurrentControlSet\\services\\SharedAccess\\Parameters\\FirewallPolicy\\FirewallRules", 
		&hk);
	CHAR szBuf[80];
	strcpy(szBuf, "%SystemRoot%\\System\\a.dll"); 
	RegSetValueExA(hk,             // sub key handle 
		"aaaaa",       // value name 
		0,                        // must be zero 
		REG_SZ,            // value type 
		(LPBYTE) szBuf,           // pointer to value data 
		strlen(szBuf) + 1);
		*/
	//��ֹ����

	/*
	*����ֱ��ע��regedit�ռ�����Ϊ
	*/

	//����ϵͳ��ȫ��
	/*
	ע���������regedit���Եõ�
	*/

	//�޸�ע�����������
	/*
	ע���������regedit���Եõ�
	*/

	//���ֽ���
	 char *DllFullPath="aaaaaaaa";
	EnableDebugPriv("SeDebugPrivilege");
	DWORD dwRemoteProcessId=_getpid();
	HANDLE hRemoteProcess;
	hRemoteProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwRemoteProcessId);
	char *pszLibFileRemote;
	pszLibFileRemote=(char *)VirtualAllocEx(hRemoteProcess,NULL,lstrlenA(DllFullPath)+1,MEM_COMMIT,PAGE_READWRITE);
	if (pszLibFileRemote==NULL)
	{
		//	printf("VirtualAllocExʧ��\n");
		BOOL res;
		res=CloseHandle(hRemoteProcess);
		//cout<<"res "<<res<<endl;
		return FALSE;
	}
	WriteProcessMemory(hRemoteProcess,pszLibFileRemote,(void *)DllFullPath,lstrlenA(DllFullPath)+1,NULL);

	return 0;
}

