// ShutDown.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "windows.h"
#include "wtypes.h"
#include "WinUser.h"
#include "ole2.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)   
{
	 char tmp[10]="hello"; 
	 switch (message)   
  {   
    
  case WM_QUERYENDSESSION:   
  /*���Ҫע����ر�ϵͳ���� 1 ���򷵻� 0   
  ����WM_QUERYENDSESSION���Ľ���ǿ���˳����������˳��������Windows������   
  һ��WM_ENDSESSION����Ϣ�����е�Process����wParam�����ݱ���ָ���Ƿ����˳������*/   
   
 // sprintf(tmp,"wParam = 0x%x lParam = 0x%x",wParam,lParam);   
  if(lParam == 0)   
  MessageBoxA(0,"�յ��ػ���������Ϣ",tmp,MB_OK);   
  else//lParam == 0x80000000   
  MessageBoxA(0,"�յ�ע����Ϣ",tmp,MB_OK);   
  return 0; //�������ػ�,�ͷ���1,����ػ��ͷ���0   
  case WM_ENDSESSION:   
  return 1; //���ﷵ��ʲô��ϵ����,�ܷ�ػ���������Ϣ����   
  default:   
  return DefWindowProc(hWnd, message, wParam, lParam);   
  }   
  return 0;   
}
int _tmain(int argc, _TCHAR* argv[])
{
	int a=0;
	Sleep(10000000);
	return 0;
}

