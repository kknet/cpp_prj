#pragma once
#include "direct.h"  
#include "io.h"   
#include <deque>  
#include <iostream>
#pragma warning(disable : 4996)
using namespace std;

class CBrowseDir
{
protected:
	//��ų�ʼĿ¼�ľ���·������'\'��β  .
	char m_szInitDir[_MAX_PATH];

public:
	//������  .
	CBrowseDir(const char *filepath);

	//���ó�ʼĿ¼Ϊdir���������false����ʾĿ¼������  .
	bool SetInitDir(const char *dir);

	//����Ŀ¼dir����filespecָ�����ļ�  .
	//������Ŀ¼,���õ����ķ���  .
	//�������false,��ʾ��ֹ�����ļ�  .
	bool BrowseDir(const char *dir, const char *filespec);
	void GetDirFilenames(const char *dir, const char *filespec,deque<string>&);


	//����BrowseDirÿ�ҵ�һ���ļ�,�͵���ProcessFile  .
	//�����ļ�����Ϊ�������ݹ�ȥ  .
	//�������false,��ʾ��ֹ�����ļ�  .
	//�û����Ը�д�ú���,�����Լ��Ĵ������  .
	virtual bool ProcessFile(const char *filename);

	//����BrowseDirÿ����һ��Ŀ¼,�͵���ProcessDir  .
	//�������ڴ����Ŀ¼������һ��Ŀ¼����Ϊ�������ݹ�ȥ  .
	//������ڴ�����ǳ�ʼĿ¼,��parentdir=NULL  .
	//�û����Ը�д�ú���,�����Լ��Ĵ������  .
	//�����û�����������ͳ����Ŀ¼�ĸ���  .
	virtual void ProcessDir(const char *currentdir, const char *parentdir);
};


//��CBrowseDir�����������࣬����ͳ��Ŀ¼�е��ļ�����Ŀ¼����  .
//class CStatDir :public CBrowseDir.
//{.
//protected:.
//	int m_nFileCount;   //�����ļ�����  .
//	int m_nSubdirCount; //������Ŀ¼����  .
//.
//public:.
//	//ȱʡ������  .
//	CStatDir(const char *filepath) :CBrowseDir(filepath).
//	{.
//		//��ʼ�����ݳ�Աm_nFileCount��m_nSubdirCount  .
//		m_nFileCount = m_nSubdirCount = 0;.
//	}.
//.
//	//�����ļ�����  .
//	int GetFileCount().
//	{.
//		return m_nFileCount;.
//	}.
//.
//	//������Ŀ¼����  .
//	int GetSubdirCount().
//	{.
//		//��Ϊ�����ʼĿ¼ʱ��Ҳ����ú���ProcessDir��  .
//		//���Լ�1�������������Ŀ¼������  .
//		return m_nSubdirCount - 1;.
//	}.
//.
//protected:.
//	//��д�麯��ProcessFile��ÿ����һ�Σ��ļ�������1  .
//	virtual bool ProcessFile(const char *filename).
//	{.
//		m_nFileCount++;.
//		return CBrowseDir::ProcessFile(filename);.
//	}.
//.
//	//��д�麯��ProcessDir��ÿ����һ�Σ���Ŀ¼������1  .
//	virtual void ProcessDir.
//		(const char *currentdir, const char *parentdir).
//	{.
//		m_nSubdirCount++;.
//		CBrowseDir::ProcessDir(currentdir, parentdir);.
//	}.
//};.

//void main().
//{.
//	//��ȡĿ¼��  .
//	char buf[256];.
//	printf("������Ҫͳ�Ƶ�Ŀ¼��:");.
//	gets(buf);.
//.
//	//���������  .
//	CStatDir statdir;.
//.
//	//����Ҫ������Ŀ¼  .
//	if (!statdir.SetInitDir(buf)).
//	{.
//		puts("Ŀ¼�����ڡ�");.
//		return;.
//	}.
//.
//	//��ʼ����  .
//.
//	vector<string>file_vec;.
//    statdir.BeginBrowseFilenames("*.*",file_vec);.
//	for (vector<string>::const_iterator it = file_vec.begin(); it < file_vec.end(); ++it).
//		std::cout << *it << std::endl;.
//.
//	printf("�ļ�����: %d\n", file_vec.size());.
//	system("pause");.
//}