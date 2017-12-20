#pragma once;
#include "Inihelp.h"  
#include <iostream>
#include "tchar.h"


inihelp::inihelp(const char *szfile)
{
	memset(m_file, 0x00, sizeof(m_file));
	CharToTchar(szfile, m_file);
}

void inihelp::TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength;
	//��ȡ�ֽڳ���    ..
    iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char      ..
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}


void inihelp::CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength;
	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength*sizeof(wchar_t));
}


int inihelp::ReadInteger(LPCTSTR szSection, LPCTSTR szKey, int iDefaultValue)
{
	int iResult = GetPrivateProfileInt(szSection, szKey, iDefaultValue, m_file);
	return iResult;
}

double inihelp::ReadFloat(LPCTSTR szSection, LPCTSTR szKey, double fltDefaultValue)
{
	TCHAR szResult[255];
	TCHAR szDefault[255];
	double fltResult;
	_stprintf_s(szDefault, 255, TEXT("%f"), fltDefaultValue);
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_file);
	DWORD errcode = GetLastError();
	fltResult = _wtof(szResult);
	return fltResult;
}

bool inihelp::ReadBoolean(LPCTSTR szSection, LPCTSTR szKey, bool bolDefaultValue)
{
	TCHAR szResult[255];
	TCHAR szDefault[255];
	bool bolResult;
	_stprintf_s(szDefault, 255, TEXT("%s"), bolDefaultValue ? TEXT("True") : TEXT("False"));
	GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_file);
	bolResult = (_tcscmp(szResult, TEXT("True")) == 0 ||_tcscmp(szResult, TEXT("true")) == 0) ? true : false;
	return bolResult;
}

LPTSTR inihelp::ReadString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefaultValue)
{
	LPTSTR szResult = new TCHAR[255];
	memset(szResult, 0x00, sizeof(szResult));
	GetPrivateProfileString(szSection, szKey, szDefaultValue, szResult, 255, m_file);
	return szResult;
}

void inihelp::WriteInteger(LPCTSTR szSection, LPCTSTR szKey, int iValue)
{
	TCHAR szValue[255];
	_stprintf_s(szValue, 255, TEXT("%d"), iValue);
	WritePrivateProfileString(szSection, szKey, szValue, m_file);
}

void inihelp::WriteFloat(LPCTSTR szSection, LPCTSTR szKey, double fltValue)
{
	TCHAR szValue[255];
	_stprintf_s(szValue, 255, TEXT("%f"), fltValue);
	WritePrivateProfileString(szSection, szKey, szValue, m_file);
}

void inihelp::WriteBoolean(LPCTSTR szSection, LPCTSTR szKey, bool bolValue)
{
	TCHAR szValue[255];
	_stprintf_s(szValue, 255, TEXT("%s"), bolValue ? TEXT("True") : TEXT("False"));
	WritePrivateProfileString(szSection, szKey, szValue, m_file);
}

void inihelp::WriteString(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue)
{
	WritePrivateProfileString(szSection, szKey, szValue, m_file);
}

int inihelp::CalCount(LPCTSTR szfile)
{
	TCHAR       chSectionNames[2048] = { 0 };       //���н�����ɵ��ַ�����..
	TCHAR * pSectionName; //�����ҵ���ĳ�������ַ������׵�ַ..
	int i;       //iָ������chSectionNames��ĳ��λ�ã���0��ʼ��˳�����..
	int j = 0;      //j����������һ�������ַ������׵�ַ����ڵ�ǰi��λ��ƫ����..
	int count = 0;      //ͳ�ƽڵĸ���..

	GetPrivateProfileSectionNames(chSectionNames, 2048, szfile);
	for (i = 0; i<2048; i++, j++)
	{
		if (chSectionNames[0] == '\0')
			break;       //�����һ���ַ�����0����˵��ini��һ����Ҳû��..
		if (chSectionNames[i] == '\0')
		{
			pSectionName = &chSectionNames[i - j]; //�ҵ�һ��0����˵��������ַ���ǰ������j��ƫ����������һ���������׵�ַ..
			count++;
			j = -1;         //�ҵ�һ��������j��ֵҪ��ԭ����ͳ����һ��������ַ��ƫ����..
			//����-1����Ϊ�����ַ��������һ���ַ�0����ֹ����������Ϊ������һ����..
			if (chSectionNames[i + 1] == 0)
			{
				break;      //���������ڵ��ַ�����0ʱ�������еĽ��������ҵ���ѭ����ֹ..
			}
		}
	}
	return count;
}