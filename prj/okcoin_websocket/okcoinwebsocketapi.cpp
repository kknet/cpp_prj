
#include "string.h"
#include "okcoinwebsocketapi.h"
#include <sstream>
#include "parameter.h"

OKCoinWebSocketApi::OKCoinWebSocketApi():
pWebsocket(0),
m_callbak_open(0),
m_callbak_close(0),
m_callbak_message(0)
{
	hThread = 0;
};

OKCoinWebSocketApi::~OKCoinWebSocketApi()
{
	Close();
};

void OKCoinWebSocketApi::SetKey(string api_key,string secret_key)
{	
	m_api_key		=	api_key;
	m_secret_key	=	secret_key;	
}

void OKCoinWebSocketApi::SetUri(string uri)
{
	m_uri = uri;
}
void OKCoinWebSocketApi::Close()
{
	if(pWebsocket != 0)
	{
		pWebsocket->doclose();
		WaitForSingleObject(hThread,INFINITE);
		if (NULL != hThread)
		{
			CloseHandle(hThread);
		}
	}
}

void OKCoinWebSocketApi::Emit(const char *channel,string &parameter)
{
	if(pWebsocket != 0)
	{
		pWebsocket->emit(channel,parameter);
	}
};
void OKCoinWebSocketApi::Emit(const char *channel)
{
	if(pWebsocket != 0)
	{
		pWebsocket->emit(channel);
	}
};
void OKCoinWebSocketApi::Emit(string &channel)
{
	if(pWebsocket != 0)
	{
		pWebsocket->emit(channel);
	}
};

void OKCoinWebSocketApi::Remove(string channel)
{
	if(pWebsocket != 0)
	{
		pWebsocket->remove(channel);
	}
};

void OKCoinWebSocketApi::Run()
{
	unsigned int threadId = 0;
	hThread = (HANDLE)_beginthreadex(NULL,0,OKCoinWebSocketApi::RunThread,this,0,&threadId);
}

