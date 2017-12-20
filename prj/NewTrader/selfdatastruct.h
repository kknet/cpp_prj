#pragma once
#include "ThostFtdcUserApiStruct.h"
#include <map>

using namespace std;
typedef string StrategyID;
#define MAX_CODE_SIZE 100


#pragma pack(push,1)
struct orderkey
{
	///ǰ�ñ��..
	TThostFtdcFrontIDType	FrontID;
	///�Ự���..
	TThostFtdcSessionIDType	SessionID;
	///��������..
	TThostFtdcOrderRefType	OrderRef;
	bool operator<(const orderkey& rhs) const
	{
		if (FrontID != rhs.FrontID)
		{
			return (FrontID < rhs.FrontID);
		}
		else if (SessionID != rhs.SessionID)
		{
			return (SessionID < rhs.SessionID);
		}
		else
		{
			return (strcmp(OrderRef, rhs.OrderRef)<0);
		}

	}

	orderkey(){ memset(this, 0, sizeof(orderkey)); }
};

struct tradekey
{
	///����������..
	TThostFtdcExchangeIDType	ExchangeID;
	///���ر������..
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///����������Ա����..
	TThostFtdcTraderIDType	TraderID;
	bool operator<(const tradekey& rhs) const
	{
		if (strcmp(ExchangeID, rhs.ExchangeID) != 0)
		{
			return (strcmp(ExchangeID, rhs.ExchangeID)<0);
		}
		else if (strcmp(TraderID, rhs.TraderID) != 0)
		{
			return (strcmp(TraderID, rhs.TraderID)<0);
		}
		else
		{
			return (strcmp(OrderLocalID, rhs.OrderLocalID)<0);
		}
	}

	tradekey(){ memset(this, 0, sizeof(tradekey)); }
};

struct tradeidkey
{
	///������
	TThostFtdcDateType	TradingDay;
	///�ɽ����
	TThostFtdcTradeIDType	TradeID;

	tradeidkey(){ memset(this, 0, sizeof(tradeidkey)); }
	bool operator<(const tradeidkey& rhs) const
	{
		if (strcmp(TradingDay, rhs.TradingDay) != 0)
		{
			return (strcmp(TradingDay, rhs.TradingDay) < 0);
		}
		else
		{
			return (strcmp(TradeID, rhs.TradeID) < 0);
		}
	}
};

struct poskey//��ʹ���ֲֳ֣�Ҳ�����ֲ�����key..
{
	//����ID..
	StrategyID strID;
	///��Լ����..
	TThostFtdcInstrumentIDType	InstrumentID;
	bool operator<(const poskey& rhs) const
	{
		if (strID != rhs.strID)
		{
			return (strID<rhs.strID);
		}
		else
		{
			return (strcmp(InstrumentID, rhs.InstrumentID)<0);
		}

	}
	poskey(){ memset(this, 0, sizeof(poskey)); }
};

struct inputorder //�����ṹ��..
{
	///����ID..
	StrategyID strID;
	///��Լ����..
	TThostFtdcInstrumentIDType InstrumentID;
	///�����۸�..
	TThostFtdcPriceType LimitPrice;
	///��������..
	TThostFtdcVolumeType Vol;
	///��������..
	TThostFtdcDirectionType Direction;
	///��Ͽ�ƽ�ֱ�־..
	TThostFtdcCombOffsetFlagType CombOffsetFlag;
	///������..
	TThostFtdcRequestIDType	RequestID;
	///׷����־..
	int Purchasing;
	///�Ѿ�׷������..
	int PurNum;
	///���׷������..
	int MaxPurching;
	///׷��ʱ����/��..
	double WaitSecond;
	///��λ�۸�䶯ֵ..
	double UnitTick;
	///�Ƿ�ı俪ƽ�����־.
	bool changeflag;

	inputorder(){ memset(this, 0, sizeof(inputorder)); }
};

struct order_t
{
	///����ID..
	StrategyID strID;
	///Ͷ���ߴ���..
	TThostFtdcInvestorIDType	InvestorID;
	///��Լ����..
	TThostFtdcInstrumentIDType	InstrumentID;
	///��������..
	TThostFtdcOrderRefType	OrderRef;
	///��������..
	TThostFtdcDirectionType	Direction;
	///��Ͽ�ƽ��־..
	TThostFtdcCombOffsetFlagType	CombOffsetFlag;
	///�۸�..
	TThostFtdcPriceType	LimitPrice;
	///����..
	TThostFtdcVolumeType	VolumeTotalOriginal;
	///����״̬..
	TThostFtdcOrderStatusType	OrderStatus;
	///ί��ʱ��..
	TThostFtdcTimeType	InsertTime;
	///ǰ�ñ��..
	TThostFtdcFrontIDType	FrontID;
	///�Ự���..
	TThostFtdcSessionIDType	SessionID;
	///����������..
	TThostFtdcExchangeIDType	ExchangeID;
	///���ر������..
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///����������Ա����..
	TThostFtdcTraderIDType	TraderID;
	///�ɽ�����..
	TThostFtdcVolumeType VolumeTraded;
	///ʣ������..
	TThostFtdcVolumeType VolumeTotal;
	///������..
	TThostFtdcRequestIDType	RequestID;
	///׷����־��0��׷��1׷..
	int                     Purchasing;
	///�Ѿ�׷������..
	int PurNum;
	///���׷������..
	int MaxPurching;
	///���ر���ʱ��..
	double LocalSec;
	///׷��ʱ����/��..
	double WaitSecond;
	///��λ�۸�䶯ֵ..
	double UnitTick;
	///�Ƿ񾭹�ת�� ���ڴ���ƽ�������ѹ���..
	bool Change;

