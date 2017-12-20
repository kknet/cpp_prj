/*��������
1 �������ݽṹ������Ӣ�ĵ��ʵ�Сд
2 ���������������շ�ʽ
3 �����������Ӣ�ĵ���Сд
*/


#pragma once
#include "selfdatastruct.h"
#include <iostream>
#include <deque>
#include <vector>
#include "Trade.h"
#include "Quote.h"
#include "strategy.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#pragma  comment(lib,"WinMM.Lib")
#pragma warning(disable : 4018)

#define BACKTEST 0

#ifdef _DEBUG
	#define SIMNOW 1
	#define LOG 1
#else
	#define SIMNOW 1
	#define LOG 0
#endif

using namespace std;
typedef vector<strategy*> pStr;
struct sqlite3;
/** ��ѯSQlite���ݿ�Ļص����� */
typedef int(*fnOnQuerySQLite)(void*, int, char**, char**);

static string trim(string s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" 0"));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

class common
{
public:
	/// �����ļ�ȫ��Ψһʵ��.
	static common& GetInstance();
	~common();
	bool Start(const char *szfile);
	bool IsMyOrder(orderkey key);
	bool IsMyTrade(tradekey key);
	bool AddStrategy(string ppinst,strategy *pstr);
	int  BeforeMktOpen();
	int  AfterMktClose();
	void Warning();
	void Strategy();
	void CtpFun();
	void CheckOrder2();
	void CreateThread();
	void ReleaseThread();
	void StartSqlite();
	void StopSqlite();
	// �����߳�.
	void TrdFunTester();
	/*
	ԭʼ����������
	�������ĳֲַֽ�֣����տ��֣�����֣���ʷ�ֲ֣���ƽ��ʱ��Ҫָ����ƽ��ֻ�����֡�
	�����ֶ��У� �����������ĳֲ�ֱ��ʹ��THOST_FTDC_OF_Close �� ��Ч��ͬʹ��
	THOST_FTDC_OF_CloseYesterday �����������������ĳֲ�ʹ����THOST_FTDC_OF_CloseToday ��
	THOST_FTDC_OF_CloseYesterday����Ч��ͬʹ��THOST_FTDC_OF_Close��
	ע�⣺�����ļҽ�����ƽ��˳��ͳһΪ�ȿ���ƽ���������ڴ˻����ϻ�����ƽ���Ȳ���ƽ��ϲֵĹ���
	
	���� change���Ƿ񿪷��򵥣�Ϊ���ƽ�������Ѹߵ�����
	*/
	void SendOrder(inputorder& porder);
	/*
	���ƽ��������Ѹߵ����⣺
	1.���֣���ƽ��������֣�Ȼ��ʣ������
	2.ƽ�֣���ƽ����֣�Ȼ�󿪷������ʣ������
	*/
	void SendOrder2(inputorder& porder);
	int  CheckOrder(string strname, signtype signal);
	void CancelOrder(order_t *order);
	void CalPos(CThostFtdcTradeField *pTrade,postype type);
	void CalFrozen(CThostFtdcOrderField& order,order_t *porder);
	// ���̺��ctp�õ��� �������ݿ⣬�����ڴ������Աȣ���ͬ�򱨾�.
	bool ComparePos_Mem2Ctp();
	bool Insert_Pos();
	// �����ݿ�Tradeid2Strid_updating�����Ȼ��������µ�����vec_tradeid_straid_
	void UpdateTradeid2Strid();

	int iNumber=0;//��Լ��Ŀ.
	int dayofweek = 0;//���ڼ�����ֹ��ĩ����������.
	double moneyavaible;//�����ʽ�.
	char* ppInst[MAX_CODE_SIZE];//��Լ����.
	bool qryflag = true;//��ѯ���Ʊ���.

	bool releaseTrade = false;
	bool initTrade = false;

	//���ò���.

