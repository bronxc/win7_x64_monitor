// InjectDll.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "iostream"
#include "windows.h"
#include "TLHELP32.H"
#include "fstream"
using namespace std;
//ofstream f("C:\\Log\\log.txt");

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
	return 0;
}
BOOL InjectDll(const char *DllFullPath,const DWORD dwRemoteProcessId)
{
	HANDLE hRemoteProcess;
	EnableDebugPriv("SeDebugPrivilege");
	//��Զ���߳�
	hRemoteProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwRemoteProcessId);
	char *pszLibFileRemote;
	//ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ����ռ�
	pszLibFileRemote=(char *)VirtualAllocEx(hRemoteProcess,NULL,lstrlenA(DllFullPath)+1,MEM_COMMIT,PAGE_READWRITE);
	if (pszLibFileRemote==NULL)
	{
		printf("VirtualAllocExʧ��\n");
	}
	printf("%c",pszLibFileRemote);
	printf("%d\n",GetLastError());
	//ʹ��WriteProcessMemory������DLL��·����д�뵽Զ�̽��̵��ڴ�
	WriteProcessMemory(hRemoteProcess,pszLibFileRemote,(void *)DllFullPath,lstrlenA(DllFullPath)+1,NULL);
	/*
	//����LoadLibraryA����ڵ�ַ
	PTHREAD_START_ROUTINE pfnStartAddr=NULL;
	pfnStartAddr=(PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")),"LoadLibraryA");
	//����GetModuleHandle������GetProcAddress����
	HANDLE hRemoteThread=NULL;
	if((hRemoteThread=CreateRemoteThread(hRemoteThread,NULL,0,pfnStartAddr,pszLibFileRemote,0,NULL))==NULL)
	{
		printf("�߳�ע��ʧ��");
		return FALSE;
	}
	*/
	DWORD dwID;
	LPVOID pFunc = LoadLibraryA;
	HANDLE hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, pszLibFileRemote, 0, &dwID );
	if (hRemoteThread==NULL)
	{
		printf("CreateRemoteThreadʧ��\n");
		printf("%d",GetLastError());
		return FALSE;
	}
	//�ͷž��
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);
	return TRUE;
}

void InjectAll()
{
	PROCESSENTRY32 pe32;//������ſ��ս�����Ϣ��һ���ṹ�塣�����һ��������Ϣ�͵��ó�Ա���������Ϣ��
	//���� Process32Firstָ���һ��������Ϣ��
	//����������Ϣ��ȡ��PROCESSENTRY32�С���Process32Nextָ����һ��������Ϣ��
	pe32.dwSize = sizeof(pe32);   //�����������Ҫ��Ȼ��ȡ����pe32 
	// ��ϵͳ�ڵ����н�����һ������
	HANDLE hprocesssnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS , 0);
	if (hprocesssnap == INVALID_HANDLE_VALUE) 
	{
		printf("INVALID_HANDLE_VALUE\n");
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
	ofstream f("C:\\process.txt",ios::out);
	f.clear();
	while (bmore)
	{
		wprintf(L"%s\n",pe32.szExeFile);
		wprintf(L"%u\n" , pe32.th32ProcessID ); 
		//flag=InjectDll("C://DllHook.dll",pe32.th32ProcessID);
		WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, wcslen(pe32.szExeFile) + 1, ch, 300, NULL, NULL);
		if (flag==TRUE)
		{
			f<<ch<<" ";
		}
		InjectDll("D://EasyHook64.dll",pe32.th32ProcessID);
		//wprintf(_T("�߳���Ŀ: %d \n"),pe32.cntThreads);
		hfilename = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE , pe32.th32ProcessID );
		if (hfilename == NULL )
		{
			printf("�ļ����ַ���ʧ��!");
		}
		BOOL i = Module32First(hfilename , &pes);
		wprintf(L"%s \n" , pes.szExePath);
		WideCharToMultiByte(CP_ACP, 0, pes.szExePath, wcslen(pes.szExePath) + 1, ch1, 300, NULL, NULL);
		f<<ch1<<endl;
		//wf<<pes.szExePath<<endl;
		bmore = Process32Next(hprocesssnap , &pe32);
	}
	f.close();
	//�ر��ں˶���
	CloseHandle(hfilename);
	CloseHandle(hprocesssnap);
	return;
}
int main(int argc, CHAR* argv[])
{
	
	InjectAll();
	//printf("%s %s %s",argv[0],argv[1],argv[2]);
	//InjectDll(argv[1],atoi(argv[2]));
    //cout<<argv[1]<<" "<<argv[2]<<endl;
	//InjectDll("D://EasyHook64.dll",3184);
	return 0;
}

