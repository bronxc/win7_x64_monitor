#include "HookApi.h"

//int GetFileNameByHandle(HANDLE hFile,LPSTR buff,DWORD size)
//{
//	HANDLE hfilemap = CreateFileMapping(hFile,NULL,PAGE_READWRITE,NULL,NULL,NULL);
//	if(INVALID_HANDLE_VALUE==hfilemap)
//	{
//		//printf("file mapping error");
//		return 0;
//	}
//
//
//	LPVOID lpmap = MapViewOfFile(hfilemap,FILE_MAP_READ|FILE_MAP_WRITE,NULL,NULL,0);
//	if(NULL==lpmap)
//	{
//		//printf("map view file error%d",GetLastError());
//		return 0;
//	}
//
//	//���������Psapi.h ��˵��GetMappedFileNameû������
//	//    DWORD length = GetMappedFileName(GetCurrentProcess(),map,buff,size);
//
//	MyGetMappedFileName GetMappedFileName =(MyGetMappedFileName)GetProcAddress(LoadLibraryA("psapi.dll"),"GetMappedFileNameA");
//
//	if(GetMappedFileName==NULL)
//	{
//		//printf("Get funcaddress error");
//		return 0;
//	}
//	DWORD length = GetMappedFileNameA(GetCurrentProcess(),lpmap,buff,size);
//	if(0==length)
//	{
//		//printf("get mapped file name error");
//		return 0;
//
//	}
//	//    printf("%s",buff);
//
//
//	char DosPath[MAX_PATH]={0};
//	char DriverString[MAX_PATH]={0};
//
//	GetLogicalDriveStringsA(MAX_PATH,DriverString);
//	char * p = (char *)DriverString;  //p����ָ���̷�
//	do
//	{
//		*(p+2)='\0'; //����QuerDosDevice��һ������������c:�������͵ģ�������\�����Ұ��Ǹ�\��Ĩ����  
//		QueryDosDeviceA(p,DosPath,MAX_PATH);
//		char * q = strstr(buff,DosPath);//���buff���Ƿ���DosDevice�е�DosPath���еĻ���pָ����Ǹ��ִ�����Ҫ���̷�
//		if(q!=0)
//		{
//			//�ҵ�֮��Ӧ�ð�buff�����һ������\�ط����ִ����ƹ������̷����·��
//
//			q = strrchr(buff,0x5c);
//
//			//�ٰ�DriverString·���������ַ����㣬ֻ�����ҵ����̷�
//			memset(p+2,0,MAX_PATH-2);
//			strcat_s(p,strlen(q),q);  //����·��
//			strcpy_s(buff,strlen(p),p);
//			return 1;
//		}
//
//
//		p=p+4;  //ָ���ƶ���DriverString����һ���̷���
//	}while(*p!=0);
//	return 0;
//}

char * GetProcessPath(){
	DWORD pid=(DWORD)_getpid();
	HANDLE hProcess=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,pid);
	GetModuleFileNameEx(hProcess,NULL,pathname,MAX_PATH);
	int iLength ;
	iLength = WideCharToMultiByte(CP_ACP, 0, pathname, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, pathname, -1, path, iLength, NULL, NULL);
	//OpenProcess֮��һ��Ҫ��סclose
	CloseHandle(hProcess);
	return path;
}

char * LogTime(){
	time_t t=time(NULL);
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	sprintf_s(tim,"%4d-%02d-%02d-%02d-%02d-%02d-%03d:\0",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
	return tim;
}

//���ַ�ת��Ϊ���ֽ�
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

char * GetDate(){
	time_t t=time(NULL);
	SYSTEMTIME sys;
	GetLocalTime(&sys);
	sprintf_s(dat,"%4d%02d%02d",sys.wYear,sys.wMonth,sys.wDay);
	return dat;
}

void GetProcessName(char* szProcessName,int* nLen){
	DWORD dwProcessID = GetCurrentProcessId();  
	HANDLE hProcess=OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwProcessID);   
	if(hProcess)  
	{  
		HMODULE hMod;  
		DWORD   dwNeeded;   
		if(EnumProcessModules(hProcess,&hMod,sizeof(hMod),&dwNeeded))  
		{  
			GetModuleBaseNameA(hProcess,hMod,szProcessName,*nLen);  
		}  
	}
	CloseHandle(hProcess);
}