	TThostFtdcInvestorIDType INVESTOR_ID;			// Ͷ���ߴ���.
	TThostFtdcBrokerIDType	BROKER_ID;				// ���͹�˾����.
	TThostFtdcPasswordType  PASSWORD;			// �û�����.
	char tdfront[MAX_CODE_SIZE];                              // ����ǰ��.
	char tdfront1[MAX_CODE_SIZE];                            // ����ǰ��.
	char mdfront[MAX_CODE_SIZE];                         // ����ǰ��.
	char mdfront1[MAX_CODE_SIZE];                            // ����ǰ��.

	//�����ͳֲֶ���.

	vector<CThostFtdcDepthMarketDataField> mktdata; //�������.
	deque<TR>trseq; //�����ͳɽ�����.
	map<string, CThostFtdcDepthMarketDataField>LatestMarket;//��������.
	map<string, pStr>pStrmap;//��Լ��������Ĳ��Զ���ָ��vector���ɵ�map.
	map<StrategyID, strategy*>pStrID;//����ID����Զ���ָ�빹�ɵ�map.
	orderinfo_map orderinfo; //������Ψһ��ʶ��¼�Ķ���map.
	tradeinfo_map tradeinfo; //��ÿ�ʳɽ�Ψһ��ʶ��¼�ĳɽ�map.
	orderinfo_map orderbook; //ֻ�������ڽ��׵Ķ�����ָ��,Ҳ����ί�ж�����δ�ɽ��Ķ���.
	strpos_map strpos; //�����Է����¼�ĳֲ�map.
	instpos_map instpos; //��Ʒ�ַ����¼�ĳֲ�map.
	// ���̺��ctp�õ��� �������ݿ⣬�����ڴ������Աȣ���ͬ�򱨾�.
	strpos_map strpos2; //�����Է����¼�ĳֲ�map.
	instpos_map instpos2; //��Ʒ�ַ����¼�ĳֲ�map.
	cfg_map cfgfile;//�����ļ�.
	trade2order_map tmapo; //trade key��order key��ӳ��..
	tradeid2strategyid_map map_tradeid_straid_; //tradeidkey��strategy id��ӳ�� ���ڷ�����ҹ��..
	vector<TradeidStraidItem> vec_tradeid_straid_;
	open_permit openpermit;//��Լ�ܷ񿪲�.
	map<string, char>inststatus;//��Լ����״̬.
	std::condition_variable c_strategy, c_ctp, c_warn, c_checkorder,c_main, c_reqPosdetail;
	std::mutex m_strategy, m_ctp, m_warn, m_checkorder, m_main, m_reqPosdetail;
	bool ctpflag = false, strflag = false, warnflag = false, checkflag = false;
	std::thread T_ctp, T_str, T_warn, T_checkorder, T_tester;
	CRITICAL_SECTION cs_strategy;

	// �Ự����.

	TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��.
	TThostFtdcSessionIDType	SESSION_ID;	//�Ự���.
	TThostFtdcOrderRefType	ORDER_REF;	//��������.
	TThostFtdcDirectionType Direction;  //���׷���.
	int nReq=0;                         //����Ự���.
	typedef pair<string, string>Entry;
	CThostFtdcMdApi* md;
	CThostFtdcTraderApi* api;
	Trade *t;//������ص�.
	Quote *q;//������ص�.

	sqlite3 *sqlite3_;
	void SQLite_TradekeyStrid_Callback(int num, char **values, char **colName);
	void SQLite_StraPos_Callback(int num, char **values, char **colName);
	void SQLite_InstPos_Callback(int num, char **values, char **colName);
private:
	bool ReqPosDetail();
	bool SQLite_Query_TradekeyStrid();//�ֲ���ϸ.Tradekey -> �����ĸ�����..
	bool SQLite_Query_Pos(); // ����Գֲ֡���Լ�ֲ�..
	// �������ݿ�.
	bool SQLite_Insert_StraPos(const poskey&, position_t&);
	bool SQLite_Insert_InstPos(const string&, position_t&);
	bool SQLite_Insert_TradekeyStrid(TThostFtdcDateType &, TThostFtdcTradeIDType &, StrategyID &);
	bool _SQLite_EXEC(const char *SQL, fnOnQuerySQLite cb = nullptr, void *p = nullptr);

	bool loaded;

	common() :loaded(false), sqlite3_(NULL) { }
};