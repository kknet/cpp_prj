#pragma once
#include "strategy.h"

using namespace std;
#define KLINE 500

class newllt : public strategy
{
	struct Tpars //���߲��Բ���..
	{
		double coef;//ƽ��ϵ��..
		double bar0;//��ǰ�۸�..
		double bar1;//��һ�����ڼ۸�..
		double bar2;//���������ڼ۸�..
		double llt0;//��ǰƽ��ֵ..
		double llt1;//��һ������ƽ��ֵ..
		double llt2;//����������ƽ��ֵ..
		int    period;//���׵ķ�������..
		int    lastbarnum;//��һ�εķ���bar���������ڼ������bar��..
		int klinecount;//K�߼���..
		int openkline;//���ַ���..
		int openswitch;//���ֿ���..
		int closeswitch;//ƽ�ֿ���..
		int vol;//������ ..
	};

public:
	newllt(string addr);
	~newllt();
	void ReSet();
	virtual void GetIniValue(string addr, char *sect);
	virtual void SetIniValue(string addr, char *sect);
	virtual void OnTick(CThostFtdcDepthMarketDataField& pData);
	virtual void Llt_base(CThostFtdcDepthMarketDataField& pData);
	virtual void OnOrder(CThostFtdcOrderField& pOrder);
	virtual void OnTrade(CThostFtdcTradeField& pTrade);
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

private:
	Tpars tpar;
};