std::string GetKeyPathFromKKEY(HKEY key)
{
	std::wstring keyPath;
	if (key==NULL)
	{
		return "NULL";
	}
	if (key != NULL)
	{
		HMODULE dll = LoadLibrary(L"ntdll.dll");
		if (dll != NULL) {
			typedef DWORD (__stdcall *ZwQueryKeyType)(
				HANDLE  KeyHandle,
				int KeyInformationClass,
				PVOID  KeyInformation,
				ULONG  Length,
				PULONG  ResultLength);

			ZwQueryKeyType func = reinterpret_cast<ZwQueryKeyType>(::GetProcAddress(dll, "ZwQueryKey"));

			if (func != NULL) {
				DWORD size = 0;
				DWORD result = 0;
				result = func(key, 3, 0, 0, &size);
				if (result == STATUS_BUFFER_TOO_SMALL)
				{
					size = size + 2;
					wchar_t* buffer = new (std::nothrow) wchar_t[size];
					if (buffer != NULL)
					{
						result = func(key, 3, buffer, size, &size);
						if (result == STATUS_SUCCESS)
						{
							buffer[size / sizeof(wchar_t)] = L'\0';
							keyPath = std::wstring(buffer + 2);
						}

						delete[] buffer;
					}
				}
			}

			FreeLibrary(dll);
		}
	}
	return WideToMutilByte(keyPath);
}

char * GetIPbySocket(SOCKET s){
	char *sock_ip;
	sockaddr_in sock;
	int socklen=sizeof(sock);
	//char sock_ip[]="NULL";
	//char sock_ip[1000]="NULL";
	getsockname(s,(struct sockaddr*)&sock,&socklen);
	sock_ip=inet_ntoa(sock.sin_addr);
	return sock_ip;
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
		//printf("LookupPrivilegeValueAʧ��");
	}
	tp.PrivilegeCount=1;
	tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid=luid;
	//����Ȩ��
	if(!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
	{
		//printf("AdjustTokenPrivilegesʧ��");
	}
	CloseHandle(hToken);
	return 0;
}

BOOL InjectDll(const char *DllFullPath,const DWORD dwRemoteProcessId)
{
	HANDLE hRemoteProcess;
	EnableDebugPriv("SeDebugPrivilege");
	//��Զ���߳�
	hRemoteProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwRemoteProcessId);
	if (hRemoteProcess==NULL)
	{
		return FALSE;
	}
	char *pszLibFileRemote;
	//ʹ��VirtualAllocEx������Զ�̽��̵��ڴ��ַ�ռ����DLL�ļ����ռ�
	pszLibFileRemote=(char *)VirtualAllocEx(hRemoteProcess,NULL,lstrlenA(DllFullPath)+1,MEM_COMMIT,PAGE_READWRITE);
	if (pszLibFileRemote==NULL)
	{
		CloseHandle(hRemoteProcess);
		return FALSE;
		//ofstream f("C:\\pro.txt",ios::app);
		//printf("VirtualAllocExʧ��\n");
		//f<<"VirtualAllocExʧ��"<<endl;
		//f.close();
	}
	//printf("%c",pszLibFileRemote);
	//printf("%d\n",GetLastError());
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
		//ofstream f("C:\\pro.txt",ios::app);
		//printf("CreateRemoteThreadʧ��\n");
		//printf("%d",GetLastError());
		//f<<"CreateRemoteThreadʧ��"<<endl;
		//f<<GetLastError()<<endl;
		//f.close();
		CloseHandle(hRemoteProcess);
		return FALSE;
	}
	//�ͷž��
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);
	return TRUE;
}

//�жϽ����Ƿ���64λ�������64λ����0�������32λ����1
int GetProcessIsWOW64(DWORD pid)
{
	int nRet=-1;
	EnableDebugPriv("SeDebugPrivilege");
	HANDLE hProcess;

	//��Զ���߳�
	hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
	LPFN_ISWOW64PROCESS fnIsWow64Process; 
	BOOL bIsWow64 = FALSE; 
	BOOL bRet;
	DWORD nError;
	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(L"kernel32"),"IsWow64Process"); 
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

