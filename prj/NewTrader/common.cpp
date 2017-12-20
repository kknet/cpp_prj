#include "common.h"
#include "windows.h"
#include "backtest.h"
#include <fstream>
#include <functional>
#include "sqlite3.h"

#pragma warning(disable : 4996)

#if BACKTEST
extern backtest *pback;
#endif

using namespace std;
extern CThostFtdcTraderApi* api;
static const char *g_db_name = "ctp_trade_info.db";
static const char *g_crt_tbl_TradekeyStrid =
"create table if not exists Tradeid2Strid("
"TradingDay varchar(9), TradeID varchar(21), StrategyID varchar(21),"
"primary key(TradingDay, TradeID));";
//ʵʱ���µ� ֻ��������.
static const char *g_crt_tbl_TradekeyStrid_updating =
"create table if not exists Tradeid2Strid_updating("
"TradingDay varchar(9), TradeID varchar(21), StrategyID varchar(21),"
"primary key(TradeID));";

static const char *g_crt_tbl_Stra2Pos =
"create table if not exists Stra2Pos("
"StrategyID varchar(21), InstrumentID varchar(31),"
"InvestorID varchar(13), PosCostLong double, YdPosLong int,"
"PosLong int, PosCostShort double, YdPosShort int, PosShort int,"
"OLongFrozen int, OShortFrozen int, CLongFrozen int, CShortFrozen int,"
"MarginRate double, UnitTick double, Multiple int,"
"primary key(StrategyID, InstrumentID));";
static const char *g_crt_tbl_Inst2Pos =
"create table if not exists Inst2Pos("
"StrategyID varchar(21), InstrumentID varchar(31),"
"InvestorID varchar(13), PosCostLong double, YdPosLong int,"
"PosLong int, PosCostShort double, YdPosShort int, PosShort int,"
"OLongFrozen int, OShortFrozen int, CLongFrozen int, CShortFrozen int,"
"MarginRate double, UnitTick double, Multiple int,"
"primary key(InstrumentID));";

common& common::GetInstance()
{
	static common info;
	return info;
}

common::~common(){
	//ReleaseThread();.
	//StopSqlite();
}