	order_t(){ memset(this, 0, sizeof(order_t)); }
};

struct trade_t
{
	///����ID..
	StrategyID strID;
	///Ͷ���ߴ���..
	TThostFtdcInvestorIDType	InvestorID;
	///��Լ����..
	TThostFtdcInstrumentIDType	InstrumentID;
	///����������..
	TThostFtdcExchangeIDType	ExchangeID;
	///��������..
	TThostFtdcDirectionType	Direction;
	///���ر������..
	TThostFtdcOrderLocalIDType	OrderLocalID;
	///����������Ա����..
	TThostFtdcTraderIDType	TraderID;
	///��ƽ��־..
	TThostFtdcOffsetFlagType	OffsetFlag;
	///Ͷ���ױ���־..
	TThostFtdcHedgeFlagType	HedgeFlag;
	///�۸�..
	TThostFtdcPriceType	Price;
	///����..
	TThostFtdcVolumeType	Volume;
	///�ɽ�ʱ��..
	TThostFtdcTimeType	TradeTime;
	///������..
	TThostFtdcDateType	TradingDay;

	trade_t(){ memset(this, 0, sizeof(trade_t)); }
};

struct position_t
{
	///����ID..
	StrategyID strID;
	///Ͷ���ߴ���..
	TThostFtdcInvestorIDType	InvestorID;
	///��Լ����..
	TThostFtdcInstrumentIDType	InstrumentID;
	///��ͷ�ֲֳɱ�..
	TThostFtdcMoneyType	PosCostLong;
	///���ն�ͷ�ֲ�..
	TThostFtdcVolumeType	YdPosLong;
	///���ն�ͷ�ֲ�..
	TThostFtdcVolumeType	PosLong;
	///��ͷ�ֲֳɱ�..
	TThostFtdcMoneyType	PosCostShort;
	///���տ�ͷ�ֲ�..
	TThostFtdcVolumeType	YdPosShort;
	///���տ�ͷ�ֲ�..
	TThostFtdcVolumeType	PosShort;
	///��ͷ���ֶ���..
	TThostFtdcVolumeType	OLongFrozen;
	///��ͷ���ֶ���..
	TThostFtdcVolumeType	OShortFrozen;
	///��ͷƽ�ֶ���..
	TThostFtdcVolumeType	CLongFrozen;
	///��ͷƽ�ֶ���..
	TThostFtdcVolumeType	CShortFrozen;
	///��֤����.
	TThostFtdcRatioType	MarginRate;
	///��С�䶯��λ.
	TThostFtdcPriceType UnitTick;
	///��Լ��������.
	TThostFtdcVolumeMultipleType	Multiple;

	position_t(){ memset(this, 0, sizeof(position_t)); }
	bool operator==(const position_t &rhs) { return YdPosLong == rhs.YdPosLong && PosLong == rhs.PosLong && YdPosShort == rhs.YdPosShort && PosShort == rhs.PosShort; }
	bool operator!=(const position_t &rhs) { return !(*this == rhs); }
};

struct compar  //ͨ�ò���..
{
	position_t *pos; //���Գֲ���Ϣ..
	int pos_num;//�ֲֺ�Լ�ĸ���..
	//double MarginRate;//��֤�����..
	double MoneyRate;//������ʽ����..
	//double UnitTick;//��λ�۸�..
	//int Multiple;//��Լ����..
	int RequestID;//������..
	bool changeflag;//�Ƿ���Ҫ������ƽ��־.
};

enum postype :char
{
	InstPos = '0',//��Լ�ֲ�..
	StrPos = '1',//���Գֲ�..

};

enum signtype :char //�����ź�����..
{
	CTP_Buy = '0',//��..
	CTP_Sell = '1',//��ƽ..
	CTP_Short = '2',//����..
	CTP_Cover = '3',//��ƽ..
	CTP_None = 'n',//�ǽ����ź�..
	CTP_All = 'c',// ȫ������..
};

enum membertype
{
	ord = 0,//������洢order..
	trd = 1,//������洢trade..
};

struct TR
{
	membertype flag;
	union ot
	{
		CThostFtdcOrderField order;
		CThostFtdcTradeField trade;
	} tuo;
};

struct kline
{
	char updatetime[9];//ʱ��.
	double open;//���̼�.
	double high;//��߼�.
	double low;//��ͼ�.
	double close;//���̼�.
	int  vol;//�ɽ���.
};

struct TradeidStraidItem
{
	TThostFtdcDateType TradingDay;
	TThostFtdcTradeIDType TradeID;
	StrategyID StraId;
};

#pragma pack(pop)

typedef map<orderkey, order_t*> orderinfo_map;
typedef map<tradekey, trade_t> tradeinfo_map;
typedef map<string, position_t> instpos_map;
typedef map<poskey, position_t> strpos_map;
typedef map<string, string> cfg_map;
typedef map<tradekey, orderkey> trade2order_map;//orderkey VS tradekey..
typedef map<tradeidkey, StrategyID> tradeid2strategyid_map;
typedef map<string, bool>open_permit;