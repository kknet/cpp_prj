#pragma once
//#include "common.h"..
#include "strategy.h"
#include <iostream>
#include <deque>
#include <map>
using namespace std;

struct pars //���Բ���..
{
	int	N;//���ϵ����������..
	double basis;//�۲���Сֵ..
	double min_coe;//��С���ϵ��..
	double max_coe;//������ϵ��..
	double upLimit;//ƫ���׼���..
	double upLimit_sell;//ֹӯϵ��..
	double upLimit_lose;//ֹ��ϵ��..
	double trackLose;//�����������ϵ��..
	int    period;//���׵ķ�������..
	int    lastbarnum;//��һ�εķ���bar���������ڼ������bar��..
	string file1;//��Լ1����ʷ���������ļ���..
	string file2;//��Լ2����ʷ���������ļ���..
};

struct Tpars //��������..
{
	double moneyrate;//�ʽ����..
	double marginrate;//��֤�����..
	double unittick;//��Լ��λtickֵ..
	int klinecount;//K�߼���..
	int openkline;//���ַ���..
	int holdvol;//�ֲ���..
	int startjudgetime;//���������ж�ʱ��..
	int terminatejudgetime;//���������ж�ʱ��..
	int closetime;//β��ƽ��ʱ��..
	int vol;//������ ..
};

typedef deque<double> darray;

class arb1 : public strategy
{
public:
	arb1(string addr);
	virtual ~arb1();
	virtual bool init(string symbol_addr);//���Գ�ʼ�����������Ϊ��Լ�����ļ���ַ�����ܰ�����ȡ�����ļ�����ȡ��ʷ��Ϣ..
	virtual void getsymbol(string addr);//��ȡ��Լ������Ϣ..
	virtual void getinivalue(string addr);//��ȡ�����ļ���Ϣ..
	virtual double  cal_mean(darray data,int n=0);//�����ֵ..
	virtual void  arb_strategy(CThostFtdcDepthMarketDataField& pData);//���������ź�..
	virtual void  tf_strategy(CThostFtdcDepthMarketDataField& pData);//���Ʋ����ź�..
	virtual double  cal_std(darray data,int n=0);//�����׼��..
	virtual double cal_corr(darray data1, darray data2, int n = 0);//�������ϵ��..
	virtual void   cal_spread(darray data1, darray data2);//����۲�..
	virtual double cal_cov(darray data1, darray data2,int n=0);//����Э����..
	virtual void OnTick(CThostFtdcDepthMarketDataField& pData);
	virtual void OnOrder(CThostFtdcOrderField& pOrder);
	virtual void OnTrade(CThostFtdcTradeField& pTrade);
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

protected:
	bool inited = false, SignUpdateFlag = false;
	string strinfo_addr = "./arb1info.ini";//�����ź����ݵ�ַ..
	pars inipar;//����ini�ļ��еĲ���..
	darray close1,close2,spread;
	CThostFtdcDepthMarketDataField preData1,preData2;
	Tpars spar;
	int  status = 0;
};