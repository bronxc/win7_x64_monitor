// FileOperation.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
//#include "iostream"
#include <iostream>
#include "stdio.h"
#include "windows.h"
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	char ch[256]="";
	//�����ļ� CreateFileW
	cout<<"�����ļ���������C�����洴��a.txt,b.txt,c.txt��d.txt�ļ������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	HANDLE fileHandle1,fileHandle2,fileHandle3,fileHandle4; 
	fileHandle1=CreateFileW(L"C:\\a.txt", GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); 
	fileHandle2=CreateFileW(L"C:\\b.txt", GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); 
	fileHandle3=CreateFileW(L"C:\\c.txt", GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); 
	fileHandle4=CreateFileW(L"C:\\d.txt", GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); 
	cout<<"�����ļ���������\n"<<endl;
	//���ļ�
	cout<<"���ļ���������C�������a.txt�ļ������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	OFSTRUCT ofStruct={ sizeof(OFSTRUCT) };
	OpenFile("C:\\a.txt",&ofStruct, OF_READWRITE|OF_SHARE_EXCLUSIVE);
	cout<<"���ļ���������\n"<<endl;

	//д���ļ�
	cout<<"д���ļ���������C�������a.txt��b.txt�зֱ�д���ַ�����aaaa��,���������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	char *data="aaaa"; 
	DWORD a = strlen(data); 
	unsigned long b; 
	WriteFile(fileHandle1, data, a, &b, NULL);
	HANDLE h_Event = CreateEvent(NULL, TRUE, FALSE, NULL);
	OVERLAPPED k_Over = {0};
	//&k_Over->hEvent=h_Event;
	WriteFileEx(fileHandle2,data,a,&k_Over,NULL);
	CloseHandle(fileHandle1);
	CloseHandle(fileHandle2);
	cout<<"д���ļ���������\n"<<endl;

	//��ȡ�ļ�
	cout<<"��ȡ�ļ�����������ȡC�������a.txt�ļ��е�����,���������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	fileHandle1=CreateFileW(L"C:\\a.txt", GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); 
	char read[256]="";
	DWORD readsize=0;
	ReadFile(fileHandle1,read,4,&readsize,NULL);
	OVERLAPPED r_Over = {0};
	ReadFileEx(fileHandle1,read,4,&r_Over,NULL);
	//cout<<"���ļ��ж�ȡ�����ݣ�"<<read<<endl;
	CloseHandle(fileHandle1);
	cout<<"��ȡ�ļ���������\n"<<endl;
	//�ض��ļ�
	cout<<"�ض��ļ���������C�������a.txt�ӿ�ʼ��ƫ��Ϊ2�ĵط��ض��ļ������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	long distance=2;
	fileHandle1=CreateFileW(L"C:\\a.txt", GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0); 
	SetFilePointer(fileHandle1, distance, NULL, FILE_BEGIN);
	SetEndOfFile(fileHandle1);
	CloseHandle(fileHandle1);
	cout<<"�ض��ļ���������\n"<<endl;

	//�������ļ�
	cout<<"�������ļ���������C�������a.txt,b.txt,c.txt��d.txt������Ϊe.txt,f.txt,g.txt��h.txt,���������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	CloseHandle(fileHandle1);
	CloseHandle(fileHandle2);
	CloseHandle(fileHandle3);
	CloseHandle(fileHandle4);
	MoveFileW(L"C:\\a.txt",L"C:\\e.txt");
	MoveFileExW(L"C:\\b.txt",L"C:\\f.txt",MOVEFILE_COPY_ALLOWED);
	MoveFileA("C:\\c.txt","C:\\g.txt");
	MoveFileExA("C:\\d.txt","C:\\h.txt",MOVEFILE_COPY_ALLOWED);
	cout<<"�������ļ���������\n"<<endl;

	//ɾ���ļ�
	cout<<"ɾ���ļ������ὫC�������g.txt�ļ���h.txt�ļ�ɾ�������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	DeleteFileA("C:\\g.txt");
	DeleteFileW(L"C:\\h.txt");
	cout<<"ɾ���ļ���������\n"<<endl;

    //����Ŀ¼
	cout<<"����Ŀ¼�����������C�̵�Windows�ļ��е��ļ������������ʼִ����Ϊ��"<<endl;
	gets_s(ch);
	WIN32_FIND_DATAA q1;
	HANDLE h1=FindFirstFileA("C:\\Windows\\*.*",&q1);
	FindNextFileA(h1,&q1);
	WIN32_FIND_DATAW q2;
	HANDLE h2=FindFirstFileW(L"C:\\Windows\\*.*",&q2);
	FindNextFileW(h2,&q2);
	cout<<"����Ŀ¼��������\n"<<endl;

	return 0;
}