void common::CalFrozen(CThostFtdcOrderField& order, order_t *myorder)
{
	poskey pkey;
	pkey.strID = myorder->strID;
	strcpy_s(pkey.InstrumentID, order.InstrumentID);
	if (strpos.count(pkey) <= 0)
	{
		position_t pos;
		memset(&pos, 0, sizeof(pos));
		strcpy_s(pos.InstrumentID, myorder->InstrumentID);
		strcpy_s(pos.InvestorID, myorder->InvestorID);
		pos.strID = myorder->strID;
		auto check = strpos.insert(make_pair(pkey, pos));
		if (!check.second)
		{
			cerr << "insert to strpos failure!" << endl;
		}
	}
	position_t& rpos = strpos[pkey];
	if (order.OrderSubmitStatus == THOST_FTDC_OSS_InsertSubmitted && order.OrderStatus == THOST_FTDC_OST_Unknown) //�����ύ����������Ϊί������.
	{
		if (order.Direction == THOST_FTDC_D_Buy)
		{
			if (order.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
			{
				rpos.OLongFrozen += (order.VolumeTotalOriginal - myorder->VolumeTotal);
			}
			else
			{
				rpos.CLongFrozen += (order.VolumeTotalOriginal - myorder->VolumeTotal);
			}
		}
		else
		{
			if (order.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
			{
				rpos.OShortFrozen += (order.VolumeTotalOriginal - myorder->VolumeTotal);
			}
			else
			{
				rpos.CShortFrozen += (order.VolumeTotalOriginal - myorder->VolumeTotal);
			}
		}
	}
	else if (order.OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing || order.OrderStatus == THOST_FTDC_OST_Canceled ||
		order.OrderStatus == THOST_FTDC_OST_AllTraded || order.OrderStatus == THOST_FTDC_OST_NoTradeNotQueueing) //�����Ѿ�Ϊ����״̬�������ڶ���.
	{
		if (order.Direction == THOST_FTDC_D_Buy)
		{
			if (order.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
			{
				rpos.OLongFrozen -= myorder->VolumeTotal;
			}
			else
			{
				rpos.CLongFrozen -= myorder->VolumeTotal;
			}
		}
		else
		{
			if (order.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
			{
				rpos.OShortFrozen -= myorder->VolumeTotal;
			}
			else
			{
				rpos.CShortFrozen -= myorder->VolumeTotal;
			}
		}
	}
	else //�����м�״̬����Ҫ������ʣ���������в��������õ����¶�������.
	{
		if (order.Direction == THOST_FTDC_D_Buy)
		{
			if (order.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
			{
				rpos.OLongFrozen += (order.VolumeTotal - myorder->VolumeTotal);
			}
			else
			{
				rpos.CLongFrozen += (order.VolumeTotal - myorder->VolumeTotal);
			}
		}
		else
		{
			if (order.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
			{
				rpos.OShortFrozen += (order.VolumeTotal - myorder->VolumeTotal);
			}
			else
			{
				rpos.CShortFrozen += (order.VolumeTotal - myorder->VolumeTotal);
			}
		}
	}
	ofstream out("position.txt", ios_base::app);
	if (!out)
	{
		cerr << "open positionfile failure" << endl;
	}
	else
	{
		out << "strname:" << rpos.strID.c_str() << ",";
		out << "inst:" << rpos.InstrumentID << ",";
		out << "tradedate:" << order.TradingDay << ",";
		out << "tradetime: " << order.InsertTime << ",";
		out << "OC:" << order.CombOffsetFlag[0] << ",";
		out << "direction: " << order.Direction << ",";
		out << "OLongFrozen: " << rpos.OLongFrozen << ",";
		out << "OShortFrozen: " << rpos.OShortFrozen << ",";
		out << "CLongFrozen: " << rpos.CLongFrozen << ",";
		out << "CShortFrozen: " << rpos.CShortFrozen << endl;
		out.close();
	}
}


int common::CheckOrder(string strname, signtype signal)
{
	int onordernum = 0;
	//std::unique_lock<std::mutex> lock(m_ctp);.
	if (!orderinfo.empty())
	{
		for (orderinfo_map::iterator itor = orderinfo.begin(); itor != orderinfo.end(); itor++)
		{
			order_t* order = itor->second;
			if (!order->strID.empty() && strname == order->strID && order->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing &&
				order->OrderStatus != THOST_FTDC_OST_Canceled && order->OrderStatus != THOST_FTDC_OST_AllTraded &&
				order->OrderStatus != THOST_FTDC_OST_NoTradeNotQueueing)
			{
				switch (signal)
				{
				case CTP_Buy: //��.
				{
								  if (order->Direction == THOST_FTDC_D_Buy && order->CombOffsetFlag[0] == THOST_FTDC_OF_Open)
								  {
									  CancelOrder(order);
								  }
								  break;
				}
				case CTP_Sell: //��ƽ.
				{
								   if (order->Direction == THOST_FTDC_D_Sell && order->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday)
								   {
									   CancelOrder(order);
								   }
								   break;
				}
				case CTP_Short: //����.
				{
									if (order->Direction == THOST_FTDC_D_Sell && order->CombOffsetFlag[0] == THOST_FTDC_OF_Open)
									{
										CancelOrder(order);
									}
									break;
				}
				case CTP_Cover: //��ƽ.
				{
									if (order->Direction == THOST_FTDC_D_Buy && order->CombOffsetFlag[0] == THOST_FTDC_OF_CloseToday)
									{
										CancelOrder(order);
									}
									break;
				}
				case CTP_All: //ȫ��.
				{
								  CancelOrder(order);
								  break;
				}
				default:
				{
						   break;
				}
				}
				onordernum++;
			}
		}
	}
	return onordernum; //ָ��������;����ͳ�ƽ��.
}

bool common::Start(const char *szfile)
{
	if (loaded) return false;

	ifstream cfgFile(szfile);
	if (!cfgFile)
	{
		cout << "can not open cfg file!" << endl;
		return false;
	}
	char tmp[200];
	while (!cfgFile.eof())//ѭ����ȡÿһ��   .
	{
		cfgFile.getline(tmp, 200);//ÿ�ж�ȡǰ200���ַ���200��Ӧ���㹻��   .
		string line(tmp);
		size_t pos = line.find('=');//�ҵ�ÿ�еġ�=����λ�ã�֮ǰ��key֮����value   .
		if (pos == string::npos) continue;
		string key = line.substr(0, pos);//ȡ=��֮ǰ.
		string value = line.substr(pos + 1);
		if (strcmp(key.c_str(), "ppInst") == 0)
		{
			string::size_type pos;
			string pattern = ",";
			value += pattern;//��չ�ַ����Է������.
			unsigned int size = value.size();

			for (unsigned int i = 0; i < size; i++)
			{
				pos = value.find(pattern, i);
				if (pos < size)
				{
					std::string s = value.substr(i, pos - i);
					ppInst[iNumber] = new char[10];
					strcpy(ppInst[iNumber], s.c_str());
					size_t loc = s.find('1');
					string skey = s.substr(0, loc);
					if (inststatus.count(skey) <= 0)
					{
						auto check = inststatus.insert(make_pair(skey, '2'));
						if (!check.second)
						{
							cerr << "insert to instrument status failure!" << endl;
						}
					}
					iNumber++;
					openpermit[s] = true;
					i = pos + pattern.size() - 1;
				}
			}
		}
		else if (strcmp(key.c_str(), "INVESTOR_ID") == 0)
		{
			strcpy(INVESTOR_ID, value.c_str());
		}
		else if (strcmp(key.c_str(), "PASSWORD") == 0)
		{
			strcpy(PASSWORD, value.c_str());
		}
		else if (strcmp(key.c_str(), "BROKER_ID") == 0)
		{
			strcpy(BROKER_ID, value.c_str());
		}
		else if (strcmp(key.c_str(), "tdfront1") == 0)
		{
			strcpy(tdfront1, value.c_str());
		}
		else if (strcmp(key.c_str(), "tdfront") == 0)
		{
			strcpy(tdfront, value.c_str());
		}
		else if (strcmp(key.c_str(), "mdfront") == 0)
		{
			strcpy(mdfront, value.c_str());
		}
		else if (strcmp(key.c_str(), "mdfront1") == 0)
		{
			strcpy(mdfront1, value.c_str());
		}
		Entry p = make_pair(key, value);
		auto insertvalue = cfgfile.insert(p);
		if (!insertvalue.second)
		{
			cerr << "insert failure!" << endl;
		}
	}
	FRONT_ID = 0;
	SESSION_ID = 0;
	strcpy_s(ORDER_REF, "00");
	InitializeCriticalSection(&cs_strategy);
	mktdata.reserve(2 * iNumber);
	loaded = true;
	return true;
}

bool common::ComparePos_Mem2Ctp()
{
	for (strpos_map::iterator iter = strpos2.begin(); iter != strpos2.end(); ++iter){
		strpos_map::iterator iter2 = strpos.find(iter->first);
		if (iter2 == strpos.end() || iter->second != iter2->second)
			return false;
	}
	for (instpos_map::iterator iter = instpos2.begin(); iter != instpos2.end(); ++iter){
		instpos_map::iterator iter2 = instpos.find(iter->first);
		if (iter2 == instpos.end() || iter->second != iter2->second)
			return false;
	}
	return true;
}

bool common::Insert_Pos()
{
	SYSTEMTIME ct;
	GetLocalTime(&ct);
	int h, m, s;
	h = ct.wHour;
	m = ct.wMinute;
	s = ct.wSecond;
	long long ss = 10000 * h + 100 * m + s;
	if (ss>=151500 && ss<=210000){
		// ������� �����ת�Ƶ����.
		for (strpos_map::iterator iter = strpos2.begin(); iter != strpos2.end(); ++iter)
		{
			iter->second.YdPosLong += iter->second.PosLong;
			iter->second.PosLong = 0;
			iter->second.YdPosShort += iter->second.PosShort;
			iter->second.PosShort = 0;
		}
	}
	for (strpos_map::iterator iter = strpos2.begin(); iter != strpos2.end(); ++iter){
		if (!SQLite_Insert_StraPos(iter->first, iter->second))
			return false;
	}
	for (instpos_map::iterator iter = instpos2.begin(); iter != instpos2.end(); ++iter){
		if (!SQLite_Insert_InstPos(iter->first, iter->second))
			return false;
	}
	return true;
}

void common::UpdateTradeid2Strid()
{
	char *sql = "delete * from Tradeid2Strid_updating;";
	_SQLite_EXEC(sql);
	for (vector<TradeidStraidItem>::iterator iter = vec_tradeid_straid_.begin(); iter != vec_tradeid_straid_.end(); ++iter)
	{
		SQLite_Insert_TradekeyStrid(iter->TradingDay, iter->TradeID, iter->StraId);
	}
}

bool common::ReqPosDetail()
{
	vec_tradeid_straid_.clear();
	strpos2.clear();
	instpos2.clear();
	CThostFtdcQryInvestorPositionDetailField req = { 0 };
	strcpy_s(req.BrokerID, cfgfile["BROKER_ID"].c_str());
	strcpy_s(req.InvestorID, cfgfile["INVESTOR_ID"].c_str());
	int Res = 0, loop_num = 5;
	do {
		Sleep(2000);
		Res = api->ReqQryInvestorPositionDetail(&req, ++nReq);
		cout << "�ֲ���ϸ��ѯ��" << ((Res == 0) ? "�ɹ�" : "ʧ��") << endl;
	} while (Res != 0 && loop_num-- > 0/*comm.qryflag*/);
	if (loop_num > 0)
		return true;
	else
		return false;
}

int common::AfterMktClose()
{
	std::unique_lock<std::mutex> lock(m_main);
	c_main.wait(lock);
	ReleaseThread();
	for (int i = 0; i < iNumber; i++)
	{
		delete[] ppInst[i];
		ppInst[i] = NULL;
	}

	for (map<orderkey, order_t*>::iterator it = orderinfo.begin(); it != orderinfo.end(); it++)
	{
		delete it->second;
		it->second = NULL;
	}

	for (map<orderkey, order_t*>::iterator it = orderbook.begin(); it != orderbook.end(); it++)
	{
		it->second = NULL;
	}

	StopSqlite();
	return 0;
}
int common::BeforeMktOpen()
{
	t = new Trade();
	CreateThread();
	StartSqlite();
#if !BACKTEST
	t->Start();
#endif
	return 0;
}

bool common::IsMyOrder(orderkey key)
{
	return (orderinfo.count(key) > 0);
}

bool common::IsMyTrade(tradekey key)
{
	return (tmapo.count(key) > 0);
}

void common::CalPos(CThostFtdcTradeField *pTrade, postype type)
{
	tradekey key;
	strcpy_s(key.ExchangeID, pTrade->ExchangeID);
	strcpy_s(key.OrderLocalID, pTrade->OrderLocalID);
	strcpy_s(key.TraderID, pTrade->TraderID);

	orderkey& okey = tmapo[key];
	order_t* order = orderinfo[okey];
	position_t *ppos = NULL;
	if (type == postype::InstPos)
	{
		if (instpos.count(pTrade->InstrumentID) <= 0)
		{
			position_t pos;
			memset(&pos, 0, sizeof(pos));
			strcpy_s(pos.InstrumentID, pTrade->InstrumentID);
			strcpy_s(pos.InvestorID, pTrade->InvestorID);
			pos.strID = order->strID;
			auto check = instpos.insert(make_pair(pTrade->InstrumentID, pos));
			if (!check.second)
			{
				cerr << "insert to strpos failure!" << endl;
			}
		}
		ppos = &instpos[pTrade->InstrumentID];
	}
	else if (type == postype::StrPos)
	{
		poskey pkey;
		pkey.strID = order->strID;
		strcpy_s(pkey.InstrumentID, pTrade->InstrumentID);
		if (strpos.count(pkey) <= 0)
		{
			position_t pos;
			memset(&pos, 0, sizeof(pos));
			strcpy_s(pos.InstrumentID, pkey.InstrumentID);
			strcpy_s(pos.InvestorID, pTrade->InvestorID);
			pos.strID = pkey.strID;
			auto check = strpos.insert(make_pair(pkey, pos));
			if (!check.second)
			{
				cerr << "insert to strpos failure!" << endl;
			}
		}
		ppos = &strpos[pkey];
	}
	if (pTrade->OffsetFlag == THOST_FTDC_OF_Open)
	{
		if (pTrade->Direction == THOST_FTDC_D_Buy)
		{
			double amount = (ppos->PosCostLong*(ppos->PosLong + ppos->YdPosLong) + pTrade->Volume*pTrade->Price);
			TThostFtdcVolumeType vol = (ppos->PosLong + ppos->YdPosLong + pTrade->Volume);
			ppos->PosCostLong = amount / vol;
			cout << "Otype:" << type << endl;
			cout << "Ooriginallomg:" << ppos->PosLong << endl;
			ppos->PosLong += pTrade->Volume;
			cout << "Onewlong:" << ppos->PosLong << endl;
		}
		else if (pTrade->Direction == THOST_FTDC_D_Sell)
		{
			double amount = (ppos->PosCostShort*(ppos->PosShort + ppos->YdPosShort) + pTrade->Volume*pTrade->Price);
			TThostFtdcVolumeType vol = (ppos->PosShort + ppos->YdPosShort + pTrade->Volume);
			ppos->PosCostShort = amount / vol;
			cout << "Otype:" << type << endl;
			cout << "Ooriginalshort:" << ppos->PosShort << endl;
			ppos->PosShort += pTrade->Volume;
			cout << "Onewshort:" << ppos->PosShort << endl;
		}
	}
	if (pTrade->OffsetFlag == THOST_FTDC_OF_Close || pTrade->OffsetFlag == THOST_FTDC_OF_CloseToday)
	{
		if (pTrade->Direction == THOST_FTDC_D_Buy)
		{
			double amount = (ppos->PosCostShort*(ppos->PosShort + ppos->YdPosShort) - pTrade->Volume*pTrade->Price);
			TThostFtdcVolumeType vol = (ppos->PosShort + ppos->YdPosShort - pTrade->Volume);
			if (vol >= 1)
			{
				ppos->PosCostShort = amount / vol;
			}
			else
			{
				ppos->PosCostShort = 0;
			}
			cout << "Ctype:" << type << endl;
			cout << "Coriginalshort:" << ppos->PosShort << endl;
			ppos->PosShort -= pTrade->Volume;
			cout << "Cnewshort:" << ppos->PosShort << endl;
		}
		else if (pTrade->Direction == THOST_FTDC_D_Sell)
		{
			double amount = (ppos->PosCostLong*(ppos->PosLong + ppos->YdPosLong) - pTrade->Volume*pTrade->Price);
			TThostFtdcVolumeType vol = (ppos->PosLong + ppos->YdPosLong - pTrade->Volume);
			if (vol >= 1)
			{
				ppos->PosCostLong = amount / vol;
			}
			else
			{
				ppos->PosCostLong = 0;
			}
			cout << "Ctype:" << type << endl;
			cout << "Coriginallomg:" << ppos->PosLong << endl;
			ppos->PosLong -= pTrade->Volume;
			cout << "Cnewlong:" << ppos->PosLong << endl;
		}
	}
	if (pTrade->OffsetFlag == THOST_FTDC_OF_CloseYesterday)
	{
		if (pTrade->Direction == THOST_FTDC_D_Buy)
		{
			double amount = (ppos->PosCostShort*(ppos->PosShort + ppos->YdPosShort) - pTrade->Volume*pTrade->Price);
			TThostFtdcVolumeType vol = (ppos->PosShort + ppos->YdPosShort - pTrade->Volume);
			if (vol >= 1)
			{
				ppos->PosCostShort = amount / vol;
			}
			else
			{
				ppos->PosCostShort = 0;
			}
			ppos->YdPosShort -= pTrade->Volume;
		}
		else if (pTrade->Direction == THOST_FTDC_D_Sell)
		{
			double amount = (ppos->PosCostLong*(ppos->PosLong + ppos->YdPosLong) - pTrade->Volume*pTrade->Price);
			TThostFtdcVolumeType vol = (ppos->PosLong + ppos->YdPosLong - pTrade->Volume);
			if (vol >= 1)
			{
				ppos->PosCostLong = amount / vol;
			}
			else
			{
				ppos->PosCostLong = 0;
			}
			ppos->YdPosLong -= pTrade->Volume;
		}
	}
	if (type == postype::InstPos)
	{
		ofstream out("instpos.txt", ios_base::app);
		if (!out)
		{
			cerr << "open errfile failure" << endl;
		}
		else
		{
			out << "strname:" << ppos->strID.c_str() << ",";
			out << "inst:" << ppos->InstrumentID << ",";
			out << "tradedate:" << pTrade->TradeDate << ",";
			out << "tradetime: " << pTrade->TradeTime << ",";
			out << "OC:" << pTrade->OffsetFlag << ",";
			out << "direction: " << pTrade->Direction << ",";
			out << "PosLong: " << ppos->PosLong << ",";
			out << "PosShort: " << ppos->PosShort << ",";
			out << "YdPosLong: " << ppos->YdPosLong << ",";
			out << "YdPosShort: " << ppos->YdPosShort << ",";
			out << "PosCostLong: " << ppos->PosCostLong << ",";
			out << "PosCostShort: " << ppos->PosCostShort << endl;
			out.close();
		}
	}
	if (type == postype::StrPos)
	{
		ofstream out("strpos.txt", ios_base::app);
		if (!out)
		{
			cerr << "open errfile failure" << endl;
		}
		else
		{
			out << "strname:" << ppos->strID.c_str() << ",";
			out << "inst:" << ppos->InstrumentID << ",";
			out << "tradedate:" << pTrade->TradeDate << ",";
			out << "tradetime: " << pTrade->TradeTime << ",";
			out << "OC:" << pTrade->OffsetFlag << ",";
			out << "direction: " << pTrade->Direction << ",";
			out << "PosLong: " << ppos->PosLong << ",";
			out << "PosShort: " << ppos->PosShort << ",";
			out << "YdPosLong: " << ppos->YdPosLong << ",";
			out << "YdPosShort: " << ppos->YdPosShort << ",";
			out << "PosCostLong: " << ppos->PosCostLong << ",";
			out << "PosCostShort: " << ppos->PosCostShort << endl;
			out.close();
		}
	}
}

void common::CancelOrder(order_t *order)
{
	if (order->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing &&  order->OrderStatus != THOST_FTDC_OST_Canceled
		&& order->OrderStatus != THOST_FTDC_OST_AllTraded && order->OrderStatus != THOST_FTDC_OST_NoTradeNotQueueing)
	{
		CThostFtdcInputOrderActionField req;
		memset(&req, 0, sizeof(req));
		///���͹�˾����.
		strcpy_s(req.BrokerID, BROKER_ID);
		///Ͷ���ߴ���.
		strcpy_s(req.InvestorID, order->InvestorID);
		///��������.
		strcpy_s(req.OrderRef, order->OrderRef);
		///ǰ�ñ��.
		req.FrontID = order->FrontID;
		///�Ự���.
		req.SessionID = order->SessionID;
		///������־.
		req.ActionFlag = THOST_FTDC_AF_Delete;
		strcpy_s(req.InstrumentID, order->InstrumentID);
		//strcpy_s(req.ExchangeID, order->ExchangeID);.
		//strcpy_s(req.OrderLocalID, order.OrderLocalID);.
#if !BACKTEST
		string key = order->InstrumentID;
		size_t loc = key.find('1');
		string skey = key.substr(0, loc);
		if (inststatus[skey] == THOST_FTDC_IS_Continous || inststatus[skey] == THOST_FTDC_IS_AuctionOrdering)
		{
			int iResult = api->ReqOrderAction(&req, ++nReq);
			cerr << "--->>> OrderAction: " << ((iResult == 0) ? "Success" : "Failure") << endl;
		}
		/*
		else if (inststatus[skey] == THOST_FTDC_IS_BeforeTrading || inststatus[skey] == THOST_FTDC_IS_NoTrading)
		{
		CThostFtdcParkedOrderActionField f;
		memset(&f, 0, sizeof(f));
		///���͹�˾����
		strcpy_s(f.BrokerID, BROKER_ID);
		///Ͷ���ߴ���
		strcpy_s(f.InvestorID, order->InvestorID);
		///��������
		strcpy_s(f.OrderRef, order->OrderRef);
		///ǰ�ñ��
		f.FrontID = order->FrontID;
		///�Ự���
		f.SessionID = order->SessionID;
		///������־
		f.ActionFlag = THOST_FTDC_AF_Delete;
		strcpy_s(f.InstrumentID, order->InstrumentID);
		strcpy_s(f.ExchangeID, order->ExchangeID);
		int iResult = api->ReqParkedOrderAction(&f, ++nReq);
		cerr << "--->>> ParkedOrderAction: " << ((iResult == 0) ? "Success" : "Failure") << endl;
		}
		*/
#endif
		ofstream out("cancelorder.txt", ios_base::app);
		if (!out)
		{
			cerr << "open errfile failure" << endl;
		}
		else
		{
			out << "strname:" << order->strID.c_str() << ",";
			out << "inst:" << order->InstrumentID << ",";
			out << "insertdate:" << LatestMarket[order->InstrumentID].TradingDay << ",";
			out << "orderinserttime: " << order->InsertTime << ",";
			out << "ordeactiontime:" << LatestMarket[order->InstrumentID].UpdateTime << ",";
			out << "price:" << order->LimitPrice << ",";
			out << "vol:" << order->VolumeTotalOriginal << ",";
			out << "direction:" << order->Direction << ",";
			out << "status:" << order->OrderStatus << ",";
			out << "OC:" << order->CombOffsetFlag[0] << endl;
			out.close();
		}
	}
}

void common::SendOrder2(inputorder& porder) 
{
	poskey pkey;
	pkey.strID = porder.strID;
	strcpy_s(pkey.InstrumentID, porder.InstrumentID);
	strpos_map::iterator iter = strpos.find(pkey);
	if (porder.changeflag && iter != strpos.end())
	{
		position_t &pos = iter->second;
		if (porder.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
		{
			if (porder.Direction == THOST_FTDC_D_Buy && pos.YdPosShort>0) //��������.
			{	
				if (pos.YdPosShort >= porder.Vol)
				{
					porder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
					SendOrder(porder);//��ƽ��ղ�..
				}
				else 
				{
					inputorder porder2 = porder;
					porder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
					porder.Vol = pos.YdPosShort;
					SendOrder(porder);//����ƽ��ղ�..
					porder2.Vol -= pos.YdPosShort;
					porder2.changeflag = false;
					SendOrder(porder2);//����ʣ������..
				}
				return;
			}
			else if (porder.Direction == THOST_FTDC_D_Sell && pos.YdPosLong > 0) //����.
			{
				if (pos.YdPosLong >= porder.Vol)
				{
					porder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
					SendOrder(porder);//��ƽ����..
				}
				else 
				{
					inputorder porder2 = porder;
					porder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
					porder.Vol = pos.YdPosLong;
					SendOrder(porder);//����ƽ����.
					porder2.Vol -= pos.YdPosShort;
					porder2.changeflag = false;
					SendOrder(porder2);//������ʣ������.
				}
				return;
			}
		}
		else // ƽ�֣�.
		{
			if (porder.Direction == THOST_FTDC_D_Buy) //��ƽ.
			{
				if (pos.YdPosShort > 0) 
				{
					if (pos.YdPosShort >= porder.Vol) 
					{
						porder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
						porder.changeflag = false;
						SendOrder(porder);//��ƽ��ղ�..
					}
					else
					{
						inputorder porder2 = porder;
						porder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
						porder.Vol = pos.YdPosShort;
						porder.changeflag = false;
						SendOrder(porder);//��ƽ��ղ�..
						porder2.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
						porder2.Vol -= pos.YdPosShort;
						SendOrder(porder2);//��ʣ������..
					}
				}
				else
				{
					porder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
					SendOrder(porder);//��..
				}
				return;
			}
			else if (porder.Direction == THOST_FTDC_D_Sell) //��ƽ.
			{
				if (pos.YdPosLong > 0)
				{
					if (pos.YdPosLong >= porder.Vol) 
					{
						porder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
						porder.changeflag = false;
						SendOrder(porder);//��ƽ����..
					}
					else
					{
						inputorder porder2 = porder;
						porder.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
						porder.Vol = pos.YdPosLong;
						porder.changeflag = false;
						SendOrder(porder);//��ƽ����..
						porder2.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
						porder2.Vol -= pos.YdPosLong;
						SendOrder(porder2);//����ʣ������..
					}
				}
				else
				{
					porder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
					SendOrder(porder);//����..
				}
				return;
			}
		}
	}
	porder.changeflag = false;
	SendOrder(porder);
}

void common::TrdFunTester(){
	static CThostFtdcDepthMarketDataField tick;
	class MyStratey : public strategy {
	public:
		MyStratey() :strategy("./inifiles/mhilbert_ru.ini"){}
		virtual void OnTick(CThostFtdcDepthMarketDataField& pData){ tick = pData; /*cout << "OnTick " << tick.InstrumentID << endl;*/ }
		/*
		///ȫ���ɽ�.
		#define THOST_FTDC_OST_AllTraded '0'
		///���ֳɽ����ڶ�����.
		#define THOST_FTDC_OST_PartTradedQueueing '1'
		///���ֳɽ����ڶ�����.
		#define THOST_FTDC_OST_PartTradedNotQueueing '2'
		///δ�ɽ����ڶ�����.
		#define THOST_FTDC_OST_NoTradeQueueing '3'
		///δ�ɽ����ڶ�����.
		#define THOST_FTDC_OST_NoTradeNotQueueing '4'
		///����.
		#define THOST_FTDC_OST_Canceled '5'
		///δ֪.
		#define THOST_FTDC_OST_Unknown 'a'
		///��δ����.
		#define THOST_FTDC_OST_NotTouched 'b'
		///�Ѵ���.
		#define THOST_FTDC_OST_Touched 'c'

		*/
		virtual void OnOrder(CThostFtdcOrderField& pOrder) { cout << "strategy::OnOrder:"<<pOrder.OrderStatus<<" "<<pOrder.StatusMsg<<endl; }
		virtual void OnTrade(CThostFtdcTradeField& pTrade) { cout << "strategy::OnTrade:"<<pTrade.OffsetFlag<<endl; }
		virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){ cout << "OnRspOrderInsert\n"; }
		virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){ cout << "OnRspOrderAction\n"; }
	};
	MyStratey *my_stra = new MyStratey;
	//AddStrategy("ru1709", my_stra);
	common& comm = common::GetInstance();
	int chose;
	double price;
	int volume;
	while (1)
	{
		printf("0.����1.����2.ƽ��/��ƽ3.����4.ƽ��/��ƽ:\n");
		scanf("%d", &chose);
		/*printf("price:");
		scanf("%lf", &price);
		printf("volume:");
		scanf("%d", &volume);*/

		inputorder torder;
		memset(&torder, 0, sizeof(torder));
		torder.strID = my_stra->GetName();
		strcpy_s(torder.InstrumentID, my_stra->GetInst()[0].c_str());
		//torder.LimitPrice = price;.
		torder.Vol = 1;
		torder.Purchasing = 1;
		torder.UnitTick = 15.;
		torder.WaitSecond = 1;
		torder.RequestID = 100203;
		switch (chose)
		{
		case 0:
		{
				  c_main.notify_one();
				  return;
		}
			break;
		case 1:
		{
				  torder.LimitPrice = tick.LastPrice + torder.UnitTick;
				  torder.Direction = THOST_FTDC_D_Buy;
				  torder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
				 
		}
			break;
		case 2:
		{
				  torder.LimitPrice = tick.LastPrice - torder.UnitTick;
				  torder.Direction = THOST_FTDC_D_Sell;
				  torder.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
		}
			break;
		case 3:
		{
				  torder.LimitPrice = tick.LastPrice - torder.UnitTick;
				  torder.Direction = THOST_FTDC_D_Sell;
				  torder.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
		}
			break;
		case 4:
		{
				  torder.LimitPrice = tick.LastPrice + torder.UnitTick;
				  torder.Direction = THOST_FTDC_D_Buy;
				  torder.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
		}
			break;
		default:
			continue;
		};
		comm.SendOrder2(torder);
	}
}
void common::SendOrder(inputorder& porder)
{
	common& comm = common::GetInstance();
	CThostFtdcInputOrderField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����.
	strcpy_s(req.BrokerID, comm.BROKER_ID);
	///Ͷ���ߴ���.
	strcpy_s(req.InvestorID, comm.INVESTOR_ID);
	///��Լ����.
	strcpy_s(req.InstrumentID, porder.InstrumentID);
	///��������.
	int iNextOrderRef = atoi(comm.ORDER_REF);
	iNextOrderRef++;
	sprintf_s(comm.ORDER_REF, "%d", iNextOrderRef);
	strcpy_s(req.OrderRef, comm.ORDER_REF);
	///��������: .
	req.Direction = porder.Direction;
	///��Ͽ�ƽ��־: ����.
	req.CombOffsetFlag[0] = porder.CombOffsetFlag[0];
	///���Ͷ���ױ���־.
	req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	///����: 1.
	req.VolumeTotalOriginal = porder.Vol;
	if (porder.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
	{
		req.TimeCondition = THOST_FTDC_TC_GFD;
		///�����۸�����: �޼�.
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		///�۸�.
		req.LimitPrice = porder.LimitPrice;
	}
	else
	{
		//if (porder->pur_num <= porder->max_purching).
		{
			req.TimeCondition = THOST_FTDC_TC_GFD;
			///�����۸�����: �޼�.
			req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///�۸�.
			req.LimitPrice = porder.LimitPrice;
		}
		//else if (porder->pur_num >= porder->max_purching).
		//{.
		//req.TimeCondition = THOST_FTDC_TC_IOC;.
		///�����۸�����: �м�.
		//req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;.
		///�۸�.
		//req.LimitPrice = 0;.
		//}.
	}
	///�ɽ�������: �κ�����.
	req.VolumeCondition = THOST_FTDC_VC_AV;
	///��С�ɽ���: 1.
	req.MinVolume = 1;
	///��������: ����.
	req.ContingentCondition = THOST_FTDC_CC_Immediately;
	///ǿƽԭ��: ��ǿƽ.
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
	///�Զ������־: ��.
	req.IsAutoSuspend = 0;
	///�û�ǿ����־: ��.
	req.UserForceClose = 0;
	req.RequestID = porder.RequestID;
#if !BACKTEST
	//���ͱ���.
	string str = porder.InstrumentID;
	size_t loc = str.find('1');
	string skey = str.substr(0, loc);
	if ((inststatus[skey] == THOST_FTDC_IS_Continous || inststatus[skey] == THOST_FTDC_IS_AuctionOrdering) && openpermit[porder.InstrumentID])
	{
		int iResult = comm.api->ReqOrderInsert(&req, ++comm.nReq);
		cerr << "--->>> ����¼������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
	}
	else if ((inststatus[skey] == THOST_FTDC_IS_BeforeTrading || inststatus[skey] == THOST_FTDC_IS_NoTrading) && openpermit[porder.InstrumentID])
	{
		CThostFtdcParkedOrderField f;
		memset(&f, 0, sizeof(f));
		///���͹�˾����.
		strcpy_s(f.BrokerID, comm.BROKER_ID);
		///Ͷ���ߴ���.
		strcpy_s(f.InvestorID, comm.INVESTOR_ID);
		///��Լ����.
		strcpy_s(f.InstrumentID, porder.InstrumentID);
		///��������.
		int iNextOrderRef = atoi(comm.ORDER_REF);
		iNextOrderRef++;
		sprintf_s(comm.ORDER_REF, "%d", iNextOrderRef);
		strcpy_s(f.OrderRef, comm.ORDER_REF);
		///��������: .
		f.Direction = porder.Direction;
		///��Ͽ�ƽ��־: ����.
		f.CombOffsetFlag[0] = porder.CombOffsetFlag[0];
		///���Ͷ���ױ���־.
		f.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
		///����: 1.
		f.VolumeTotalOriginal = porder.Vol;
		if (porder.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
		{
			f.TimeCondition = THOST_FTDC_TC_GFD;
			///�����۸�����: �޼�.
			f.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///�۸�.
			f.LimitPrice = porder.LimitPrice;
		}
		else
		{
			f.TimeCondition = THOST_FTDC_TC_GFD;
			///�����۸�����: �޼�.
			f.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
			///�۸�.
			f.LimitPrice = porder.LimitPrice;
		}
		///�ɽ�������: �κ�����.
		f.VolumeCondition = THOST_FTDC_VC_AV;
		///��С�ɽ���: 1.
		f.MinVolume = 1;
		///��������: ����.
		f.ContingentCondition = THOST_FTDC_CC_Immediately;
		///ǿƽԭ��: ��ǿƽ.
		f.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
		///�Զ������־: ��.
		f.IsAutoSuspend = 0;
		///�û�ǿ����־: ��.
		f.UserForceClose = 0;
		int iResult = comm.api->ReqParkedOrderInsert(&f, ++comm.nReq);
		cerr << "--->>> Ԥ��¼������: " << ((iResult == 0) ? "�ɹ�" : "ʧ��") << endl;
	}
	else if (inststatus[skey] == THOST_FTDC_IS_Closed)
	{
		openpermit[porder.InstrumentID] = false;
	}
#endif

	//�ȷ������ٱ��棬�ӿ��ٶȣ������ʱ���Ա�֤�����ܶ���.
	orderkey key;
	key.FrontID = comm.FRONT_ID;
	key.SessionID = comm.SESSION_ID;
	strcpy_s(key.OrderRef, req.OrderRef);
	order_t *order = new order_t();
	memset(order, 0, sizeof(order_t));
	order->strID = porder.strID;
	order->CombOffsetFlag[0] = req.CombOffsetFlag[0];
	order->Direction = req.Direction;
	strcpy_s(order->InstrumentID, req.InstrumentID);
	order->FrontID = key.FrontID;
	strcpy_s(order->InvestorID, req.InvestorID);
	order->LimitPrice = req.LimitPrice;
	strcpy_s(order->OrderRef, req.OrderRef);
	order->SessionID = key.SessionID;
	order->VolumeTotalOriginal = req.VolumeTotalOriginal;
	order->Purchasing = porder.Purchasing;
	order->WaitSecond = porder.WaitSecond;
	order->Change = porder.changeflag;
	SYSTEMTIME ct;
	GetLocalTime(&ct);
	int h, m, s, ms;
	h = ct.wHour;
	m = ct.wMinute;
	s = ct.wSecond;
	ms = ct.wMilliseconds;
	dayofweek = ct.wDayOfWeek;
	double ss = 3600 * h + 60 * m + s + ms / 1000;
	order->LocalSec = ss;
	strcpy_s(order->InsertTime, comm.LatestMarket[order->InstrumentID].UpdateTime);
	order->OrderStatus = '#';
	order->UnitTick = porder.UnitTick;
	std::unique_lock<std::mutex> lock_ctp(m_ctp);
	auto check1 = orderinfo.insert(make_pair(key, order));
	lock_ctp.unlock();
	std::unique_lock<std::mutex> lock_check(m_checkorder);
	auto check2 = orderbook.insert(make_pair(key, order));
	lock_check.unlock();
	if (!check1.second || !check2.second)
	{
		cerr << "order insert failure!" << endl;
	}
	else
	{
		c_checkorder.notify_all();
	}


	ofstream out("sendorder.txt", ios_base::app);
	if (!out)
	{
		cerr << "open errfile failure" << endl;
	}
	else
	{
		out << "strname:" << order->strID.c_str() << ",";
		out << "inst:" << order->InstrumentID << ",";
		out << "insertdate:" << ct.wYear * 10000 + ct.wMonth * 100 + ct.wDay << ",";
		out << "orderinserttime: " << order->InsertTime << ",";
		out << "orderlocalss:" << order->LocalSec << ",";
		out << "orderref" << order->OrderRef << ",";
		out << "price:" << order->LimitPrice << ",";
		out << "vol:" << order->VolumeTotalOriginal << ",";
		out << "direction:" << order->Direction << ",";
		out << "status:" << order->OrderStatus << ",";
		out << "OC:" << order->CombOffsetFlag[0] << endl;
		out.close();
	}
#if BACKTEST
	pback->OnRtnOrder(order);
	pback->OnRtnTrade(order);
#endif
}

void common::StartSqlite() {
	if (NULL == sqlite3_){
		int res = sqlite3_open(g_db_name, &sqlite3_);
		if (res){
			cerr << "sqlite3_open " << g_db_name << " fail\n";
			sqlite3_close(sqlite3_);
			sqlite3_ = NULL;
		}		
	}

	if (!_SQLite_EXEC(g_crt_tbl_TradekeyStrid)
		|| !_SQLite_EXEC(g_crt_tbl_TradekeyStrid_updating)
		/*|| !_SQLite_EXEC(g_crt_tbl_Inst2Pos)*/) {
		cerr << "create tables fail\n";
	}

	if (!SQLite_Query_TradekeyStrid()/* || !SQLite_Query_Pos()*/){
		cerr << "SQLite_Query_TradekeyStrid fail\n";
	}
}

void common::SQLite_TradekeyStrid_Callback(int num, char **values, char **colName){
	tradeidkey key;
	strcpy_s(key.TradingDay, values[0]);
	string TradeID = trim(values[1]);
	strcpy_s(key.TradeID, TradeID.c_str());
	StrategyID id = values[2];
	map_tradeid_straid_[key] = id;
}

static int TradekeyStrid_Callback(void *param, int num, char **values, char **colName){
	((common*)param)->SQLite_TradekeyStrid_Callback(num, values, colName);
	return 0;
}

bool common::SQLite_Query_TradekeyStrid(){
	/*SYSTEMTIME ct;
	GetLocalTime(&ct);
	int y, m, d;
	y = ct.wYear;
	m = ct.wMonth;
	d = ct.wDay;
	d -= 8;
	if (d < 1){
		d += 30; 
		m -= 1;
		if (m < 1) {
			m += 12;
			y -= 1;
		}
	}

	char sql[256] = { 0 };
	sprintf_s(sql, "select * from Tradeid2Strid where TradingDay >= %04d%02d%02d;", y, m, d);*/
	char *sql = "select * from Tradeid2Strid_updating";
	if (!_SQLite_EXEC(sql, TradekeyStrid_Callback, this))
		return false;

	return true;
}


void common::SQLite_StraPos_Callback(int num, char **values, char **colName)
{
	poskey key;
	key.strID = values[0];
	strcpy_s(key.InstrumentID, values[1]);

	position_t pos;
	pos.strID = values[0];
	strcpy_s(pos.InstrumentID, values[1]);
	strcpy_s(pos.InvestorID, values[2]);
	pos.PosCostLong = atof(values[3]);
	pos.YdPosLong = atoi(values[4]);
	pos.PosLong = atoi(values[5]);
	pos.PosCostShort = atof(values[6]);
	pos.YdPosShort = atoi(values[7]);
	pos.PosShort = atoi(values[8]);
	pos.OLongFrozen = atoi(values[9]);
	pos.OShortFrozen = atoi(values[10]);
	pos.CLongFrozen = atoi(values[11]);
	pos.CShortFrozen = atoi(values[12]);
	pos.MarginRate = atof(values[13]);
	pos.UnitTick = atof(values[14]);
	pos.Multiple = atoi(values[15]);

	strpos[key] = pos;
}

void common::SQLite_InstPos_Callback(int num, char **values, char **colName)
{
	position_t pos;
	pos.strID = values[0];
	strcpy_s(pos.InstrumentID, values[1]);
	strcpy_s(pos.InvestorID, values[2]);
	pos.PosCostLong = atof(values[3]);
	pos.YdPosLong = atoi(values[4]);
	pos.PosLong = atoi(values[5]);
	pos.PosCostShort = atof(values[6]);
	pos.YdPosShort = atoi(values[7]);
	pos.PosShort = atoi(values[8]);
	pos.OLongFrozen = atoi(values[9]);
	pos.OShortFrozen = atoi(values[10]);
	pos.CLongFrozen = atoi(values[11]);
	pos.CShortFrozen = atoi(values[12]);
	pos.MarginRate = atof(values[13]);
	pos.UnitTick = atof(values[14]);
	pos.Multiple = atoi(values[15]);

	instpos[pos.InstrumentID] = pos;
}

static int StraPos_Callback(void *param, int num, char **values, char **colName){
	((common*)param)->SQLite_StraPos_Callback(num, values, colName);
	return 0;
}

static int InstPos_Callback(void *param, int num, char **values, char **colName){
	((common*)param)->SQLite_InstPos_Callback(num, values, colName);
	return 0;
}

bool common::SQLite_Query_Pos()
{
	char *sql = "select * from Stra2Pos where YdPosLong+PosLong+YdPosShort+PosShort>0;";
	if (!_SQLite_EXEC(sql, StraPos_Callback, this))
		return false;

	char *sql2 = "select * from Inst2Pos where YdPosLong+PosLong+YdPosShort+PosShort>0;";
	if (!_SQLite_EXEC(sql, InstPos_Callback, this))
		return false;

	return true;
}

bool common::SQLite_Insert_StraPos(const poskey& key, position_t& pos){
	char sql[1024] = { 0 };
	sprintf_s(sql, "replace into Stra2Pos values('%s','%s', '%s', %g, %d, %d, %g, %d, %d, %d, %d, %d, %d, %g, %g, %d)",
		key.strID.c_str(), key.InstrumentID,
		pos.InvestorID, pos.PosCostLong, pos.YdPosLong,
		pos.PosLong, pos.PosCostShort, pos.YdPosShort, pos.PosShort,
		pos.OLongFrozen, pos.OShortFrozen, pos.CLongFrozen, pos.CShortFrozen,
		pos.MarginRate, pos.UnitTick, pos.Multiple);
	if (!_SQLite_EXEC(sql))
		return false;
	return true;
}

bool common::SQLite_Insert_InstPos(const string& inst, position_t& pos)
{
	char sql[1024] = { 0 };
	sprintf_s(sql, "replace into Inst2Pos values('%s','%s', '%s', %g, %d, %d, %g, %d, %d, %d, %d, %d, %d, %g, %g, %d)",
		pos.strID.c_str(), pos.InstrumentID,
		pos.InvestorID, pos.PosCostLong, pos.YdPosLong,
		pos.PosLong, pos.PosCostShort, pos.YdPosShort, pos.PosShort,
		pos.OLongFrozen, pos.OShortFrozen, pos.CLongFrozen, pos.CShortFrozen,
		pos.MarginRate, pos.UnitTick, pos.Multiple);
	if (!_SQLite_EXEC(sql))
		return false;
	return true;
}

bool common::SQLite_Insert_TradekeyStrid(TThostFtdcDateType &TradingDay, TThostFtdcTradeIDType &TradeID, StrategyID &id){
	string trade_id = trim(TradeID);
	tradeidkey key;
	strcpy_s(key.TradingDay, TradingDay);
	strcpy_s(key.TradeID, trade_id.c_str());
	map_tradeid_straid_[key] = id;
	char sql[1024] = { 0 };
	sprintf_s(sql, "replace into Tradeid2Strid values('%s','%s','%s')",
		TradingDay, trade_id.c_str(), id.c_str());
	if (!_SQLite_EXEC(sql))
		return false;
	memset(sql, 0, sizeof(sql));
	sprintf_s(sql, "replace into Tradeid2Strid_updating values('%s','%s','%s')",
		TradingDay, trade_id.c_str(), id.c_str());
	if (!_SQLite_EXEC(sql))
		return false;
	return true;
}

bool common::_SQLite_EXEC(const char *SQL, fnOnQuerySQLite cb, void *p)
{
	int res = 0;
	char *errMsg = nullptr;
	if (sqlite3_)
	{
		while (1)
		{
			res = sqlite3_exec(sqlite3_, SQL, cb, p, &errMsg);
			if (SQLITE_OK != res)
			{
				if (strstr(errMsg, "database is locked"))
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));
					continue;
				}
				else
				{
					cout << SQL << errMsg;
					sqlite3_free(errMsg);
					return false;
				}
				break;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		cout << "SQLite is closed";
		return false;
	}


	sqlite3_free(errMsg);
	return true;
}

void common::StopSqlite(){
	/*if (strpos2.empty()){
		char *sql = "delete from Stra2Pos;";
		_SQLite_EXEC(sql);
	}
	else {
		for (strpos_map::iterator iter = strpos2.begin(); iter != strpos2.end(); ++iter){
			SQLite_Insert_StraPos(iter->first, iter->second);
		}
	}
	
	if (instpos2.empty()){
		char *sql = "delete from Inst2Pos;";
		_SQLite_EXEC(sql);
	}
	else {
		for (instpos_map::iterator iter = instpos2.begin(); iter != instpos2.end(); ++iter){
			SQLite_Insert_InstPos(iter->first, iter->second);
		}
	}*/

	if (sqlite3_)
		sqlite3_close(sqlite3_);
}

bool common::AddStrategy(string ppinst, strategy *pstr)
{
	bool res = false;
	if (pStrmap.count(ppinst) <= 0)
	{
		pStr pstrategy;
		auto check = pStrmap.insert(make_pair(ppinst, pstrategy));
		if (!check.second)
		{
			cerr << "insert to pstrmap failure!" << endl;
			return res;
		}
	}
	pStr& rvect = pStrmap[ppinst];
	rvect.push_back(pstr);
	StrategyID strID = pstr->GetName();
	if (pStrID.count(strID) <= 0)
	{
		auto check = pStrID.insert(make_pair(strID, pstr));
		if (!check.second)
		{
			cerr << "insert to pstrid failure!" << endl;
			return res;
		}
	}
	return true;
}


void common::CreateThread()
{
	ctpflag = true;
	strflag = true;
	warnflag = true;
	checkflag = true;
	T_ctp = std::thread(std::mem_fn(&common::CtpFun), this);
	T_str = std::thread(std::mem_fn(&common::Strategy), this);
	T_warn = std::thread(std::mem_fn(&common::Warning), this);
	T_checkorder = std::thread(std::mem_fn(&common::CheckOrder2), this);
	T_tester = std::thread(std::mem_fn(&common::TrdFunTester), this);
}

void common::ReleaseThread()
{
	strflag = false;
	c_strategy.notify_all();
	T_str.join();
	warnflag = false;
	c_warn.notify_all();
	T_warn.join();
	ctpflag = false;
	c_ctp.notify_all();
	T_ctp.join();
	checkflag = false;
	c_checkorder.notify_all();
	T_checkorder.join();

	/*������ctp��ֲ���ϸ,��ѯʧ���򱨴���ѯ�ɹ��Ļ������鵽�ĳֲ�д�����ݿ⣬���ȶԲ�õĽ�����ڴ��¼�������һ����Ҳ���´�*/
	/*if (!ReqPosDetail()){
		ofstream out("err.txt", ios_base::app);
		if (!out)
		{
			cerr << "open errfile failure" << endl;
		}
		else
		{
			out << "ReqPosDetail fail" << endl;
			out.close();
		}
	}
	else {
		std::unique_lock<std::mutex> lock(m_reqPosdetail);
		c_reqPosdetail.wait(lock);
	}*/

	api->Release();
	api = NULL;
	md->Release();
	md = NULL;
	delete t;
	t = NULL;
	delete q;
	q = NULL;
	for (map<string, strategy*>::iterator it = pStrID.begin(); it != pStrID.end(); it++)
	{
		delete it->second;
		it->second = NULL;
	}

	for (map<string, pStr>::iterator iter = pStrmap.begin(); iter != pStrmap.end(); iter++)
	{
		pStr pvect = iter->second;
		for (vector<strategy*>::iterator it = pvect.begin(); it != pvect.end(); it++)
		{
			(*it) = NULL;
		}
	}
}

void common::CheckOrder2()
{
	while (checkflag)
	{
		std::unique_lock<std::mutex> lock(m_checkorder);
		while (orderbook.empty() && checkflag)
		{
			c_checkorder.wait(lock);
		}
		if (!orderbook.empty())
		{
			for (orderinfo_map::iterator iter = orderbook.begin(); iter != orderbook.end();)
			{
				if ((iter->second->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
					(iter->second->OrderStatus != THOST_FTDC_OST_Canceled) &&
					(iter->second->OrderStatus != THOST_FTDC_OST_AllTraded) &&
					(iter->second->OrderStatus != THOST_FTDC_OST_NoTradeNotQueueing))
				{
					SYSTEMTIME ct;
					GetLocalTime(&ct);
					int h, m, s, ms;
					h = ct.wHour;
					m = ct.wMinute;
					s = ct.wSecond;
					ms = ct.wMilliseconds;
					dayofweek = ct.wDayOfWeek;
					double ss = 3600 * h + 60 * m + s + ms / 1000;
					bool timeflag = ss - iter->second->LocalSec > iter->second->WaitSecond;
					bool priceflag = (iter->second->Direction == THOST_FTDC_D_Buy && iter->second->LimitPrice < LatestMarket[iter->second->InstrumentID].BidPrice1) ||
						(iter->second->Direction == THOST_FTDC_D_Sell && iter->second->LimitPrice > LatestMarket[iter->second->InstrumentID].AskPrice1);
					if (timeflag && priceflag)
					{
						iter->second->LocalSec = ss;
						CancelOrder(iter->second);
					}
					iter++;
				}
				else
				{
					orderbook.erase(iter++);
				}
			}
		}
	}
}

void common::CtpFun()
{
	while (ctpflag)
	{
		std::unique_lock<std::mutex> lock(m_ctp);
		while (trseq.empty() && ctpflag)
		{
			c_ctp.wait(lock);
		}
		if (!trseq.empty())
		{
			TR tr = trseq.front();
			trseq.pop_front();
			lock.unlock();
			if (tr.flag == ord)
			{
				CThostFtdcOrderField& order = tr.tuo.order;
				orderkey key;
				key.FrontID = order.FrontID;
				key.SessionID = order.SessionID;
				strcpy_s(key.OrderRef, order.OrderRef);
				if (IsMyOrder(key))
				{
					order_t *porder = orderinfo[key];
					CalFrozen(order, porder);
					porder->OrderStatus = order.OrderStatus;
					if (porder->Purchasing == 1 && order.OrderStatus == THOST_FTDC_OST_Canceled)//�����ɹ���������Ҫ׷���ģ������µ�.
					{
						double price = LatestMarket[order.InstrumentID].LastPrice;
						if (order.Direction == THOST_FTDC_D_Buy)
						{
							price = LatestMarket[order.InstrumentID].BidPrice1+porder->UnitTick;
						}
						else if (order.Direction == THOST_FTDC_D_Sell)
						{
							price = LatestMarket[order.InstrumentID].AskPrice1-porder->UnitTick;
						}
						inputorder torder;
						memset(&torder, 0, sizeof(torder));
						torder.strID = porder->strID;
						strcpy_s(torder.InstrumentID, porder->InstrumentID);
						torder.LimitPrice = price;
						torder.Direction = order.Direction;
						torder.CombOffsetFlag[0] = order.CombOffsetFlag[0];
						torder.Vol = order.VolumeTotalOriginal - order.VolumeTraded;
						torder.Purchasing = porder->Purchasing;
						torder.PurNum = porder->PurNum + 1;
						torder.changeflag = porder->Change;
						SendOrder(torder);
						porder->Purchasing = 0;//�����ظ�׷��.
					}
					strcpy_s(porder->ExchangeID, order.ExchangeID);
					strcpy_s(porder->OrderLocalID, order.OrderLocalID);
					strcpy_s(porder->TraderID, order.TraderID);
					strcpy_s(porder->InsertTime, order.InsertTime);
					//order.VolumeTotalOriginal = order.VolumeTotalOriginal;.
					porder->VolumeTotal = order.VolumeTotal;
					porder->VolumeTraded = order.VolumeTraded;
					if (strlen(order.ExchangeID) > 0 && strlen(order.OrderLocalID) > 0 && strlen(order.TraderID) > 0)
					{
						tradekey tkey;
						strcpy_s(tkey.ExchangeID, order.ExchangeID);
						strcpy_s(tkey.TraderID, order.TraderID);
						strcpy_s(tkey.OrderLocalID, order.OrderLocalID);
						if (tmapo.count(tkey) <= 0)
						{
							auto check = tmapo.insert(make_pair(tkey, key));
							if (!check.second)
							{
								cerr << "insert to tradekey&orderkey failure!" << endl;
							}
						}
						if (tradeinfo.count(tkey) > 0)
						{
							trade_t& trade = tradeinfo[tkey];
							trade.strID = porder->strID;
						}
						else
						{
							trade_t trade;
							memset(&trade, 0, sizeof(trade));
							trade.strID = porder->strID;
							tradeinfo.insert(make_pair(tkey, trade));
						}
					}
					//�ص�������غ���.
					if (porder->Change){
						//���ɽ�������ת��..
						if (tr.tuo.order.CombOffsetFlag[0] == THOST_FTDC_OF_Open)
							tr.tuo.order.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
						else
							tr.tuo.order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
					}
					pStrID[porder->strID]->OnOrder(tr.tuo.order);
#if LOG
					ofstream out("order.txt", ios_base::app);
					if (!out)
					{
						cerr << "open errfile failure" << endl;
					}
					else
					{
						out << "strname:" << porder->strID.c_str() << ",";
						out << "inst:" << porder->InstrumentID << ",";
						out << "insertdate:" << order.InsertDate << ",";
						out << "orderinserttime: " << porder->InsertTime << ",";
						out << "orderlocalss:" << porder->LocalSec << ",";
						out << "price:" << porder->LimitPrice << ",";
						out << "vol:" << porder->VolumeTotalOriginal << ",";
						out << "tradevol:" << porder->VolumeTraded << ",";
						out << "remainvol:" << porder->VolumeTotal << ",";
						out << "direction:" << porder->Direction << ",";
						out << "status:" << porder->OrderStatus << ",";
						out << "OC:" << porder->CombOffsetFlag[0] << endl;
						out.close();
					}
#endif
				}
			}
			else if (tr.flag == trd)
			{
				CThostFtdcTradeField *pTrade = &tr.tuo.trade;
				//cout << "�ɽ�ʱ�䣺" << pTrade->TradeTime << endl;.
				tradekey key;
				strcpy_s(key.ExchangeID, pTrade->ExchangeID);
				strcpy_s(key.TraderID, pTrade->TraderID);
				strcpy_s(key.OrderLocalID, pTrade->OrderLocalID);
				if (IsMyTrade(key))
				{
					CalPos(pTrade, postype::InstPos);
					CalPos(pTrade, postype::StrPos);
					string strname;
					if (tradeinfo.count(key) > 0)
					{
						trade_t& trade = tradeinfo[key];
						trade.Price = (trade.Price*trade.Volume + pTrade->Price*pTrade->Volume) / (trade.Volume + pTrade->Volume);
						trade.Volume += pTrade->Volume;
						strcpy_s(trade.TradeTime, pTrade->TradeTime);
						strcpy_s(trade.TradingDay, pTrade->TradingDay);
						strcpy_s(trade.InstrumentID, pTrade->InstrumentID);
						strcpy_s(trade.InvestorID, pTrade->InvestorID);
						trade.Direction = pTrade->Direction;
						trade.HedgeFlag = pTrade->HedgeFlag;
						trade.OffsetFlag = pTrade->OffsetFlag;
						strcpy_s(trade.OrderLocalID, pTrade->OrderLocalID);
						strcpy_s(trade.ExchangeID, pTrade->ExchangeID);
						strcpy_s(trade.TraderID, pTrade->TraderID);
						if (trade.strID.empty())
						{
							orderkey& okey = tmapo[key];
							order_t *order = orderinfo[okey];
							trade.strID = order->strID;
						}
						SQLite_Insert_TradekeyStrid(pTrade->TradingDay, pTrade->TradeID, trade.strID);
						strname = trade.strID;
					}
					else
					{
						trade_t trade;
						memset(&trade, 0, sizeof(trade));
						trade.Price = (trade.Price*trade.Volume + pTrade->Price*pTrade->Volume) / (trade.Volume + pTrade->Volume);
						trade.Volume += pTrade->Volume;
						strcpy_s(trade.TradeTime, pTrade->TradeTime);
						strcpy_s(trade.TradingDay, pTrade->TradingDay);
						strcpy_s(trade.InstrumentID, pTrade->InstrumentID);
						strcpy_s(trade.InvestorID, pTrade->InvestorID);
						trade.Direction = pTrade->Direction;
						trade.HedgeFlag = pTrade->HedgeFlag;
						trade.OffsetFlag = pTrade->OffsetFlag;
						strcpy_s(trade.OrderLocalID, pTrade->OrderLocalID);
						strcpy_s(trade.ExchangeID, pTrade->ExchangeID);
						strcpy_s(trade.TraderID, pTrade->TraderID);
						strname = trade.strID;
						tradeinfo.insert(make_pair(key, trade));
					}
					//�ص�������غ���.
					if (orderinfo[tmapo[key]]->Change){
						//���ɽ�������ת��..
						if (tr.tuo.trade.OffsetFlag == THOST_FTDC_OF_Open)
							tr.tuo.trade.OffsetFlag = THOST_FTDC_OF_Close;
						else
							tr.tuo.trade.OffsetFlag = THOST_FTDC_OF_Open;
					}
					pStrID[strname]->OnTrade(tr.tuo.trade);
#if LOG
					ofstream out("trade.txt", ios_base::app);
					if (!out)
					{
						cerr << "open errfile failure" << endl;
					}
					else
					{
						out << "strname:" << strname.c_str() << ",";
						out << "inst:" << pTrade->InstrumentID << ",";
						out << "tradedate:" << pTrade->TradeDate << ",";
						out << "tradetime: " << pTrade->TradeTime << ",";
						out << "price:" << pTrade->Price << ",";
						out << "tradevol:" << pTrade->Volume << ",";
						out << "direction:" << pTrade->Direction << ",";
						out << "OC:" << pTrade->OffsetFlag << endl;
						out.close();
					}
#endif
				}
			}
			//trseq.pop_front();.
		}
	}
}

void common::Strategy()
{
	while (strflag)
	{
		if (!mktdata.empty())
		{
			EnterCriticalSection(&cs_strategy);
			CThostFtdcDepthMarketDataField mkt = mktdata.front();
			mktdata.erase(mktdata.begin());
			LeaveCriticalSection(&cs_strategy);
			if (strcmp(mkt.UpdateTime, "21:00:00") >= 0 || strcmp(mkt.UpdateTime, "02:30:00") <= 0 ||
				(strcmp(mkt.UpdateTime, "09:00:00") >= 0 && strcmp(mkt.UpdateTime, "15:16:00") <= 0))
			{
				string pkey = mkt.InstrumentID;
#if BACKTEST
				pkey = pkey.substr(0, 2);
#endif
				map<string, pStr>::iterator iter = pStrmap.find(pkey);
				if (iter != pStrmap.end())
				{
					pStr pvect = iter->second;
					for (vector<strategy*>::iterator it = pvect.begin(); it != pvect.end(); it++)
					{
						//�ص�������غ���.
						string str = mkt.InstrumentID;
						size_t loc = str.find('1');
						string skey = str.substr(0, loc);
						if ((*it) != NULL && (inststatus[skey] == THOST_FTDC_IS_Continous || inststatus[skey] == THOST_FTDC_IS_AuctionOrdering))
						{
							(*it)->OnTick(mkt);
						}
					}
				}
#if !BACKTEST
				if (strcmp(mkt.UpdateTime, "15:14:59") >= 0 && strcmp(mkt.UpdateTime, "21:00:00") < 0)// && mkt.ClosePrice <= mkt.UpperLimitPrice && mktdata.size() <= 1).
				{
					SYSTEMTIME ct;
					GetLocalTime(&ct);
					int h, m, s;
					h = ct.wHour;
					m = ct.wMinute;
					s = ct.wSecond;
					double ss = 10000 * h + 100 * m + s;
					if (ss >= 143000 && ss < 205000)
					{
						common& comm = common::GetInstance();
						CThostFtdcQryTradingAccountField req;
						memset(&req, 0, sizeof(req));
						strcpy_s(req.BrokerID, comm.cfgfile["BROKER_ID"].c_str());
						strcpy_s(req.InvestorID, comm.cfgfile["INVESTOR_ID"].c_str());
						int Res = comm.api->ReqQryTradingAccount(&req, ++comm.nReq);
						std::cout << "�ʽ��ѯ��" << ((Res == 0) ? "�ɹ�" : "ʧ��") << std::endl;//�ʽ��ѯ.
						comm.c_main.notify_all();
					}
				}
#endif
			}
		}
	}
}

void common::Warning()
{
	while (warnflag)
	{
		/*std::unique_lock<std::mutex> lock(m_warn);
		c_warn.wait(lock);
		LPCWSTR lpcwStr = TEXT("by.wav");
		PlaySound(lpcwStr, NULL, SND_FILENAME | SND_SYNC);*/
		if (!releaseTrade) {
			SYSTEMTIME ct;
			GetLocalTime(&ct);
			int h, m, s;
			h = ct.wHour;
			m = ct.wMinute;
			s = ct.wSecond;
			double ss = 10000 * h + 100 * m + s;
			if (ss >= 23000 && ss <= 23500){
				api->Release();
				delete t;
				t = NULL;
				releaseTrade = true;
			}
		}
		if (!initTrade) {
			SYSTEMTIME ct;
			GetLocalTime(&ct);
			int h, m, s;
			h = ct.wHour;
			m = ct.wMinute;
			s = ct.wSecond;
			double ss = 10000 * h + 100 * m + s;
			if (ss >= 90000 && ss <= 90500){
				t = new Trade;
				t->Start();
				initTrade = true;
			}
		}

		Sleep(60000);
	}
}