unsigned __stdcall OKCoinWebSocketApi::RunThread( LPVOID arg )
{
	if(arg != 0)
	{
		OKCoinWebSocketApi *api = (OKCoinWebSocketApi *)arg;
		
		for(int i = 0;i < MAX_RETRY_COUNT;i++)
		{
			if(api->pWebsocket != 0)
			{
				api->pWebsocket->doclose();
			}
			
			if(api->pWebsocket == 0)
			{
				api->pWebsocket = new WebSocket();
			}

			if(api->pWebsocket != 0)
			{
				api->pWebsocket->callbak_open = api->m_callbak_open;
				api->pWebsocket->callbak_close = api->m_callbak_close;
				api->pWebsocket->callbak_message = api->m_callbak_message;
				api->pWebsocket->run(api->m_uri);
				bool bManualClose = api->pWebsocket->m_manual_close;
				delete api->pWebsocket;
				api->pWebsocket = 0;
				if(bManualClose == false)//�Ƿ�Ϊ�����ر����ӣ���������û������رգ����ӵ��Ͽ����ӻص�ʱ���Զ�ִ���������ӻ��ơ�.
				{
					Sleep(2000);
				}
				else
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
	}
	//::SetEvent(Global::g_hExit);.

	return 0;
}

void OKCoinWebSocketApi::SetCallBackOpen(websocketpp_callbak_open callbak_open)
{
	m_callbak_open = callbak_open;
}

void OKCoinWebSocketApi::SetCallBackClose(websocketpp_callbak_close callbak_close)
{
	m_callbak_close = callbak_close;
}

void OKCoinWebSocketApi::SetCallBackMessage(websocketpp_callbak_message callbak_message)
{
	m_callbak_message = callbak_message;
}

//////////////////////////////////////////////////////////////////////////////////////////////.

//��ȡOKCoin�ֻ���������.
void OKCoinWebSocketApiCn::ok_spotcny_btc_ticker() //���ر���������.
{
	Emit("ok_sub_spotcny_btc_ticker");
	Emit("ok_sub_spotcny_ltc_ticker");
	Emit("ok_sub_spotcny_eth_ticker");
	Emit("ok_sub_spotcny_etc_ticker");
	Emit("ok_sub_spotcny_bcc_ticker");
}
void OKCoinWebSocketApiCn::ok_spotcny_btc_depth_20() //���ر�20���г����.
{
	Emit("ok_sub_spotcny_btc_depth_20");
	Emit("ok_sub_spotcny_ltc_depth_20");
	Emit("ok_sub_spotcny_eth_depth_20");
	Emit("ok_sub_spotcny_etc_depth_20");
	Emit("ok_sub_spotcny_bcc_depth_20");
}
void OKCoinWebSocketApiCn::ok_spotcny_btc_trades() //���ر�ʵʱ�ɽ���¼.
{
	Emit("ok_sub_spotcny_btc_trades");
}
void OKCoinWebSocketApiCn::ok_spotcny_btc_kline_1min() //���ر�K������.
{
	Emit("ok_sub_spotcny_btc_kline_1min");
	Emit("ok_sub_spotcny_ltc_kline_1min");
	Emit("ok_sub_spotcny_eth_kline_1min");
	Emit("ok_sub_spotcny_etc_kline_1min");
	Emit("ok_sub_spotcny_bcc_kline_1min");
}

//��OKCoin�����ֻ�����.
void OKCoinWebSocketApiCn::ok_spotcny_trades() //���Ľ�������.
{
	Parameter prmt;
	prmt.AddParam("api_key",m_api_key);
	string sign = prmt.GetSign(m_secret_key);
	prmt.AddParam("sign",sign);
	string prmtstr = prmt.ToJsonString();
	Emit("ok_sub_spotcny_trades",prmtstr);
	return ;
}

void OKCoinWebSocketApiCn::ok_spotcny_trade(string &symbol,string &type,string &price,string &amount) //�µ�����.
{
	Parameter prmt;
	prmt.AddParam("api_key",m_api_key);
	prmt.AddParam("symbol",symbol);
	prmt.AddParam("type",type);
	prmt.AddParam("price",price);
	prmt.AddParam("amount",amount);

	string sign = prmt.GetSign(m_secret_key);
	prmt.AddParam("sign",sign);
	string prmtstr = prmt.ToJsonString();
	Emit("ok_spotcny_trade",prmtstr);
	return ;
}
void OKCoinWebSocketApiCn::ok_spotcny_cancel_order(string &symbol,string &order_id) //ȡ������.
{
	Parameter prmt;
	prmt.AddParam("api_key",m_api_key);
	prmt.AddParam("symbol",symbol);
	prmt.AddParam("order_id",order_id);
	string sign = prmt.GetSign(m_secret_key);
	prmt.AddParam("sign",sign);
	string prmtstr = prmt.ToJsonString();
	Emit("ok_spotcny_cancel_order",prmtstr);
}

//ȡ������.
void OKCoinWebSocketApiCn::remove_ok_spotcny_btc_ticker() //���ر���������.
{
	Remove("ok_sub_spotcny_btc_ticker");
}

//////////////////////////////////////////////////////////////////////////////////.

//��ȡOKCoin�ֻ���������.
void OKCoinWebSocketApiCom::ok_spotusd_btc_ticker() //���ر���������.
{
	Emit("ok_sub_spotusd_btc_ticker");
}

//��ȡOKCoin��Լ��������.
void OKCoinWebSocketApiCom::ok_futureusd_btc_ticker_this_week() //���رҵ��ܺ�Լ����.
{
	Emit("ok_sub_futureusd_btc_ticker_this_week");
}

void OKCoinWebSocketApiCom::ok_futureusd_btc_index() //���رҺ�Լָ��.
{
	Emit("ok_sub_futureusd_btc_index");
}

//ȡ������.
void OKCoinWebSocketApiCom::remove_ok_spotusd_btc_ticker() //���ر���������.
{
	Remove("ok_sub_spotusd_btc_ticker");
}
