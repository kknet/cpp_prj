#pragma once
#include "strategy.h"
#include <deque>

using namespace std;
#define KLINE 500

class mhilbert : public strategy
{
	struct Tpars //���߲��Բ���..
	{
		double prema;//��һ�����ڵľ��� ..
		int malen;//��������..
		int obvlen;//�۲촰������..
		int    period;//���׵ķ�������..
		int    lastbarnum;//��һ�εķ���bar���������ڼ������bar��..
		int klinecount;//K�߼���..
		int openkline;//���ַ���..
		int openswitch;//���ֿ���..
		int closeswitch;//ƽ�ֿ���..
		int vol;//������ ..
		string file;//��ʷ�۸����ݴ洢�ļ���ַ..
	};

public:
	mhilbert(string addr);
	~mhilbert();
	void ReSet();
	double cal_mean(deque<double>data,int n);
	void GetIniValue(string addr, char *sect);
	void SetIniValue(string addr, char *sect);
	virtual void OnTick(CThostFtdcDepthMarketDataField& pData);
	void Hlt_base(CThostFtdcDepthMarketDataField& pData);
	virtual void OnOrder(CThostFtdcOrderField& pOrder);
	virtual void OnTrade(CThostFtdcTradeField& pTrade);
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
	Tpars tpar;
	deque<double>cprice,diffprice;
};