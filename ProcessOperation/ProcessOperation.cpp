// ProcessOperation.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "iostream"
#include "windows.h"
#include <TLHELP32.h>
#include "stdio.h"
using namespace std;
HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;

int const MAX_REMOTE_DATA = 1024 * 4;  

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

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		// the action is valid: HC_ACTION.
		if (wParam == WM_KEYDOWN)
		{
			// lParam is the pointer to the struct containing the data needed, so cast and assign it to kdbStruct.
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
			// a key (non-system) is pressed.
			if (kbdStruct.vkCode == VK_F1)
			{
				// F1 is pressed!
				MessageBoxA(NULL, "F1 is pressed!", "key pressed", MB_ICONINFORMATION);
			}
		}
	}

	// call the next hook in the hook chain. This is nessecary or your hook chain will break and the hook stops
	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

int _tmain(int argc, _TCHAR* argv[])
{
	/*HANDLE th,ph;
	th=OpenThread(THREAD_ALL_ACCESS,FALSE,2300);
	if (th==NULL)
	{
		cout<<"ʧ��1"<<endl;
	}*/
	//ph=OpenThread(THREAD_ALL_ACCESS,true,GetCurrentThreadId());
	//if (ph==NULL)
	//{
	//	cout<<"ʧ��2"<<endl;
	//}
	////LPDWORD lpExitCode=0;
	////GetExitCodeThread(th,lpExitCode);
	//TerminateThread(th,0);
	//return 0;
	

	char ch[256]="";
	//������Ϣ����
	cout<<"������Ϣ���Ӳ��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	_hook=SetWindowsHookExA(WH_KEYBOARD_LL, HookCallback, NULL, 0);
	UnhookWindowsHookEx(_hook);
	_hook=SetWindowsHookExW(WH_KEYBOARD_LL,HookCallback, NULL,0);
	UnhookWindowsHookEx(_hook);
	cout<<"������Ϣ���Ӳ�������\n"<<endl;

	//���Ҵ���
	cout<<"���Ҵ��ڲ��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	HWND maindHwnd = FindWindowW(L"�ޱ���-���±�",L"�ޱ���-���±�");
	FindWindowExW(maindHwnd, maindHwnd, L"�ޱ���-���±�",L"�ޱ���-���±�");
	FindWindowExA(maindHwnd,maindHwnd,"�ޱ���-���±�","�ޱ���-���±�");
	GetForegroundWindow();
	cout<<"���Ҵ��ڲ�������\n"<<endl;

	//ע�����
	cout<<"ע�������������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);  
	//TCHAR szServiceName[] = _T("ServiceTest"); 
	LPCWSTR szServiceName=L"ServiceTest";
	LPSTR szFilePath;
	LPCWSTR szFilePath1;
	//TCHAR szFilePath[MAX_PATH];  
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);
	SC_HANDLE hService = ::CreateServiceW(  
		hSCM, szServiceName, szServiceName,  
		SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,  
		SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,  
		szFilePath1, NULL, NULL, L"", NULL, NULL);
	char szServiceName1[]="ServiceTest1";
	char szFilePath2[MAX_PATH];
	GetModuleFileNameA(NULL,szFilePath2,MAX_PATH);
	SC_HANDLE hService1 = ::CreateServiceA(hSCM,szServiceName1,szServiceName1,SERVICE_ALL_ACCESS,SERVICE_WIN32_OWN_PROCESS,
								SERVICE_DEMAND_START,SERVICE_ERROR_NORMAL,szFilePath2,NULL,NULL,"",NULL,NULL);
	cout<<"ע������������\n"<<endl;


	//ö�ٽ���
	cout<<"ö�ٽ��̲��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32); 
	HANDLE hprocesssnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS , 0);
	BOOL bmore = Process32First(hprocesssnap , &pe32);
	bmore = Process32Next(hprocesssnap , &pe32);
	CloseHandle(hprocesssnap);
	cout<<"ö�ٽ��̲�������\n"<<endl;
	//�����ں�ģ��
	
	//����̽����߳�
	cout<<"����̽����̲߳��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	int tid=0;
	cout<<"������Explorer.exe���̵�һ���̵߳��̺߳ţ�����ProcessHacker���Բ鿴Explorer.exe���̺߳ţ���"<<endl;
	cin>>tid;
	HANDLE th;
	th=OpenThread(THREAD_ALL_ACCESS,FALSE,tid);
	TerminateThread(th,0);
	CloseHandle(th);
	cout<<"����̽����̲߳�������\n"<<endl;
	
	
	//����̹����߳�
	cout<<"����̹����̲߳��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	cout<<"������Explorer.exe���̵�һ���̵߳��̺߳ţ�����ProcessHacker���Բ鿴Explorer.exe���̺߳ţ���"<<endl;
	cin>>tid;
	th=OpenThread(THREAD_ALL_ACCESS,FALSE,tid);
	SuspendThread(th);
	cout<<"����̹����̲߳�������\n"<<endl;

	//����ָ̻��߳�
	cout<<"����ָ̻��̲߳���������һ���й�����߳̽��лָ������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	ResumeThread(th);
	CloseHandle(th);
	cout<<"����ָ̻��̲߳�������\n"<<endl;

	//����������߳�������
	cout<<"����������߳������Ĳ���,���������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	HANDLE ht1;
	ht1=OpenThread(THREAD_ALL_ACCESS,FALSE,tid);
	LPCONTEXT lpc=NULL;
	GetThreadContext(ht1,lpc);
	SetThreadContext(ht1,lpc);
	CloseHandle(ht1);
	cout<<"����������߳������Ĳ�������\n"<<endl;
	
	//����Զ���߳�
	cout<<"����Զ���̲߳��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	int pid=0;
	cout<<"������Explorer.exe���̵Ľ��̺ţ�"<<endl;
	cin>>pid;
	EnableDebugPriv("SeDebugPrivilege");
	HANDLE hProcess;
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
	char DllFullPath[]="aaaa";
	LPVOID lpfunc=LoadLibraryA;
	char *pszLibFileRemote;
	pszLibFileRemote=(char *)VirtualAllocEx(hProcess,NULL,lstrlenA(DllFullPath)+1,MEM_COMMIT,PAGE_READWRITE);
	WriteProcessMemory(hProcess,pszLibFileRemote,(void *)DllFullPath,lstrlenA(DllFullPath)+1,NULL);
	DWORD dwID;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpfunc, pszLibFileRemote, 0, &dwID );
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
	cout<<"����Զ���̲߳�������\n"<<endl;

	//����̶��ڴ�
	cout<<"����̶��ڴ���������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
	int tmp;
	SIZE_T dwNumberOfBytesRead;
	ReadProcessMemory(hProcess,(LPCVOID)0x00401000,&tmp,4,&dwNumberOfBytesRead);
	CloseHandle(hProcess);
	cout<<"����̶��ڴ��������\n"<<endl;
	
	//�����д�ڴ�
	cout<<"�����д�ڴ���������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
	pszLibFileRemote=(char *)VirtualAllocEx(hProcess,NULL,lstrlenA(DllFullPath)+1,MEM_COMMIT,PAGE_READWRITE);
	WriteProcessMemory(hProcess,pszLibFileRemote,(void *)DllFullPath,lstrlenA(DllFullPath)+1,NULL);
	cout<<"�����д�ڴ��������\n"<<endl;

	//������޸��ڴ�����
	cout<<"������޸��ڴ����Բ��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	DWORD dwPrev;
	VirtualProtectEx(hProcess,pszLibFileRemote,4,PAGE_READWRITE,&dwPrev);
	cout<<"������ͷ��ڴ����Բ�������\n"<<endl;
	
	//������ͷ��ڴ�
	cout<<"������ͷ��ڴ���������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	VirtualFreeEx(hProcess,pszLibFileRemote,0,MEM_RELEASE);
	CloseHandle(hProcess);
	cout<<"������ͷ��ڴ��������\n"<<endl;
	
	
	
	//��������
	cout<<"�������̲��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	STARTUPINFOA si = { sizeof(si) };   
	PROCESS_INFORMATION pi;   
	si.dwFlags = STARTF_USESHOWWINDOW;   
	si.wShowWindow = TRUE; //TRUE��ʾ��ʾ�����Ľ��̵Ĵ���  
	//TCHAR cmdline[] =TEXT("c://program files//internet explorer//iexplore.exe http://community.csdn.net/"); 
	char cmd[]="c://Windows//system//cmd.exe";
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
	cout<<"�������̲�������\n"<<endl;
	
	//�򿪽���
	cout<<"�򿪽��̲��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
	cout<<"�򿪽��̲�������\n"<<endl;
	
	//�������
	
	//�ָ�����
	
	//�������̲���
	cout<<"�������̲��������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	cout<<"����һ�����̺ţ����������ý��̣�"<<endl;
	cin>>pid;
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
	TerminateProcess(hProcess,0);
	cout<<"�������̲�������\n"<<endl;
	return 0;
}

