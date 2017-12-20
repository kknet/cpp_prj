#pragma once
#include "Trade.h"
#include "common.h"
#include "browsdir.h"
#include <fstream>

class backtest
{
public:
	backtest(string file_addr);
	~backtest();
	int readfile(string file);//��ȡһ�����������.
	bool getfilenames(string filetype,deque<string>&);//�ҳ�ĳ���ļ�����ָ�����������ļ�������.
	bool getfilenames(string filetype);
	bool run(string filetype);//�����ز�.
	void OnRtnOrder(order_t *pOrder);//�ز��õı����ر�.
	void OnRtnTrade(order_t *pOrder);//�ز��õĳɽ��ر�.
private:
	string file_addr;
	deque<string>filenames;
};


