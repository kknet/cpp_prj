#include "browsdir.h"

CBrowseDir::CBrowseDir(const char *filepath)
{
	//��filepath��ʼ��m_szInitDir  .
	strncpy_s(m_szInitDir, filepath, _MAX_PATH);

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\'  .
	int len = strlen(m_szInitDir);
	if (m_szInitDir[len - 1] != '\\' && m_szInitDir[len - 1] != '/')
		strcat_s(m_szInitDir, "\\");
}

bool CBrowseDir::SetInitDir(const char *dir)
{

	//�ж�Ŀ¼�Ƿ����  .
	if (_chdir(m_szInitDir) != 0)
		return false;

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\'  .
	int len = strlen(m_szInitDir);
	if (m_szInitDir[len - 1] != '\\' && m_szInitDir[len - 1] != '/')
		strcat_s(m_szInitDir, "\\");

	return true;
}


bool CBrowseDir::BrowseDir(const char *dir, const char *filespec)
{
	_chdir(dir);

	//���Ȳ���dir�з���Ҫ����ļ�  .
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼  .
			//�������,����д���  .
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				char filename[_MAX_PATH];
				strcpy_s(filename, dir);
				strcat_s(filename, fileinfo.name);
				cout << filename << endl;
				if (!ProcessFile(filename))
					return false;
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	//����dir�е���Ŀ¼  .
	//��Ϊ�ڴ���dir�е��ļ�ʱ���������ProcessFile�п��ܸı���  .
	//��ǰĿ¼����˻�Ҫ�������õ�ǰĿ¼Ϊdir��  .
	//ִ�й�_findfirst�󣬿���ϵͳ��¼���������Ϣ����˸ı�Ŀ¼  .
	//��_findnextû��Ӱ�졣  .
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼  .
			//�����,�ټ���ǲ��� . �� ..   .
			//�������,���е���  .
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp
					(fileinfo.name, "..") != 0)
				{
					char subdir[_MAX_PATH];
					strcpy_s(subdir, dir);
					strcat_s(subdir, fileinfo.name);
					strcat_s(subdir, "\\");
					ProcessDir(subdir, dir);
					if (!BrowseDir(subdir, filespec))
						return false;
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}

void CBrowseDir::GetDirFilenames(const char *dir, const char *filespec, deque<string>& filenames)
{
	_chdir(dir);

	//���Ȳ���dir�з���Ҫ����ļ�  .
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼  .
			//�������,����д���  .
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				char filename[_MAX_PATH];
				strcpy(filename, dir);
				strcat(filename, fileinfo.name);
				filenames.push_back(filename);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	//����dir�е���Ŀ¼  .
	//��Ϊ�ڴ���dir�е��ļ�ʱ���������ProcessFile�п��ܸı���  .
	//��ǰĿ¼����˻�Ҫ�������õ�ǰĿ¼Ϊdir��  .
	//ִ�й�_findfirst�󣬿���ϵͳ��¼���������Ϣ����˸ı�Ŀ¼  .
	//��_findnextû��Ӱ�졣  .
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼  .
			//�����,�ټ���ǲ��� . �� ..   .
			//�������,���е���  .
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp
					(fileinfo.name, "..") != 0)
				{
					char subdir[_MAX_PATH];
					strcpy(subdir, dir);
					strcat(subdir, fileinfo.name);
					strcat(subdir, "\\");
					ProcessDir(subdir, dir);
					deque<string>tmp;
					GetDirFilenames(subdir, filespec, tmp);
					for (deque<string>::iterator it = tmp.begin(); it<tmp.end(); it++)
					{
						filenames.push_back(*it);
					}
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

bool CBrowseDir::ProcessFile(const char *filename)
{
	return true;
}

void CBrowseDir::ProcessDir(const char
	*currentdir, const char *parentdir)
{
}