//void GetFileNameFromHandle(HANDLE hFile,char myhandlepath[MAX_PATH]) 
//{
//	BOOL bSuccess = FALSE;
//	TCHAR pszFilename[MAX_PATH+1]={0};
//	HANDLE hFileMap;
//
//	// Get the file size.
//	DWORD dwFileSizeHi = 0;
//	DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi); 
//	if( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
//	{
//		strcpy_s(myhandlepath,5,"NULL");
//		return;
//		//_tprintf(TEXT("Cannot map a file with a length of zero.\n"));
//	}
//
//	// Create a file mapping object.
//	hFileMap = CreateFileMapping(hFile, 
//		NULL, 
//		PAGE_READONLY,
//		0, 
//		1,
//		NULL);
//
//	if (hFileMap) 
//	{
//		// Create a file mapping to get the file name.
//		void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);
//
//		if (pMem) 
//		{
//			if (GetMappedFileName (GetCurrentProcess(), 
//				pMem, 
//				pszFilename,
//				MAX_PATH)) 
//			{
//
//				// Translate path with device name to drive letters.
//				TCHAR szTemp[512];
//				szTemp[0] = '\0';
//
//				if (GetLogicalDriveStrings(512-1, szTemp)) 
//				{
//					TCHAR szName[MAX_PATH];
//					TCHAR szDrive[3] = TEXT(" :");
//					BOOL bFound = FALSE;
//					TCHAR* p = szTemp;
//
//					do 
//					{
//						// Copy the drive letter to the template string
//						*szDrive = *p;
//
//						// Look up each device name
//						if (QueryDosDevice(szDrive, szName, MAX_PATH))
//						{
//							size_t uNameLen = _tcslen(szName);
//
//							if (uNameLen < MAX_PATH) 
//							{
//								bFound = _tcsnicmp(pszFilename, szName, uNameLen) == 0
//									&& *(pszFilename + uNameLen) == _T('\\');
//
//								if (bFound) 
//								{
//									// Reconstruct pszFilename using szTempFile
//									// Replace device path with DOS path
//									TCHAR szTempFile[MAX_PATH];
//									StringCchPrintf(szTempFile,
//										MAX_PATH,
//										TEXT("%s%s"),
//										szDrive,
//										pszFilename+uNameLen);
//									StringCchCopyN(pszFilename, MAX_PATH+1, szTempFile, _tcslen(szTempFile));
//								}
//							}
//						}
//
//						// Go to the next NULL character.
//						while (*p++);
//					} while (!bFound && *p); // end of string
//				}
//			}
//			bSuccess = TRUE;
//			UnmapViewOfFile(pMem);
//		}
//		CloseHandle(hFileMap);
//	}
//	//_tprintf(TEXT("File name is %s\n"), pszFilename);
//	if (lstrlen(pszFilename)==0||lstrlen(pszFilename)>MAX_PATH)
//	{
//		strcpy_s(myhandlepath,5,"NULL");
//		return ;
//	}
//	string tempfilepath;
//	//wprintf(pszFilename);
//	tempfilepath=WideToMutilByte(pszFilename);
//	strcpy_s(myhandlepath,strlen(tempfilepath.c_str())+1,tempfilepath.c_str());
//}

void WriteLog(string s){
	s=s+"\n";
	extern HANDLE g_handleMailServer;
	extern LPTSTR g_strInjectMailSlot;
	cout<<s;
	if (g_handleMailServer != INVALID_HANDLE_VALUE)
	{
		DWORD cbWritten = 0;
		BOOL result = realWriteFile?realWriteFile(g_handleMailServer,s.c_str(),s.length(),&cbWritten,NULL):WriteFile(g_handleMailServer,s.c_str(),s.length(),&cbWritten,NULL);
		//�ɹ��Ļ���ֱ���˳�
		//////////////////////////////////////////////////////////////////////////
		// to do :���ɹ��Ļ�������һ���жϣ�������������Ѿ��˳�����g_handleMailServer��Ϊ��Ч
		//////////////////////////////////////////////////////////////////////////
		if (result)
			return;
	}
	
	
	ofstream f(g_log_path,ios::app);
	f<<s;
	f.close();
	
}
